// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MoverMovementTypes.h"
#include "MoverAnimationTypes.h"
#include "TraversalTypes.generated.h"

/**
 * Traversal action types for parkour/movement mechanics.
 */
UENUM(BlueprintType)
enum class ETraversalActionType : uint8
{
	/** No traversal action */
	None,
	/** Mantle over obstacle */
	Mantle,
	/** Vault over obstacle */
	Vault,
	/** Climb up */
	ClimbUp,
	/** Climb down */
	ClimbDown,
	/** Drop down */
	DropDown,
	/** Jump across gap */
	JumpAcross,
	/** Slide under obstacle */
	Slide,
	/** Wall run */
	WallRun
};

/**
 * Traversal check inputs for action feasibility testing.
 */
USTRUCT(BlueprintType)
struct FTraversalCheckInputs
{
	GENERATED_BODY()

	/** Character position for check */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	FVector CharacterPosition = FVector::ZeroVector;

	/** Character velocity for check */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	FVector CharacterVelocity = FVector::ZeroVector;

	/** Character capsule half height */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float CapsuleHalfHeight = 96.0f;

	/** Character capsule radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float CapsuleRadius = 34.0f;

	/** Desired traversal action type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	ETraversalActionType DesiredAction = ETraversalActionType::None;

	/** Look direction for traversal check */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	FVector LookDirection = FVector::ForwardVector;
};

/**
 * Traversal check result from feasibility testing.
 */
USTRUCT(BlueprintType)
struct FTraversalCheckResult
{
	GENERATED_BODY()

	/** Whether traversal action is possible */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	bool bCanPerform = false;

	/** Recommended traversal action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	ETraversalActionType RecommendedAction = ETraversalActionType::None;

	/** Target position for traversal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	FVector TargetPosition = FVector::ZeroVector;

	/** Required motion duration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float MotionDuration = 0.0f;

	/** Height difference for traversal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float HeightDifference = 0.0f;

	/** Obstacle height */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float ObstacleHeight = 0.0f;

	/** Obstacle width */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float ObstacleWidth = 0.0f;
};

/**
 * Traversal chooser inputs for animation selection.
 */
USTRUCT(BlueprintType)
struct FTraversalChooserInputs
{
	GENERATED_BODY()

	/** Traversal action type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	ETraversalActionType ActionType = ETraversalActionType::None;

	/** Character stance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	EStance Stance = EStance::Standing;

	/** Height category for traversal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float HeightCategory = 0.0f;

	/** Speed category for traversal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float SpeedCategory = 0.0f;

	/** Direction of traversal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	FVector TraversalDirection = FVector::ForwardVector;
};

/**
 * Traversal chooser outputs for animation selection.
 */
USTRUCT(BlueprintType)
struct FTraversalChooserOutputs
{
	GENERATED_BODY()

	/** Selected traversal animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	TObjectPtr<UAnimationAsset> SelectedAnimation = nullptr;

	/** Animation blend weight */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float BlendWeight = 1.0f;

	/** Play rate for animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float PlayRate = 1.0f;

	/** Whether valid animation was found */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	bool bValidSelection = false;
};

/**
 * Character properties specifically for traversal mechanics.
 */
USTRUCT(BlueprintType)
struct FCharacterPropertiesForTraversal
{
	GENERATED_BODY()

	/** Character current velocity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	FVector Velocity = FVector::ZeroVector;

	/** Character current position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	FVector Position = FVector::ZeroVector;

	/** Character rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	FRotator Rotation = FRotator::ZeroRotator;

	/** Character capsule half height */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float CapsuleHalfHeight = 96.0f;

	/** Character capsule radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float CapsuleRadius = 34.0f;

	/** Current gait */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	EGait Gait = EGait::Walking;

	/** Current stance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	EStance Stance = EStance::Standing;

	/** Whether character is grounded */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	bool bIsGrounded = true;
};