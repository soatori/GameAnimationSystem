// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MoverTypes.h"
#include "MoverDataModelTypes.h"
#include "MoverSimulationTypes.h"
#include "Character/MoverCharacter.h"
#include "Character/GASPCharacterPawnInterface.h"
#include "Data/GASPDataTypes.h"

#include "BaseCharacter_Mover.generated.h"

class AActor;
class UMotionWarpingComponent;
class UTraversalLogicComponent;
class USpringArmComponent;
class UCameraComponent;
class UGameplayCameraComponent;

/**
 * Base character using Mover movement system with input production capability.
 */
UCLASS()
class GAMEANIMATIONSYSTEM_API ABaseCharacter_Mover : public AMoverCharacter, public IGASPCharacterPawnInterface, public IMoverInputProducerInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter_Mover(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;

	/** Client RPC called when possessed - runs on owning client only */
	UFUNCTION(Client, Reliable)
	void ClientPossessed();

	/** IMoverInputProducerInterface implementation */
	virtual void ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult) override;

	/** IGASPCharacterPawnInterface implementation */
	virtual void Set_CharacterInputState_Implementation(const FPlayerInputState& InInputState) override;
	virtual FCharacterPropertiesForAnimation Get_PropertiesForAnimation_Implementation() const override;
	virtual FCharacterPropertiesForCamera Get_PropertiesForCamera_Implementation() const override;
	virtual FCharacterPropertiesForTraversal Get_PropertiesForTraversal_Implementation() const override;

	// ============================================================
	// Default Functions
	// ============================================================

	/** Setup input bindings */
	UFUNCTION(BlueprintNativeEvent, Category = "Default")
	void SetupInput();

	/** Setup camera components */
	UFUNCTION(BlueprintNativeEvent, Category = "Default")
	void SetupCamera();

	/** Update sliding audio effects */
	UFUNCTION(BlueprintNativeEvent, Category = "Default")
	void Update_SlidingAudio();

	/** Update twin stick mode state */
	UFUNCTION(BlueprintNativeEvent, Category = "Default")
	void Update_TwinStickMode();

	/** Update targeted actor */
	UFUNCTION(BlueprintNativeEvent, Category = "Default")
	void Update_TargetedActor();

	/** Update control rotation rate */
	UFUNCTION(BlueprintNativeEvent, Category = "Default")
	void Update_ControlRotationRate();

	/** Update floor normal and location */
	UFUNCTION(BlueprintNativeEvent, Category = "Default")
	void Update_FloorValues();

	// ============================================================
	// Traversal Functions
	// ============================================================

	/** Get traversal check inputs */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Traversal")
	FTraversalCheckInputs GetTraversalCheckInputs() const;

	// ============================================================
	// Mover Functions
	// ============================================================

	/** Get current movement mode */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mover")
	EGASPMovementMode Get_CurrentMovementMode() const;

	/** Get aiming rotation */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mover")
	FRotator Get_AimingRotation() const;

	/** Get rotation mode */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mover")
	ERotationMode Get_RotationMode() const;

	/** Get orientation intent vector */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mover")
	FVector Get_OrientationIntent() const;

	/** Get gait */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mover")
	EGait Get_Gait() const;

	/** Get speed */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mover")
	float Get_Speed() const;

	/** Get movement direction thresholds */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mover")
	FMovementDirectionThresholds Get_MovementDirectionThresholds() const;

	/** Get movement direction from thresholds */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mover")
	EMovementDirection Get_MovementDirectionFromThresholds(const FMovementDirectionThresholds& Thresholds, float Direction) const;

	/** Get movement direction and rotation offset */
	UFUNCTION(BlueprintNativeEvent, Category = "Mover")
	void Get_MovementDirectionAndOffset(EMovementDirection& MovementDirection, float& RotationOffset);

	/** Cache inputs from mover system */
	UFUNCTION(BlueprintNativeEvent, Category = "Mover")
	void CacheInputsFromMover();

	/** Called when movement mode changed */
	UFUNCTION(BlueprintNativeEvent, Category = "Mover")
	void OnMovementModeChanged(const FName& PreviousMovementModeName, const FName& NewMovementModeName);

	/** Called before simulate tick */
	UFUNCTION(BlueprintNativeEvent, Category = "Mover")
	void OnPreSimulateTick(const FMoverTimeStep& TimeStep, const FMoverInputCmdContext& InputCmd);

	// ============================================================
	// Input Functions
	// ============================================================

	/** Handle jump input, returns true if jump was triggered */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Input")
	bool Jump() const;

	/** Get move input as 2D vector */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Input")
	FVector2D Get_Move() const;

	/** Get twin stick aim direction */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Input")
	FVector2D Get_TwinStick_AimDirection() const;

	/** Called when rotation is idle */
	UFUNCTION(BlueprintNativeEvent, Category = "Input")
	void OnRotationIdle();

	// ============================================================
	// Debug Functions
	// ============================================================

	/** Draw debug visuals */
	UFUNCTION(BlueprintNativeEvent, Category = "Debug")
	void DebugDraws();

	// ============================================================
	// Blueprint Event Functions
	// ============================================================

	/** Handle look input from mouse */
	UFUNCTION(BlueprintNativeEvent, Category = "Control")
	void Look(const FVector2D& ActionValue);

	/** Handle look input from gamepad */
	UFUNCTION(BlueprintNativeEvent, Category = "Control")
	void LookGamepad(const FVector2D& ActionValue);

	/** Toggle walk mode */
	UFUNCTION(BlueprintNativeEvent, Category = "Locomotion")
	void Walk();

	/** Toggle sprint mode */
	UFUNCTION(BlueprintNativeEvent, Category = "Locomotion")
	void Sprint(bool ActionValue);

	/** Handle jump failed event */
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void JumpFailed(bool IsFailed);

	/** Handle crouch input */
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void Crouch();

	/** Toggle strafe mode */
	UFUNCTION(BlueprintNativeEvent, Category = "Aim")
	void Strafe();

	/** Toggle aim mode */
	UFUNCTION(BlueprintNativeEvent, Category = "Aim")
	void Aim(bool ActionValue);

	/** Move camera down */
	UFUNCTION(BlueprintNativeEvent, Category = "Camera")
	void CameraDown();

	/** Move camera up */
	UFUNCTION(BlueprintNativeEvent, Category = "Camera")
	void CameraUp();

	// ============================================================
	// Components
	// ============================================================

	/** Motion Warping component for traversal actions */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMotionWarpingComponent> MotionWarping;

	/** Traversal Logic component for ledge/climb actions */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTraversalLogicComponent> TraversalLogic;

	/** Gameplay Camera component attached to Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Camera")
	TObjectPtr<UGameplayCameraComponent> GameplayCamera;

	/** Spring Arm component attached to Capsule */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	/** Camera component attached to SpringArm */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Camera")
	TObjectPtr<UCameraComponent> Camera;

	// ============================================================
	// Default
	// ============================================================

	/** Array of actors that can be targeted */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TArray<TObjectPtr<AActor>> TargetableActors;

	/** Currently targeted actor */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<AActor> TargetedActor;

	// ============================================================
	// Mover
	// ============================================================

	/** Default inputs before simulation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mover")
	FCharacterDefaultInputs MoverDefaultInputs_PreSim;

	/** Custom inputs before simulation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mover")
	FMoverCustomInputs MoverCustomInputs_PreSim;

	/** Default inputs after simulation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mover")
	FCharacterDefaultInputs MoverDefaultInputs_PostSim;

	/** Custom inputs after simulation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mover")
	FMoverCustomInputs MoverCustomInputs_PostSim;

	/** Map of movement mode names to movement modes */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mover")
	TMap<FName, EGASPMovementMode> MovementModeMap;

	/** Normal of the floor surface */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mover")
	FVector FloorNormal = FVector::ZeroVector;

	/** Location on the floor */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mover")
	FVector FloorLocation = FVector::ZeroVector;

	// ============================================================
	// Input
	// ============================================================

	/** Whether using common input mode */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	bool bIsCommonInput = false;

	/** Auto idle state configuration */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	FAutoIdleState IdleSet;

	/** Current player input state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	FPlayerInputState PlayerInputState;

	/** Jump was just pressed this frame */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	bool bJump_JustPressed = false;

	/** Whether twin stick mode is active */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	bool bTwinStickMode = false;

	/** Aim rotation for twin stick mode */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	FRotator TwinStickAimRotation = FRotator::ZeroRotator;

	/** Last control rotation */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	FRotator LastControlRotation = FRotator::ZeroRotator;

	/** Rate of control rotation change */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	float ControlRotationRate = 0.0f;

	// ============================================================
	// Debug
	// ============================================================

	/** History of speed values for debugging */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	TArray<float> SpeedHistory;

	/** Debug angle value */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	float DebugAngle = 0.0f;

};