// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MotionWarpingComponent.h"

#include "GASPMovementTypes.h"
#include "GASPAnimationTypes.h"

#include "GASPTraversalTypes.generated.h"

/**
 * Input parameters for traversal check.
 */
USTRUCT(BlueprintType)
struct FTraversalCheckInputs
{
	GENERATED_BODY()

	/** Direction to trace forward. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TraceForwardDirection = FVector::ForwardVector;

	/** Distance to trace forward. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceForwardDistance = 0.0f;

	/** Offset for trace origin. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TraceOriginOffset = FVector::ZeroVector;

	/** Offset for trace end. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TraceEndOffset = FVector::ZeroVector;

	/** Radius for trace capsule. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceRadius = 0.0f;

	/** Half height for trace capsule. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceHalfHeight = 0.0f;
};

/**
 * Result of traversal check.
 */
USTRUCT(BlueprintType)
struct FTraversalCheckResult
{
	GENERATED_BODY()

	/** Type of traversal action. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETraversalActionType ActionType = ETraversalActionType::None;

	/** Whether there is a front ledge. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasFrontLedge = false;

	/** Location of the front ledge. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FrontLedgeLocation = FVector::ZeroVector;

	/** Normal of the front ledge. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FrontLedgeNormal = FVector::UpVector;

	/** Whether there is a back ledge. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasBackLedge = false;

	/** Location of the back ledge. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector BackLedgeLocation = FVector::ZeroVector;

	/** Normal of the back ledge. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector BackLedgeNormal = FVector::UpVector;

	/** Whether there is back floor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasBackFloor = false;

	/** Location of the back floor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector BackFloorLocation = FVector::ZeroVector;

	/** Height of the obstacle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ObstacleHeight = 0.0f;

	/** Depth of the obstacle. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ObstacleDepth = 0.0f;

	/** Height of the back ledge. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BackLedgeHeight = 0.0f;

	/** Component that was hit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPrimitiveComponent* HitComponent = nullptr;

	/** Chosen animation montage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ChosenMontage = nullptr;

	/** Start time for the animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartTime = 0.0f;

	/** Play rate for the animation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayRate = 1.0f;
};

/**
 * Input parameters for traversal chooser.
 */
USTRUCT(BlueprintType)
struct FTraversalChooserInputs
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETraversalActionType ActionType = ETraversalActionType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasFrontLedge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasBackLedge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasBackFloor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ObstacleHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ObstacleDepth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BackLedgeHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceToLedge = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGASPMovementMode MovementMode = EGASPMovementMode::OnGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGait Gait = EGait::Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPoseHistory PoseHistory;
};

/**
 * Output from traversal chooser.
 */
USTRUCT(BlueprintType)
struct FTraversalChooserOutputs
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETraversalActionType ActionType = ETraversalActionType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MontageStartTime = 0.0f;
};

/**
 * Character properties for traversal system.
 */
USTRUCT(BlueprintType)
struct FCharacterPropertiesForTraversal
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Capsule = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMotionWarpingComponent* MotionWarping = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGASPMovementMode MovementMode = EGASPMovementMode::OnGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGait Gait = EGait::Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 0.0f;
};