// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MoverMovementTypes.h"
#include "MoverAnimationTypes.generated.h"

/**
 * Analog stick behavior modes for movement input processing.
 */
UENUM(BlueprintType)
enum class EAnalogStickBehavior : uint8
{
	/** Absolute movement direction - direct input mapping */
	Absolute,
	/** Relative movement direction - relative to character orientation */
	Relative,
	/** Fixed movement direction - locked to specific direction */
	Fixed
};

/**
 * Experimental state machine states for animation system testing.
 */
UENUM(BlueprintType)
enum class EExperimentalStateMachineState : uint8
{
	/** Entry state */
	Entry,
	/** Active state */
	Active,
	/** Exit state */
	Exit,
	/** Transition state */
	Transition
};

/**
 * Rotation mode for character orientation control.
 */
UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	/** Velocity direction - character faces movement direction */
	VelocityDirection,
	/** Looking direction - character faces camera look direction */
	LookingDirection,
	/** Aiming - character faces aiming target */
	Aiming,
	/** Manual - manual rotation control */
	Manual
};

/**
 * Character stance states.
 */
UENUM(BlueprintType)
enum class EStance : uint8
{
	/** Standing stance */
	Standing,
	/** Crouching stance */
	Crouching,
	/** Prone stance */
	Prone
};

/**
 * Blend stack inputs for layered animation blending.
 */
USTRUCT(BlueprintType)
struct FBlendStackInputs
{
	GENERATED_BODY()

	/** Blend weight for this layer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float BlendWeight = 1.0f;

	/** Layer index in blend stack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	int32 LayerIndex = 0;

	/** Whether this layer is active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsActive = true;
};

/**
 * Chooser outputs for animation selection system.
 */
USTRUCT(BlueprintType)
struct FChooserOutputs
{
	GENERATED_BODY()

	/** Selected animation asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<UAnimationAsset> SelectedAnimation = nullptr;

	/** Selected blend weight */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float BlendWeight = 1.0f;

	/** Whether a valid animation was chosen */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bValidSelection = false;
};

/**
 * Rotation offset curve chooser inputs for dynamic rotation blending.
 */
USTRUCT(BlueprintType)
struct FRotationOffsetCurveChooserInputs
{
	GENERATED_BODY()

	/** Current gait type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	EGait Gait = EGait::Walking;

	/** Current movement direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	EMovementDirection MovementDirection = EMovementDirection::Forward;

	/** Current stance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	EStance Stance = EStance::Standing;

	/** Speed threshold for rotation offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float SpeedThreshold = 0.0f;
};