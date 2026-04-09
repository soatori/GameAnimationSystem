// Copyright Soatori Games, Inc. All Rights Reserved.

#include "Character/BaseCharacter_Mover.h"

#include "AIController.h"
#include "Camera/CameraComponent.h"
#include "Character/TraversalLogicComponent.h"
#include "Curves/CurveFloat.h"
#include "Data/GASPDataTypes.h"
#include "DefaultMovementSet/CharacterMoverComponent.h"
#include "DefaultMovementSet/NavMoverComponent.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "HAL/IConsoleManager.h"

ABaseCharacter_Mover::ABaseCharacter_Mover(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create Motion Warping component
	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));

	// Create Traversal Logic component
	TraversalLogic = CreateDefaultSubobject<UTraversalLogicComponent>(TEXT("TraversalLogic"));

	// Create Gameplay Camera component (attached to Mesh, auto-activate off, run in editor false)
	GameplayCamera = CreateDefaultSubobject<UGameplayCameraComponent>(TEXT("GameplayCamera"));
	GameplayCamera->SetupAttachment(GetMesh());
	GameplayCamera->SetAutoActivate(false);
	GameplayCamera->bRunInEditor = false;

	// Create Spring Arm component (attached to Capsule)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());

	// Create Camera component (attached to SpringArm)
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void ABaseCharacter_Mover::BeginPlay()
{
	Super::BeginPlay();

	UCharacterMoverComponent* MoverComp = GetCharacterMover();
	if (MoverComp)
	{
		// Bind OnMovementModeChanged (AssignDelegate = replace existing bindings)
		MoverComp->OnMovementModeChanged.Clear();
		MoverComp->OnMovementModeChanged.AddDynamic(this, &ABaseCharacter_Mover::OnMovementModeChanged);

		// Add OnPreSimulationTick binding (AddDelegate = add to existing bindings)
		MoverComp->OnPreSimulationTick.AddDynamic(this, &ABaseCharacter_Mover::OnPreSimulateTick);
	}

	// Set Tick prerequisite order: SkeletalMesh depends on CharacterMover
	if (GetMesh() && MoverComp)
	{
		GetMesh()->AddTickPrerequisiteComponent(MoverComp);
	}

	// Set Tick prerequisite order: SkeletalMesh depends on this Actor
	if (GetMesh())
	{
		GetMesh()->AddTickPrerequisiteActor(this);
	}

	// Initialize MoverDefaultInputs_PreSim.OrientationIntent to Actor forward vector
	MoverDefaultInputs_PreSim.OrientationIntent = GetActorForwardVector();
}

void ABaseCharacter_Mover::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Call update functions in sequence (BlueprintNativeEvent pattern)
	CacheInputsFromMover();
	Update_FloorValues();
	Update_ControlRotationRate();
	Update_SlidingAudio();
	Update_TargetedActor();
	Update_TwinStickMode();
	DebugDraws();
}

void ABaseCharacter_Mover::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Call client RPC to run setup on owning client
	ClientPossessed();
}

void ABaseCharacter_Mover::ClientPossessed_Implementation()
{
	// Runs on owning client only - setup camera and input
	SetupCamera();
	SetupInput();
}

void ABaseCharacter_Mover::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult)
{
	// Default implementation - can be extended in Blueprint or subclass
}

void ABaseCharacter_Mover::Set_CharacterInputState_Implementation(const FPlayerInputState& InInputState)
{
	PlayerInputState = InInputState;
}

FCharacterPropertiesForAnimation ABaseCharacter_Mover::Get_PropertiesForAnimation_Implementation() const
{
	return FCharacterPropertiesForAnimation();
}

FCharacterPropertiesForCamera ABaseCharacter_Mover::Get_PropertiesForCamera_Implementation() const
{
	return FCharacterPropertiesForCamera();
}

FCharacterPropertiesForTraversal ABaseCharacter_Mover::Get_PropertiesForTraversal_Implementation() const
{
	return FCharacterPropertiesForTraversal();
}

void ABaseCharacter_Mover::SetupInput_Implementation()
{
}

void ABaseCharacter_Mover::SetupCamera_Implementation()
{
}

void ABaseCharacter_Mover::Update_SlidingAudio_Implementation()
{
}

void ABaseCharacter_Mover::Update_TwinStickMode_Implementation()
{
}

void ABaseCharacter_Mover::Update_TargetedActor_Implementation()
{
}

void ABaseCharacter_Mover::Update_ControlRotationRate_Implementation()
{
}

void ABaseCharacter_Mover::Update_FloorValues_Implementation()
{
}

FTraversalCheckInputs ABaseCharacter_Mover::GetTraversalCheckInputs_Implementation() const
{
	FTraversalCheckInputs Result;

	// Get current movement mode
	const EGASPMovementMode MovementMode = Get_CurrentMovementMode();

	// Get CharacterMover component
	UCharacterMoverComponent* MoverComp = GetCharacterMover();
	if (!MoverComp)
	{
		return Result;
	}

	switch (MovementMode)
	{
		case EGASPMovementMode::OnGround:
		case EGASPMovementMode::Sliding:
		case EGASPMovementMode::Traversing:
		{
			// OnGround/Sliding/Traversing logic
			const FVector Velocity = MoverComp->GetVelocity();
			const float SpeedXY = Velocity.Size2D();

			// TraceForwardDirection: velocity direction or target orientation
			if (SpeedXY > 50.0f)
			{
				Result.TraceForwardDirection = Velocity.GetSafeNormal();
			}
			else
			{
				Result.TraceForwardDirection = MoverComp->GetTargetOrientation().Vector();
			}

			// TraceForwardDistance: MapRangeClamped(local velocity X, 0-375 → 75-300)
			const FRotator ActorRotation = GetActorRotation();
			const FVector LocalVelocity = ActorRotation.UnrotateVector(Velocity);
			const float LocalVelocityX = LocalVelocity.X;
			Result.TraceForwardDistance = FMath::GetMappedRangeValueClamped(
				FVector2D(0.0f, 375.0f),
				FVector2D(75.0f, 300.0f),
				LocalVelocityX
			);

			Result.TraceRadius = 30.0f;
			Result.TraceHalfHeight = 60.0f;
			break;
		}

		case EGASPMovementMode::InAir:
		{
			// InAir logic - use move input
			const FVector MoveInput = MoverDefaultInputs_PostSim.GetMoveInput();

			if (!MoveInput.IsNearlyZero())
			{
				Result.TraceForwardDirection = MoveInput.GetSafeNormal();
			}
			else
			{
				Result.TraceForwardDirection = GetActorForwardVector();
			}

			Result.TraceForwardDistance = 75.0f;
			Result.TraceEndOffset = FVector(0.0f, 0.0f, 50.0f);
			Result.TraceRadius = 30.0f;
			Result.TraceHalfHeight = 86.0f;
			break;
		}
	}

	return Result;
}

EGASPMovementMode ABaseCharacter_Mover::Get_CurrentMovementMode_Implementation() const
{
	// Since mover has neither a fixed set of movement modes stored in an Enum like CMC,
	// nor a fixed name for each, Mover simply returns the name of the mode it is in.
	// For convenience, we map these names to Enum values for use across our BP logic.
	
	// Get CharacterMover component
	UCharacterMoverComponent* MoverComp = GetCharacterMover();
	if (!MoverComp)
	{
		return EGASPMovementMode::OnGround;
	}

	// Get current movement mode name from Mover
	const FName MovementModeName = MoverComp->GetMovementModeName();

	// Look up in MovementModeMap
	if (const EGASPMovementMode* FoundMode = MovementModeMap.Find(MovementModeName))
	{
		return *FoundMode;
	}

	// Default fallback if not found in map
	return EGASPMovementMode::OnGround;
}

FRotator ABaseCharacter_Mover::Get_AimingRotation_Implementation() const
{
	// Priority 1: If we have a valid targeted actor, aim toward it
	if (TargetedActor)
	{
		// Calculate direction from pawn to target
		const FVector TargetLocation = TargetedActor->GetActorLocation();
		const FVector PawnLocation = GetActorLocation();
		const FVector DirectionToTarget = TargetLocation - PawnLocation;

		// Convert to rotator and extract only Yaw (keep Pitch/Roll at 0)
		const FRotator FullRotation = UKismetMathLibrary::Conv_VectorToRotator(DirectionToTarget);
		return FRotator(0.0f, FullRotation.Yaw, 0.0f);
	}

	// Priority 2: If in TwinStick mode, use the twin stick aim rotation
	if (bTwinStickMode)
	{
		return TwinStickAimRotation;
	}

	// Priority 3: Default to control rotation (camera rotation)
	return GetControlRotation();
}

ERotationMode ABaseCharacter_Mover::Get_RotationMode_Implementation() const
{
	// Priority 1: If we have a valid targeted actor, force Aim or Strafe based on input state
	if (TargetedActor)
	{
		return PlayerInputState.WantsToAim ? ERotationMode::Aim : ERotationMode::Strafe;
	}

	// Priority 2: TwinStick mode handling
	if (bTwinStickMode)
	{
		// Check if the twin stick aim direction has input (not zero)
		const FVector2D AimDirection = Get_TwinStick_AimDirection();
		if (!AimDirection.IsNearlyZero())
		{
			// Has aim input - use Aim or Strafe based on WantsToAim
			return PlayerInputState.WantsToAim ? ERotationMode::Aim : ERotationMode::Strafe;
		}
		// No aim input - use OrientToMovement
		return ERotationMode::OrientToMovement;
	}

	// Priority 3: Standard mode - call OnRotationIdle first (note: this modifies PlayerInputState)
	// Since this is a const function, we need to cast to call the non-const OnRotationIdle
	// In Blueprint, OnRotationIdle is called before checking PlayerInputState
	// The original BP uses ExecutionSequence to call OnRotationIdle then proceed
	// For C++ const implementation, we rely on OnRotationIdle being called in Tick()
	return PlayerInputState.WantsToAim ? ERotationMode::Aim
		: (PlayerInputState.WantsToStrafe ? ERotationMode::Strafe : ERotationMode::OrientToMovement);
}

FVector ABaseCharacter_Mover::Get_OrientationIntent_Implementation() const
{
	// Get current movement mode
	switch (Get_CurrentMovementMode())
	{
		case EGASPMovementMode::OnGround:
		{
			// Get move input from pre-sim inputs
			const FVector MoveInput = MoverDefaultInputs_PreSim.GetMoveInput();

			if (!MoveInput.IsNearlyZero())
			{
				// Has movement input - determine orientation based on rotation mode
				switch (Get_RotationMode())
				{
					case ERotationMode::OrientToMovement:
					{
						// Return movement input direction
						return MoveInput.GetSafeNormal();
					}

					case ERotationMode::Strafe:
					case ERotationMode::Aim:
					{
						// Use aiming rotation yaw as forward direction
						const FRotator AimingRotation = Get_AimingRotation();
						return FRotator(0.0f, AimingRotation.Yaw, 0.0f).Vector();
					}
				}
			}
			else
			{
				// No movement input - determine orientation based on rotation mode
				switch (Get_RotationMode())
				{
					case ERotationMode::OrientToMovement:
					{
						// Keep current orientation intent (no change)
						return MoverDefaultInputs_PreSim.OrientationIntent;
					}

					case ERotationMode::Strafe:
					case ERotationMode::Aim:
					{
						// Check angle between actor rotation and aiming rotation
						// Turn-in-place behavior when angle exceeds threshold
						const FRotator ActorRotation = GetActorRotation();
						const FRotator AimingRotation = Get_AimingRotation();
						const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(ActorRotation, AimingRotation);
						const float YawDiff = FMath::Abs(DeltaRotation.Yaw);

						// 10 degree threshold for turn-in-place
						if (YawDiff > 10.0f)
						{
							return FRotator(0.0f, AimingRotation.Yaw, 0.0f).Vector();
						}
						// Keep current orientation intent
						return MoverDefaultInputs_PreSim.OrientationIntent;
					}
				}
			}
			break;
		}

		case EGASPMovementMode::InAir:
		{
			switch (Get_RotationMode())
			{
				case ERotationMode::OrientToMovement:
				{
					// Keep current orientation intent (no change)
					return MoverDefaultInputs_PreSim.OrientationIntent;
				}

				case ERotationMode::Strafe:
				case ERotationMode::Aim:
				{
					// Use aiming rotation yaw as forward direction
					const FRotator AimingRotation = Get_AimingRotation();
					return FRotator(0.0f, AimingRotation.Yaw, 0.0f).Vector();
				}
			}
			break;
		}

		case EGASPMovementMode::Sliding:
		{
			switch (Get_RotationMode())
			{
				case ERotationMode::OrientToMovement:
				{
					// Use current velocity direction as orientation intent
					if (UCharacterMoverComponent* MoverComp = GetCharacterMover())
					{
						return MoverComp->GetVelocity().GetSafeNormal();
					}
					return MoverDefaultInputs_PreSim.OrientationIntent;
				}

				case ERotationMode::Strafe:
				case ERotationMode::Aim:
				{
					// Use aiming rotation yaw as forward direction
					const FRotator AimingRotation = Get_AimingRotation();
					return FRotator(0.0f, AimingRotation.Yaw, 0.0f).Vector();
				}
			}
			break;
		}

		case EGASPMovementMode::Traversing:
		{
			// During traversal (active during traversal montages), keep actor forward
			// This prevents the character from trying to rotate during traversal animations
			return GetActorForwardVector();
		}
	}

	// Default fallback - return current orientation intent
	return MoverDefaultInputs_PreSim.OrientationIntent;
}

EGait ABaseCharacter_Mover::Get_Gait_Implementation() const
{
	// Check if player wants to sprint
	if (PlayerInputState.WantsToSprint)
	{
		// Get rotation mode from custom inputs
		switch (MoverCustomInputs_PreSim.RotationMode)
		{
			case ERotationMode::OrientToMovement:
			{
				// OrientToMovement: allow sprint
				return EGait::Sprint;
			}

			case ERotationMode::Strafe:
			{
				// Strafe: sprint only if moving toward orientation intent
				// Get StrafeStyle console variable (0: Forward, 1: Backward, 2: Backward)
				static IConsoleVariable* StrafeStyleCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("DDCvar.StrafeStyle"));
				const int32 StrafeStyle = StrafeStyleCVar ? StrafeStyleCVar->GetInt() : 0;

				// Threshold based on StrafeStyle:
				// 0 (Forward): 0.5 - require forward movement
				// 1/2 (Backward): -0.1 - allow slight backward movement
				constexpr float Thresholds[] = { 0.5f, -0.1f, -0.1f };
				const float Threshold = Thresholds[FMath::Clamp(StrafeStyle, 0, 2)];

				// Calculate dot product between move input and orientation intent
				const FVector MoveInput = MoverDefaultInputs_PreSim.GetMoveInput();
				const FVector OrientationIntent = MoverDefaultInputs_PreSim.OrientationIntent;
				const float DotProduct = FVector::DotProduct(MoveInput, OrientationIntent);

				// If moving toward orientation intent, allow sprint
				if (DotProduct > Threshold)
				{
					return EGait::Sprint;
				}
				return EGait::Run;
			}

			case ERotationMode::Aim:
			{
				// Aim mode: limit gait to run (no sprint while aiming)
				return EGait::Run;
			}
		}
	}

	// Not sprinting - check if player wants to walk
	if (PlayerInputState.WantsToWalk)
	{
		return EGait::Walk;
	}

	// No explicit walk/sprint request - check AnalogInputStyle console variable
	static IConsoleVariable* AnalogInputStyleCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("DDCvar.AnalogInputStyle"));
	
	switch (AnalogInputStyleCVar ? AnalogInputStyleCVar->GetInt() : 0)
	{
		case 0:
		{
			// Fixed speed mode - always run
			return EGait::Run;
		}

		case 1:
		{
			// Variable speed mode - use stick deflection to determine gait
			const FVector2D MoveInput = Get_Move();
			const float StickMagnitude = MoveInput.Size();

			// Threshold: > 0.8 = Run, <= 0.8 = Walk
			constexpr float RunThreshold = 0.8f;
			if (StickMagnitude > RunThreshold)
			{
				return EGait::Run;
			}
			return EGait::Walk;
		}

		default:
		{
			// Default to run
			return EGait::Run;
		}
	}
}

float ABaseCharacter_Mover::Get_Speed_Implementation() const
{
	// Get CharacterMover component
	UCharacterMoverComponent* MoverComp = GetCharacterMover();
	if (!MoverComp)
	{
		return 0.0f;
	}

	// Get velocity and return 2D magnitude (horizontal speed, ignoring Z)
	const FVector Velocity = MoverComp->GetVelocity();
	return Velocity.Size2D();
}

FMovementDirectionThresholds ABaseCharacter_Mover::Get_MovementDirectionThresholds_Implementation() const
{
	// This function returns the thresholds used to determine which MovementDirection
	// the pawn should be in, which in turn determines the rotation offset and which
	// directional anims to play.
	//
	// The thresholds represent angles between the orientation intent and the movement
	// direction with a -180 to 180 range.
	//
	// Console variables control the style:
	// - DDCvar.StrafeStyle (used when RotationMode is OrientToMovement or Strafe)
	// - DDCvar.AimStyle (used when RotationMode is Aim)
	//
	// Style values:
	// 0: Use all 4 directional animations (Forward, Back, Left, Right)
	// 1: Only use Forward and Backward directions (collapses L/R quadrants)
	// 2: Only use Forward direction (always play forward loco anims)

	// Get RotationMode from custom inputs
	const ERotationMode RotationMode = MoverCustomInputs_PreSim.RotationMode;

	// Determine which console variable to read based on RotationMode
	static IConsoleVariable* StrafeStyleCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("DDCvar.StrafeStyle"));
	static IConsoleVariable* AimStyleCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("DDCvar.AimStyle"));

	int32 StyleValue = 0;
	switch (RotationMode)
	{
		case ERotationMode::OrientToMovement:
		case ERotationMode::Strafe:
		{
			StyleValue = StrafeStyleCVar ? StrafeStyleCVar->GetInt() : 0;
			break;
		}
		case ERotationMode::Aim:
		{
			StyleValue = AimStyleCVar ? AimStyleCVar->GetInt() : 0;
			break;
		}
	}

	// Get MovementDirection from custom inputs
	const EMovementDirection MovementDirection = MoverCustomInputs_PreSim.MovementDirection;

	// Define thresholds based on style and movement direction
	FMovementDirectionThresholds Result;

	switch (StyleValue)
	{
		case 0:
		{
			// Style 0: Use all 4 directional animations
			// This is how many games handle strafing - full directional movement
			switch (MovementDirection)
			{
				case EMovementDirection::F:
				case EMovementDirection::B:
				{
					// Forward/Backward: standard thresholds
					Result.FL = -60.0f;
					Result.FR = 60.0f;
					Result.BL = -120.0f;
					Result.BR = 120.0f;
					break;
				}
				case EMovementDirection::LL:
				case EMovementDirection::LR:
				case EMovementDirection::RL:
				case EMovementDirection::RR:
				{
					// Left/Right directions: tighter forward thresholds, wider back thresholds
					Result.FL = -40.0f;
					Result.FR = 40.0f;
					Result.BL = -140.0f;
					Result.BR = 140.0f;
					break;
				}
			}
			break;
		}

		case 1:
		{
			// Style 1: Only use Forward and Backward directions
			// This creates a natural look for non-combat exploration while still
			// allowing the pawn to rotate to face the aiming direction.
			// The F and B thresholds are the same, collapsing L/R quadrants.
			switch (MovementDirection)
			{
				case EMovementDirection::F:
				case EMovementDirection::LL:
				case EMovementDirection::LR:
				case EMovementDirection::RL:
				case EMovementDirection::RR:
				{
					// Collapse to Forward
					Result.FL = -140.0f;
					Result.FR = 140.0f;
					Result.BL = -140.0f;
					Result.BR = 140.0f;
					break;
				}
				case EMovementDirection::B:
				{
					// Backward: symmetric thresholds
					Result.FL = -120.0f;
					Result.FR = 120.0f;
					Result.BL = -120.0f;
					Result.BR = 120.0f;
					break;
				}
			}
			break;
		}

		case 2:
		{
			// Style 2: Only use Forward direction
			// Character will always play forward locomotion anims regardless
			// of strafe direction.
			Result.FL = -180.0f;
			Result.FR = 180.0f;
			Result.BL = -180.0f;
			Result.BR = 180.0f;
			break;
		}

		default:
		{
			// Default to style 0 values
			Result.FL = -60.0f;
			Result.FR = 60.0f;
			Result.BL = -120.0f;
			Result.BR = 120.0f;
			break;
		}
	}

	return Result;
}

EMovementDirection ABaseCharacter_Mover::Get_MovementDirectionFromThresholds_Implementation(
		const FMovementDirectionThresholds& Thresholds, float Direction) const
{
	// Check if Direction is in Forward range [FL, FR]
	// FL is typically negative (e.g., -60), FR is positive (e.g., 60)
	// This covers the forward-facing quadrant
	if (FMath::IsWithinInclusive(Direction, Thresholds.FL, Thresholds.FR))
	{
		return EMovementDirection::F;
	}

	// Check if Direction is in Left range [BL, FL]
	// BL is typically more negative (e.g., -120), FL is the forward-left boundary
	// This covers the left-facing quadrant
	if (FMath::IsWithinInclusive(Direction, Thresholds.BL, Thresholds.FL))
	{
		return EMovementDirection::LR;
	}

	// Check if Direction is in Right range [FR, BR]
	// FR is the forward-right boundary, BR is typically more positive (e.g., 120)
	// This covers the right-facing quadrant
	if (FMath::IsWithinInclusive(Direction, Thresholds.FR, Thresholds.BR))
	{
		return EMovementDirection::RL;
	}

	// Default to Backward for all other cases
	// This covers the backward-facing quadrants (angles outside all defined ranges)
	return EMovementDirection::B;
}

void ABaseCharacter_Mover::Get_MovementDirectionAndOffset_Implementation(EMovementDirection& MovementDirection, float& RotationOffset)
{
	// Default output values
	MovementDirection = EMovementDirection::F;
	RotationOffset = 0.0f;

	// Early exit: If RotationMode is OrientToMovement, return F with no offset
	// This allows the character to rotate freely toward movement direction
	const ERotationMode RotationMode = MoverCustomInputs_PreSim.RotationMode;
	if (RotationMode == ERotationMode::OrientToMovement)
	{
		DebugAngle = 0.0f;
		return;
	}

	// ============================================================
	// Step 1: Calculate Direction of Movement
	// For OnGround: use MoveInput
	// For InAir/Sliding: use velocity (due to low friction)
	// For Traversing: use zero vector (handled by early exit above)
	// ============================================================

	FVector DirectionOfMovement = FVector::ZeroVector;
	const EGASPMovementMode MovementMode = Get_CurrentMovementMode();

	switch (MovementMode)
	{
		case EGASPMovementMode::OnGround:
		{
			// OnGround: use MoveInput from pre-sim inputs
			DirectionOfMovement = MoverDefaultInputs_PreSim.GetMoveInput();
			break;
		}

		case EGASPMovementMode::InAir:
		case EGASPMovementMode::Sliding:
		{
			// InAir/Sliding: use normalized velocity (low friction modes)
			if (UCharacterMoverComponent* MoverComp = GetCharacterMover())
			{
				DirectionOfMovement = MoverComp->GetVelocity().GetSafeNormal();
			}
			break;
		}

		case EGASPMovementMode::Traversing:
		{
			// Traversing: no movement direction during traversal animations
			DirectionOfMovement = FVector::ZeroVector;
			break;
		}
	}

	// If DirectionOfMovement is zero, return default values
	if (DirectionOfMovement.IsNearlyZero())
	{
		DebugAngle = 0.0f;
		return;
	}

	// ============================================================
	// Step 2: Calculate Movement Direction Angle
	// Angle between DirectionOfMovement and OrientationIntent
	// Range: -180 to 180 degrees
	// ============================================================

	// Convert vectors to rotators
	const FRotator DirectionRotator = UKismetMathLibrary::Conv_VectorToRotator(DirectionOfMovement);
	const FRotator OrientationRotator = UKismetMathLibrary::Conv_VectorToRotator(MoverDefaultInputs_PreSim.OrientationIntent);

	// Calculate normalized delta (Direction - Orientation)
	const FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(DirectionRotator, OrientationRotator);
	float MovementDirectionAngle = DeltaRotator.Yaw;

	// Store raw angle for debugging
	DebugAngle = MovementDirectionAngle;

	// ============================================================
	// Step 3: Clamp Angle to Prevent Constant Flipping at 180°
	// When near ±180°, we clamp to prevent rapid direction changes
	// Only applies when current MovementDirection is F
	// ============================================================

	const EMovementDirection CurrentMovementDirection = MoverCustomInputs_PreSim.MovementDirection;

	if (CurrentMovementDirection == EMovementDirection::F)
	{
		// Check if angle is in the critical zones near ±180°
		// Zone 1: [170, 180] -> clamp to -179
		// Zone 2: [-180, -170] -> clamp to 179
		if (FMath::IsWithinInclusive(MovementDirectionAngle, 170.0f, 180.0f))
		{
			MovementDirectionAngle = -179.0f;
		}
		else if (FMath::IsWithinInclusive(MovementDirectionAngle, -180.0f, -170.0f))
		{
			MovementDirectionAngle = 179.0f;
		}
	}
	else
	{
		// For non-F directions, use ActorToOrientationDelta as bias
		// Step 1: Get ActorRotation vs OrientationIntent delta
		const FRotator ActorRotation = GetActorRotation();
		const FRotator ActorToOrientationDelta = UKismetMathLibrary::NormalizedDeltaRotator(ActorRotation, OrientationRotator);

		// Step 2: Check if PreviousRotationOffset != 0
		const float PreviousRotationOffset = MoverCustomInputs_PreSim.RotationOffset;
		const float BiasAngle = (PreviousRotationOffset != 0.0f) ? ActorToOrientationDelta.Yaw : 0.0f;

		// Step 3: If BiasAngle > 0, use it as new MovementDirectionAngle
		if (BiasAngle > 0.0f)
		{
			MovementDirectionAngle = BiasAngle;
		}
	}

	// Update debug angle after clamping
	DebugAngle = MovementDirectionAngle;

	// ============================================================
	// Step 4: Get Movement Direction from Thresholds
	// Based on the angle and configured thresholds
	// ============================================================

	const FMovementDirectionThresholds Thresholds = Get_MovementDirectionThresholds();
	EMovementDirection NewMovementDirection = Get_MovementDirectionFromThresholds(Thresholds, MovementDirectionAngle);

	// ============================================================
	// Step 5: Sprint Fallback
	// Force MovementDirection to F when sprinting (no strafe sprint coverage)
	// ============================================================

	const EGait Gait = Get_Gait();
	if (Gait == EGait::Sprint)
	{
		NewMovementDirection = EMovementDirection::F;
	}

	// ============================================================
	// Step 6: Calculate Rotation Offset
	// Use curve evaluation based on MovementMode and MovementDirection
	// The offset is applied to the pawn's rotation relative to OrientationIntent
	// ============================================================

	// Get the rotation offset curve via the Blueprint-implementable function
	if (UCurveFloat* RotationOffsetCurve = Get_RotationOffsetCurve(MovementMode, NewMovementDirection))
	{
		// Evaluate curve at the movement direction angle
		// X-axis: angle (-180 to 180)
		// Y-axis: rotation offset value
		RotationOffset = RotationOffsetCurve->GetFloatValue(MovementDirectionAngle);
	}
	else
	{
		// Default: no offset if no curve is provided
		RotationOffset = 0.0f;
	}

	// Set output
	MovementDirection = NewMovementDirection;
}

UCurveFloat* ABaseCharacter_Mover::Get_RotationOffsetCurve_Implementation(EGASPMovementMode MovementMode, EMovementDirection MovementDirection) const
{
	// Default implementation returns nullptr
	// Override in Blueprint to return appropriate curve from Chooser table
	return nullptr;
}

void ABaseCharacter_Mover::CacheInputsFromMover_Implementation()
{
	// Get CharacterMover component
	UCharacterMoverComponent* MoverComp = GetCharacterMover();
	if (!MoverComp)
	{
		return;
	}

	// Get last input command context from Mover
	// This contains the input data that was most recently processed by Mover
	const FMoverInputCmdContext& LastInputCmd = MoverComp->GetLastInputCmd();

	// Get input collection from context
	const FMoverDataCollection& InputCollection = LastInputCmd.InputCollection;

	// Extract CharacterDefaultInputs from collection
	// This is the standard movement input struct used by Mover
	if (const FCharacterDefaultInputs* DefaultInputs = InputCollection.FindDataByType<FCharacterDefaultInputs>())
	{
		MoverDefaultInputs_PostSim = *DefaultInputs;
	}

	// Extract custom inputs from collection
	// This contains GASP-specific data like rotation mode, gait, movement direction
	if (const FMoverCustomInputs* CustomInputs = InputCollection.FindDataByType<FMoverCustomInputs>())
	{
		MoverCustomInputs_PostSim = *CustomInputs;
	}
}

void ABaseCharacter_Mover::OnMovementModeChanged_Implementation(const FName& PreviousMovementModeName, const FName& NewMovementModeName)
{
	// ============================================================
	// Convert FName movement mode names to enum values
	// ============================================================

	EGASPMovementMode PreviousMovementMode = EGASPMovementMode::OnGround;
	EGASPMovementMode NewMovementMode = EGASPMovementMode::OnGround;

	// Look up PreviousMovementMode in MovementModeMap
	if (const EGASPMovementMode* FoundMode = MovementModeMap.Find(PreviousMovementModeName))
	{
		PreviousMovementMode = *FoundMode;
	}

	// Look up NewMovementMode in MovementModeMap
	if (const EGASPMovementMode* FoundMode = MovementModeMap.Find(NewMovementModeName))
	{
		NewMovementMode = *FoundMode;
	}

	// Get CharacterMover component for velocity and state queries
	UCharacterMoverComponent* MoverComp = GetCharacterMover();

	// ============================================================
	// Branch 1: Handle transitions based on NewMovementMode
	// ============================================================

	switch (NewMovementMode)
	{
		case EGASPMovementMode::OnGround:
		{
			// Landing: Handle based on PreviousMovementMode
			switch (PreviousMovementMode)
			{
				case EGASPMovementMode::InAir:
				{
						// TODO: Play foley event "Foley.Event.Land" with volume based on fall velocity
						break;
				}

				default:
				{
					break;
				}
			}
			break;
		}

		case EGASPMovementMode::InAir:
		{
			// Entering air: Handle based on PreviousMovementMode
			switch (PreviousMovementMode)
			{
				case EGASPMovementMode::OnGround:
				{
					// Check if jump was just pressed - play jump foley
					if (MoverDefaultInputs_PreSim.bIsJumpJustPressed)
					{
							// TODO: Play foley event "Foley.Event.Jump" with volume based on horizontal speed
					}
					break;
				}

				default:
				{
					break;
				}
			}

			// Cancel crouch when entering air
			if (MoverComp && MoverComp->IsCrouching())
			{
				PlayerInputState.WantsToCrouch = false;
				MoverComp->UnCrouch();
			}
			break;
		}

		case EGASPMovementMode::Sliding:
		{
			// TODO: Play foley event "Foley.Event.Slide.Loop" and store returned AudioComponent in SlidingAudioComponent
			break;
		}

		case EGASPMovementMode::Traversing:
		{
			// No action needed for traversing
			break;
		}
	}

	// ============================================================
	// Branch 2: Handle transitions based on PreviousMovementMode
	// ============================================================

	switch (PreviousMovementMode)
	{
		case EGASPMovementMode::OnGround:
		{
			// No action for exiting OnGround
			break;
		}

		case EGASPMovementMode::InAir:
		{
			// No action for exiting InAir
			break;
		}

		case EGASPMovementMode::Sliding:
		{
			// TODO: Check if SlidingAudioComponent is valid, then FadeOut(0.5f, 0.0f)

			// If sprinting, cancel crouch
			if (PlayerInputState.WantsToSprint)
			{
				PlayerInputState.WantsToCrouch = false;
			}
			break;
		}

		case EGASPMovementMode::Traversing:
		{
			// No action for exiting Traversing
			break;
		}
	}
}

void ABaseCharacter_Mover::OnPreSimulateTick_Implementation(const FMoverTimeStep& TimeStep, const FMoverInputCmdContext& InputCmd)
{
	// Get CharacterMover component
	UCharacterMoverComponent* MoverComp = GetCharacterMover();
	if (!MoverComp)
	{
		return;
	}

	// Get InputCollection from InputCmd
	const FMoverDataCollection& InputCollection = InputCmd.InputCollection;

	// Extract FMoverCustomInputs form collection
	FMoverCustomInputs CustomInputs;
	if (const FMoverCustomInputs* FoundInputs = InputCollection.FindDataByType<FMoverCustomInputs>())
	{
		CustomInputs = *FoundInputs;
	}

	// Apply crouch state based on WantsToCrouch
	if (CustomInputs.WantsToCrouch)
	{
		MoverComp->Crouch();
	}
	else
	{
		MoverComp->UnCrouch();
	}
}

bool ABaseCharacter_Mover::Jump_Implementation()
{
	// Check if not currently doing traversal action
	if (!TraversalLogic || TraversalLogic->bDoingTraversalAction)
	{
		return false;
	}

	// Get traversal check inputs
	FTraversalCheckInputs Inputs = GetTraversalCheckInputs();

	// Try traversal action
	bool TraversalCheckFailed = false;
	bool MontageSelectionFailed = false;
	TraversalLogic->TryTraversalAction(Inputs, EDrawDebugTrace::ForOneFrame, TraversalCheckFailed, MontageSelectionFailed);

	// Return true if either check failed (jump failed)
	return TraversalCheckFailed || MontageSelectionFailed;
}

FVector2D ABaseCharacter_Mover::Get_Move_Implementation() const
{
	// TODO: Implement move input retrieval logic
	return FVector2D::ZeroVector;
}

FVector ABaseCharacter_Mover::Get_MoveInput_Implementation() const
{
	// Check if controller is an AIController
	if (Cast<AAIController>(GetController()))
	{
		// AI-controlled: use NavMover's navigation data
		if (UNavMoverComponent* NavMoverComp = GetNavMover())
		{
			FVector OutMoveInputIntent;
			FVector OutMoveInputVelocity;
			NavMoverComp->ConsumeNavMovementData(OutMoveInputIntent, OutMoveInputVelocity);
			return OutMoveInputVelocity.GetSafeNormal();
		}
		return FVector::ZeroVector;
	}

	// Player-controlled: use player input
	const FVector2D Move2D = Get_Move();
	const FVector MoveVector(Move2D.X, Move2D.Y, 0.0f);

	// Clamp magnitude to 0-1
	const FVector ClampedVector = MoveVector.GetClampedToSize(0.0f, 1.0f);

	// Rotate by control rotation (yaw only)
	const FRotator ControlRotation = GetControlRotation();
	const FRotator YawOnlyRotation(0.0f, ControlRotation.Yaw, 0.0f);
	const FVector RotatedVector = YawOnlyRotation.RotateVector(ClampedVector);

	// Normalize and return
	return RotatedVector.GetSafeNormal();
}

FVector2D ABaseCharacter_Mover::Get_TwinStick_AimDirection_Implementation() const
{
	// TODO: Implement twin-stick aim direction retrieval logic
	return FVector2D::ZeroVector;
}

void ABaseCharacter_Mover::OnRotationIdle_Implementation()
{
	// Early exit if auto idle is not enabled
	if (!IdleSet.IsAutoIdle)
	{
		return;
	}

	// Only process OnGround movement mode
	const EGASPMovementMode MovementMode = Get_CurrentMovementMode();
	if (MovementMode != EGASPMovementMode::OnGround)
	{
		return;
	}

	// Get current game time
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	// Check 1: If there is move input, update last move time and enable strafe
	const FVector MoveInput = MoverDefaultInputs_PreSim.GetMoveInput();
	if (!MoveInput.IsNearlyZero())
	{
		IdleSet.LastMoveTime = CurrentTime;
		PlayerInputState.WantsToStrafe = true;
		return;
	}

	// Check 2: If aiming, update last move time and enable strafe
	if (PlayerInputState.WantsToAim)
	{
		IdleSet.LastMoveTime = CurrentTime;
		PlayerInputState.WantsToStrafe = true;
		return;
	}

	// Check 3: If idle for longer than InTime threshold, disable strafe
	const float ElapsedTime = CurrentTime - IdleSet.LastMoveTime;
	if (ElapsedTime > IdleSet.InTime)
	{
		PlayerInputState.WantsToStrafe = false;
		return;
	}

	// Check 4: If rotation difference exceeds threshold (>10° yaw), enable strafe
	const FRotator ActorRotation = GetActorRotation();
	const FRotator AimingRotation = Get_AimingRotation();
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(ActorRotation, AimingRotation);
	const float YawDiff = FMath::Abs(DeltaRotation.Yaw);

	if (YawDiff > 10.0f)
	{
		IdleSet.LastMoveTime = CurrentTime;
		PlayerInputState.WantsToStrafe = true;
	}
}

void ABaseCharacter_Mover::DebugDraws_Implementation()
{
}

void ABaseCharacter_Mover::Look_Implementation(const FVector2D& ActionValue)
{
	AddControllerYawInput(ActionValue.X);
	AddControllerPitchInput(ActionValue.Y);
}

void ABaseCharacter_Mover::LookGamepad_Implementation(const FVector2D& ActionValue)
{
	// Only process if NOT in TwinStick mode
	if (!bTwinStickMode)
	{
		const float DeltaTime = GetWorld()->GetDeltaSeconds();
		const FVector2D ScaledInput = ActionValue * DeltaTime;
		AddControllerYawInput(ScaledInput.X);
		AddControllerPitchInput(ScaledInput.Y);
	}
}

void ABaseCharacter_Mover::Walk_Implementation()
{
	// Only toggle walk if not sprinting
	if (!PlayerInputState.WantsToSprint)
	{
		PlayerInputState.WantsToWalk = !PlayerInputState.WantsToWalk;
	}
}

void ABaseCharacter_Mover::Sprint_Implementation(bool ActionValue)
{
	// Set sprint state
	PlayerInputState.WantsToSprint = ActionValue;

	// If starting sprint while crouching, stop crouching
	if (ActionValue)
	{
		UCharacterMoverComponent* MoverComp = GetCharacterMover();
		if (MoverComp && MoverComp->IsCrouching())
		{
			PlayerInputState.WantsToCrouch = false;
		}
	}
}

void ABaseCharacter_Mover::JumpFailed_Implementation(bool IsFailed)
{
	if (IsFailed)
	{
		UCharacterMoverComponent* MoverComp = GetCharacterMover();
		if (MoverComp && MoverComp->IsCrouching())
		{
			// If crouching, stop crouching
			PlayerInputState.WantsToCrouch = false;
		}
		else
		{
			// Set jump pressed flag for one frame
			bJump_JustPressed = true;
			// Reset next frame via latent action
			GetWorldTimerManager().SetTimerForNextTick([this]()
			{
				bJump_JustPressed = false;
			});
		}
	}
}

void ABaseCharacter_Mover::Crouch_Implementation()
{
	// Get current movement mode
	const EGASPMovementMode MovementMode = Get_CurrentMovementMode();

	// Check if OnGround OR InAir
	if (MovementMode == EGASPMovementMode::OnGround || MovementMode == EGASPMovementMode::Sliding)
	{
		// Use GetCharacterMover() to access the private CharacterMover member
		if (UCharacterMoverComponent* MoverComp = GetCharacterMover())
		{
			if (MoverComp->IsCrouching())
			{
				// Already crouching -> stop crouching
				PlayerInputState.WantsToCrouch = false;
			}
			else
			{
				// Not crouching -> start crouching
				PlayerInputState.WantsToCrouch = true;
			}
		}
	}
}

void ABaseCharacter_Mover::Strafe_Implementation()
{
	// Toggle strafe state
	PlayerInputState.WantsToStrafe = !PlayerInputState.WantsToStrafe;
}

void ABaseCharacter_Mover::Aim_Implementation(bool ActionValue)
{
	// Set aim state
	PlayerInputState.WantsToAim = ActionValue;
}

void ABaseCharacter_Mover::CameraDown_Implementation()
{
}

void ABaseCharacter_Mover::CameraUp_Implementation()
{
}