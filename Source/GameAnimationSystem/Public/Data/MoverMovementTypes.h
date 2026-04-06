// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MoverMovementTypes.generated.h"

/**
 * Character gait (walking/running/sprinting) states.
 */
UENUM(BlueprintType)
enum class EGait : uint8
{
	/** Walking gait */
	Walking,
	/** Running gait */
	Running,
	/** Sprinting gait */
	Sprinting
};

/**
 * Movement direction relative to character facing.
 */
UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	/** Moving forward */
	Forward,
	/** Moving backward */
	Backward,
	/** Moving left */
	Left,
	/** Moving right */
	Right,
	/** Moving forward-left diagonal */
	ForwardLeft,
	/** Moving forward-right diagonal */
	ForwardRight,
	/** Moving backward-left diagonal */
	BackwardLeft,
	/** Moving backward-right diagonal */
	BackwardRight
};

/**
 * Movement direction bias for animation blending.
 */
UENUM(BlueprintType)
enum class EMovementDirectionBias : uint8
{
	/** No direction bias */
	None,
	/** Bias towards forward movement */
	Forward,
	/** Bias towards backward movement */
	Backward,
	/** Bias towards strafe movement */
	Strafe
};

/**
 * Mover movement mode for character locomotion.
 * Note: Prefix 'Mover' to avoid conflict with engine EMovementMode.
 */
UENUM(BlueprintType)
enum class EMoverMovementMode : uint8
{
	/** None - no active movement */
	None,
	/** Walking on ground */
	Walking,
	/** NavMesh walking */
	NavMeshWalking,
	/** Falling (in air) */
	Falling,
	/** Swimming in water */
	Swimming,
	/** Flying */
	Flying,
	/** Custom movement mode */
	Custom
};

/**
 * Movement state for animation system.
 */
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	/** Idle - not moving */
	Idle,
	/** Moving - in motion */
	Moving,
	/** In air - jumping or falling */
	InAir,
	/** Landing transition */
	Landing,
	/** Taking off/jump start */
	TakeOff
};

/**
 * Movement direction thresholds for direction detection.
 */
USTRUCT(BlueprintType)
struct FMovementDirectionThresholds
{
	GENERATED_BODY()

	/** Forward threshold angle (degrees) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ForwardThreshold = 0.0f;

	/** Backward threshold angle (degrees) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BackwardThreshold = 180.0f;

	/** Left threshold angle (degrees) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float LeftThreshold = -90.0f;

	/** Right threshold angle (degrees) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RightThreshold = 90.0f;

	/** Diagonal forward-left threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ForwardLeftThreshold = -45.0f;

	/** Diagonal forward-right threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ForwardRightThreshold = 45.0f;

	/** Diagonal backward-left threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BackwardLeftThreshold = -135.0f;

	/** Diagonal backward-right threshold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BackwardRightThreshold = 135.0f;
};