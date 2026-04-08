// Copyright Soatori Games, Inc. All Rights Reserved.

#include "Character/TraversalLogicComponent.h"
#include "Character/GASPCharacterPawnInterface.h"
#include "Character/TraversableLedgesInterface.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "DefaultMovementSet/CharacterMoverComponent.h"
#include "MoverComponent.h"
#include "MotionWarpingComponent.h"
#include "AnimationWarpingLibrary.h"
#include "ChooserFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TraversalLogicComponent)

UTraversalLogicComponent::UTraversalLogicComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UTraversalLogicComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get owner and retrieve traversal properties via interface
	AActor* OwnerActor = GetOwner();
	if (OwnerActor && OwnerActor->GetClass()->ImplementsInterface(UGASPCharacterPawnInterface::StaticClass()))
	{
		CharacterProperties = IGASPCharacterPawnInterface::Execute_Get_PropertiesForTraversal(OwnerActor);
	}

	// Cast owner to Pawn and get CharacterMoverComponent
	if (APawn* OwnerPawn = Cast<APawn>(OwnerActor))
	{
		if (UCharacterMoverComponent* MoverComponent = OwnerPawn->GetComponentByClass<UCharacterMoverComponent>())
		{
			Mover = MoverComponent;
		}
	}
}

bool UTraversalLogicComponent::TryTraversalAction(
	const FTraversalCheckInputs& Inputs,
	EDrawDebugTrace::Type DebugType,
	bool& TraversalCheckFailed,
	bool& MontageSelectionFailed)
{
	// Initialize output parameters
	TraversalCheckFailed = false;
	MontageSelectionFailed = false;

	// Get owner actor
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		TraversalCheckFailed = true;
		return false;
	}

	// ============================================================
	// Step 1: Cache important values
	// ============================================================
	UCapsuleComponent* Capsule = CharacterProperties.Capsule;
	if (!Capsule)
	{
		TraversalCheckFailed = true;
		return false;
	}

	const FVector ActorLocation = OwnerActor->GetActorLocation();
	const float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
	const float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();

	// Use input values or fallback to capsule dimensions
	const float TraceRadius = Inputs.TraceRadius > 0.0f ? Inputs.TraceRadius : CapsuleRadius;
	const float TraceHalfHeight = Inputs.TraceHalfHeight > 0.0f ? Inputs.TraceHalfHeight : CapsuleHalfHeight;

	// Initialize result structure
	FTraversalCheckResult TraversalCheckResult;

	// Debug drawing
	const bool bDrawDebug = (DebugType != EDrawDebugTrace::None);

	// ============================================================
	// Step 2.1: Forward Capsule Trace - Find Traversable Level Block
	// ============================================================
	const FVector TraceStart = ActorLocation + Inputs.TraceOriginOffset;
	const FVector TraceEnd = TraceStart + Inputs.TraceForwardDirection * Inputs.TraceForwardDistance;

	FHitResult ForwardHit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerActor);

	const bool bForwardHit = UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		TraceStart,
		TraceEnd,
		TraceRadius,
		TraceHalfHeight,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel3), // TraceTypeQuery3
		false, // bTraceComplex
		ActorsToIgnore,
		bDrawDebug ? DebugType : EDrawDebugTrace::None,
		ForwardHit,
		true // bIgnoreSelf
	);

	if (!bForwardHit)
	{
		TraversalCheckFailed = true;
		return false;
	}

	// Check if hit actor implements TraversableLedgesInterface
	AActor* HitActor = ForwardHit.GetActor();
	UPrimitiveComponent* HitComponent = ForwardHit.GetComponent();

	if (!HitActor || !HitActor->GetClass()->ImplementsInterface(UTraversableLedgesInterface::StaticClass()))
	{
		TraversalCheckFailed = true;
		return false;
	}

	// Check if the ledge is traversable
	if (!ITraversableLedgesInterface::Execute_IsTraversable(HitActor))
	{
		TraversalCheckFailed = true;
		return false;
	}

	// ============================================================
	// Step 2.2: Get front and back ledge transforms
	// ============================================================
	TraversalCheckResult.HitComponent = HitComponent;
	TraversalCheckResult = ITraversableLedgesInterface::Execute_GetLedgeTransforms(
		HitActor,
		ForwardHit.ImpactPoint,
		ActorLocation,
		TraversalCheckResult);

	// ============================================================
	// Step 3.1: Check front ledge validity
	// ============================================================
	if (!TraversalCheckResult.HasFrontLedge)
	{
		TraversalCheckFailed = true;
		return false;
	}

	// ============================================================
	// Step 3.2: Vertical space check - Room to move up to front ledge
	// ============================================================
	FVector HasRoomCheck_FrontLedgeLocation = TraversalCheckResult.FrontLedgeLocation;
	FVector HasRoomCheck_BackLedgeLocation = TraversalCheckResult.BackLedgeLocation;

	FHitResult VerticalHit;
	const bool bVerticalBlocked = UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		ActorLocation,
		HasRoomCheck_FrontLedgeLocation,
		CapsuleRadius,
		CapsuleHalfHeight,
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
		false,
		ActorsToIgnore,
		bDrawDebug ? DebugType : EDrawDebugTrace::None,
		VerticalHit,
		true
	);

	if (bVerticalBlocked && VerticalHit.bBlockingHit)
	{
		TraversalCheckFailed = true;
		return false;
	}

	// ============================================================
	// Step 3.3: Save obstacle height
	// ============================================================
	TraversalCheckResult.ObstacleHeight = HasRoomCheck_FrontLedgeLocation.Z - ActorLocation.Z;

	// ============================================================
	// Step 3.4: Top traversal space check - Room to move across
	// ============================================================
	FHitResult TopSweepResult;
	const FVector TopTraceStart = HasRoomCheck_FrontLedgeLocation + FVector(0.0f, 0.0f, CapsuleHalfHeight);
	const FVector TopTraceEnd = HasRoomCheck_BackLedgeLocation + FVector(0.0f, 0.0f, CapsuleHalfHeight);

	const bool bTopBlocked = UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		TopTraceStart,
		TopTraceEnd,
		CapsuleRadius,
		CapsuleHalfHeight,
		UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
		false,
		ActorsToIgnore,
		bDrawDebug ? DebugType : EDrawDebugTrace::None,
		TopSweepResult,
		true
	);

	// ============================================================
	// Step 3.5: Save obstacle depth
	// ============================================================
	if (bTopBlocked && TopSweepResult.bBlockingHit)
	{
		// No room - use trace impact point
		TraversalCheckResult.ObstacleDepth = FVector::Dist2D(HasRoomCheck_FrontLedgeLocation, TopSweepResult.ImpactPoint);
		TraversalCheckResult.HasBackLedge = false;
	}
	else
	{
		// Has room - use distance between ledges
		TraversalCheckResult.ObstacleDepth = FVector::Dist2D(HasRoomCheck_FrontLedgeLocation, HasRoomCheck_BackLedgeLocation);
	}

	// ============================================================
	// Step 3.6: Back floor check - Find floor below back ledge
	// ============================================================
	if (TraversalCheckResult.HasBackLedge)
	{
		const FVector FloorTraceStart = HasRoomCheck_BackLedgeLocation;
		const FVector FloorTraceEnd = HasRoomCheck_BackLedgeLocation - FVector(0.0f, 0.0f, TraversalCheckResult.ObstacleHeight + CapsuleHalfHeight);

		FHitResult FloorHit;
		const bool bFloorHit = UKismetSystemLibrary::LineTraceSingle(
			this,
			FloorTraceStart,
			FloorTraceEnd,
			UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
			false,
			ActorsToIgnore,
			bDrawDebug ? DebugType : EDrawDebugTrace::None,
			FloorHit,
			true
		);

		if (bFloorHit)
		{
			TraversalCheckResult.HasBackFloor = true;
			TraversalCheckResult.BackFloorLocation = FloorHit.Location;
			TraversalCheckResult.BackLedgeHeight = HasRoomCheck_BackLedgeLocation.Z - FloorHit.Location.Z;
		}
		else
		{
			TraversalCheckResult.HasBackFloor = false;
		}
	}

	// ============================================================
	// Determine ActionType based on obstacle properties
	// ============================================================
	// Simple heuristic - can be refined or moved to ChooserTable
	if (TraversalCheckResult.HasBackLedge && TraversalCheckResult.HasBackFloor)
	{
		// Has back floored - can land on other side
		if (TraversalCheckResult.ObstacleHeight < 100.0f)
		{
			TraversalCheckResult.ActionType = ETraversalActionType::Hurdle;
		}
		else
		{
			TraversalCheckResult.ActionType = ETraversalActionType::Vault;
		}
	}
	else
	{
		// No back floor - mantle onto obstacle
		TraversalCheckResult.ActionType = ETraversalActionType::Mantle;
	}

	// ============================================================
	// Step 5.1: Check ActionType validity
	// ============================================================
	if (TraversalCheckResult.ActionType == ETraversalActionType::None)
	{
		TraversalCheckFailed = true;
		return false;
	}

	// ============================================================
	// Step 5.2: Montage selection via Chooser
	// ============================================================
	if (MontageChooserTable)
	{
		UObject* SelectedObject = UChooserFunctionLibrary::EvaluateChooser(
			this,
			MontageChooserTable,
			UAnimMontage::StaticClass()
		);

		TraversalCheckResult.ChosenMontage = Cast<UAnimMontage>(SelectedObject);
	}

	if (!TraversalCheckResult.ChosenMontage)
	{
		MontageSelectionFailed = true;
		return false;
	}

	// Store the result
	TraversalResult = TraversalCheckResult;

	// ============================================================
	// Trigger Traversal Event
	// ============================================================
	PerformTraversalAction();

	return true;
}

void UTraversalLogicComponent::PerformTraversalAction()
{
	// Play the chosen montage using UE's PlayMoverMontageCallbackProxy
	if (TraversalResult.ChosenMontage && Mover)
	{
		// Create the proxy object using UE's built-in function
		ActiveMontageProxy = UPlayMoverMontageCallbackProxy::CreateProxyObjectForPlayMoverMontage(
			Mover,
			TraversalResult.ChosenMontage,
			TraversalResult.PlayRate,
			TraversalResult.StartTime,
			NAME_None);

		if (ActiveMontageProxy != nullptr)
		{
			// Bind callbacks to the proxy's delegates
			ActiveMontageProxy->OnCompleted.AddDynamic(this, &UTraversalLogicComponent::OnTraversalMontageCompleted);
			ActiveMontageProxy->OnBlendOut.AddDynamic(this, &UTraversalLogicComponent::OnTraversalMontageBlendOut);
			ActiveMontageProxy->OnInterrupted.AddDynamic(this, &UTraversalLogicComponent::OnTraversalMontageInterrupted);
			ActiveMontageProxy->OnNotifyBegin.AddDynamic(this, &UTraversalLogicComponent::OnTraversalNotifyBegin);
			ActiveMontageProxy->OnNotifyEnd.AddDynamic(this, &UTraversalLogicComponent::OnTraversalNotifyEnd);
		}
	}

	// Set traversal action state
	bDoingTraversalAction = true;

	// Set warp targets for motion warping
	SetWarpTargets();

	// Ignore collision with hit component during traversal
	if (TraversalResult.HitComponent && CharacterProperties.Capsule)
	{
		CharacterProperties.Capsule->IgnoreComponentWhenMoving(TraversalResult.HitComponent, true);
	}

	// Set movement mode to Flying during traversal
	SetMovementMode(MOVE_Flying);

	// Server RPC to sync traversal state
	PerformTraversalAction_Server(TraversalResult);
}

void UTraversalLogicComponent::PerformTraversalAction_Server_Implementation(const FTraversalCheckResult& InTraversalResult)
{
	PerformTraversalAction_Clients(InTraversalResult);
}

void UTraversalLogicComponent::PerformTraversalAction_Clients_Implementation(const FTraversalCheckResult& InTraversalResult)
{

	// Only execute traversal action logic on client side
	// Server already executed PerformTraversalAction() locally before calling this RPC
	if (!GetOwner()->HasAuthority())
	{
		// Set traversal result on all clients
		TraversalResult = InTraversalResult;

		PerformTraversalAction();
	}
}

void UTraversalLogicComponent::SetWarpTargets()
{
	// Get MotionWarping component from CharacterProperties
	UMotionWarpingComponent* MotionWarping = CharacterProperties.MotionWarping;
	if (!MotionWarping)
	{
		return;
	}

	// ============================================================
	// Update FrontLedge warp target
	// Location is FrontLedgeLocation with small Z offset (0.5)
	// Rotation faces away from the ledge (using negated normal)
	// ============================================================

	FVector FrontLedgeTargetLocation = TraversalResult.FrontLedgeLocation + FVector(0.0f, 0.0f, 0.5f);
	FRotator FrontLedgeTargetRotation = FRotationMatrix::MakeFromX(-TraversalResult.FrontLedgeNormal).Rotator();

	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(
		FName("FrontLedge"),
		FrontLedgeTargetLocation,
		FrontLedgeTargetRotation);

	// ============================================================
	// Handle BackLedge warp target (for Vault or Hurdle actions)
	// ============================================================

	const bool bIsVaultOrHurdle = (TraversalResult.ActionType == ETraversalActionType::Vault ||
	                               TraversalResult.ActionType == ETraversalActionType::Hurdle);

	if (bIsVaultOrHurdle)
	{
		// Get motion warping windows for BackLedge from the chosen montage
		TArray<FMotionWarpingWindowData> BackLedgeWindows;
		UMotionWarpingUtilities::GetMotionWarpingWindowsForWarpTargetFromAnimation(
			TraversalResult.ChosenMontage,
			FName("BackLedge"),
			BackLedgeWindows);

		if (BackLedgeWindows.Num() > 0)
		{
			// Get the end time of the warp window
			const float WindowEndTime = BackLedgeWindows[0].EndTime;

			// Get the animated distance from the curve at the end time
			float AnimatedDistanceFromFrontLedgeToBackLedge = 0.0f;
			UAnimationWarpingLibrary::GetCurveValueFromAnimation(
				TraversalResult.ChosenMontage,
				FName("Distance_From_Ledge"),
				WindowEndTime,
				AnimatedDistanceFromFrontLedgeToBackLedge);

			// Update BackLedge warp target
			MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(
				FName("BackLedge"),
				TraversalResult.BackLedgeLocation,
				FRotator::ZeroRotator);
		}
		else
		{
			// No warp window found, remove the target
			MotionWarping->RemoveWarpTarget(FName("BackLedge"));
		}
	}
	else
	{
		// Not a vault or hurdle, remove BackLedge target
		MotionWarping->RemoveWarpTarget(FName("BackLedge"));
	}

	// ============================================================
	// Handle BackFloor warp target (for Hurdle action only)
	// ============================================================

	if (TraversalResult.ActionType == ETraversalActionType::Hurdle)
	{
		// Get motion warping windows for BackFloor from the chosen montage
		TArray<FMotionWarpingWindowData> BackFloorWindows;
		UMotionWarpingUtilities::GetMotionWarpingWindowsForWarpTargetFromAnimation(
			TraversalResult.ChosenMontage,
			FName("BackFloor"),
			BackFloorWindows);

		if (BackFloorWindows.Num() > 0)
		{
			// Get the end time of the warp window
			const float WindowEndTime = BackFloorWindows[0].EndTime;

			// Get the animated distance from the curve at the end time
			float AnimatedDistanceFromFrontLedgeToBackFloor = 0.0f;
			UAnimationWarpingLibrary::GetCurveValueFromAnimation(
				TraversalResult.ChosenMontage,
				FName("Distance_From_Ledge"),
				WindowEndTime,
				AnimatedDistanceFromFrontLedgeToBackFloor);

			// Get the distance for BackLedge (already computed above for hurdle)
			float AnimatedDistanceFromFrontLedgeToBackLedge = 0.0f;
			TArray<FMotionWarpingWindowData> BackLedgeWindows;
			UMotionWarpingUtilities::GetMotionWarpingWindowsForWarpTargetFromAnimation(
				TraversalResult.ChosenMontage,
				FName("BackLedge"),
				BackLedgeWindows);

			if (BackLedgeWindows.Num() > 0)
			{
				UAnimationWarpingLibrary::GetCurveValueFromAnimation(
					TraversalResult.ChosenMontage,
					FName("Distance_From_Ledge"),
					BackLedgeWindows[0].EndTime,
					AnimatedDistanceFromFrontLedgeToBackLedge);
			}

			// Calculate the horizontal offset based on the difference in animated distances
			// The character lands at a different distance based on the animation
			const float DistanceDifference = FMath::Abs(AnimatedDistanceFromFrontLedgeToBackLedge - AnimatedDistanceFromFrontLedgeToBackFloor);
			const FVector HorizontalOffset = TraversalResult.BackLedgeNormal * DistanceDifference;

			// BackFloor location: use BackLedgeLocation + offset for X/Y, BackFloorLocation for Z
			const FVector BackFloorTargetLocation(
				TraversalResult.BackLedgeLocation.X + HorizontalOffset.X,
				TraversalResult.BackLedgeLocation.Y + HorizontalOffset.Y,
				TraversalResult.BackFloorLocation.Z);

			MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation(
				FName("BackFloor"),
				BackFloorTargetLocation,
				FRotator::ZeroRotator);
		}
		else
		{
			// No warp window found, remove the target
			MotionWarping->RemoveWarpTarget(FName("BackFloor"));
		}
	}
	else
	{
		// Not a hurdle, remove BackFloor target
		MotionWarping->RemoveWarpTarget(FName("BackFloor"));
	}
}

void UTraversalLogicComponent::SetMovementMode(EMovementMode NewMovementMode)
{
	if (!Mover)
	{
		return;
	}

	// Convert EMovementMode to Mover mode name
	FName DesiredModeName;
	switch (NewMovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
	case MOVE_Swimming:
	case MOVE_Custom:
		DesiredModeName = FName("Walking");
		break;
	case MOVE_Falling:
		DesiredModeName = FName("Falling");
		break;
	case MOVE_Flying:
		DesiredModeName = FName("Flying");
		break;
	default:
		DesiredModeName = FName("Walking");
		break;
	}

	Mover->QueueNextMode(DesiredModeName, false);
}

void UTraversalLogicComponent::OnTraversalMontageCompleted(FName NotifyName)
{
	// Reset traversal state
	bDoingTraversalAction = false;

	// Restore collision with hit component
	if (TraversalResult.HitComponent && CharacterProperties.Capsule)
	{
		CharacterProperties.Capsule->IgnoreComponentWhenMoving(TraversalResult.HitComponent, false);
	}

	// Determine movement mode based on action type
	EMovementMode NewMovementMode;
	switch (TraversalResult.ActionType)
	{
	case ETraversalActionType::Vault:
		NewMovementMode = MOVE_Falling;
		break;
	case ETraversalActionType::Hurdle:
	case ETraversalActionType::Mantle:
		NewMovementMode = MOVE_Walking;
		break;
	default:
		NewMovementMode = MOVE_Walking;
		break;
	}

	// Set movement mode
	SetMovementMode(NewMovementMode);

	// Clear proxy reference
	ActiveMontageProxy = nullptr;
}

void UTraversalLogicComponent::OnTraversalMontageBlendOut(FName NotifyName)
{
	// Called when montage starts blending out (not interrupted)
	// Perform cleanup same as completed
	OnTraversalMontageCompleted(NotifyName);
}

void UTraversalLogicComponent::OnTraversalMontageInterrupted(FName NotifyName)
{
	// Called when montage was interrupted (or failed to play)
	// Perform cleanup same as completed
	OnTraversalMontageCompleted(NotifyName);
}

void UTraversalLogicComponent::OnTraversalNotifyBegin(FName NotifyName)
{
	// Called when a PlayMontageNotify or PlayMontageNotifyWindow Notify begins
	// Can be used for sync points like footstep sounds, effects, etc.
}

void UTraversalLogicComponent::OnTraversalNotifyEnd(FName NotifyName)
{
	// Called when a PlayMontageNotifyWindow Notify ends
}