// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MoverTypes.h"

#include "GASPMovementTypes.generated.h"

/**
 * Movement behavior for analog stick input.
 */
UENUM(BlueprintType)
enum class EAnalogStickBehavior : uint8
{
	/** Character will move at a fixed speed regardless of stick deflection. */
	FixedSpeedSingleGait UMETA(DisplayName = "Fixed Speed - Single Gait"),

	/** Character will move at a fixed walking speed with slight stick deflection, and a fixed running speed at full stick deflection. */
	FixedSpeedWalkRun UMETA(DisplayName = "Fixed Speed - Walk / Run"),

	/** Full analog movement control with stick, character will remain walking or running based on gait input. */
	VariableSpeedSingleGait UMETA(DisplayName = "Variable Speed - Single Gait"),

	/** Full analog movement control with stick, character will switch from walk to run gait based on stick deflection. */
	VariableSpeedWalkRun UMETA(DisplayName = "Variable Speed - Walk / Run")
};

/**
 * Character gait types.
 */
UENUM(BlueprintType)
enum class EGait : uint8
{
	Walk,
	Run,
	Sprint
};

/**
 * Movement direction for animation blending.
 */
UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	F,
	B,
	LL,
	LR,
	RL,
	RR
};

/**
 * Movement direction bias based on foot positioning.
 */
UENUM(BlueprintType)
enum class EMovementDirectionBias : uint8
{
	LeftFootForward,
	RightFootForward
};

/**
 * GASP-specific movement mode categories.
 */
UENUM(BlueprintType)
enum class EGASPMovementMode : uint8
{
	OnGround,
	InAir,
	Sliding,
	Traversing
};

/**
 * Character movement state.
 */
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle,
	Moving
};

/**
 * Character rotation mode.
 */
UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	OrientToMovement,
	Strafe,
	Aim
};

/**
 * Character stance types.
 */
UENUM(BlueprintType)
enum class EStance : uint8
{
	Stand,
	Crouch
};

/**
 * Movement direction thresholds.
 */
USTRUCT(BlueprintType)
struct FMovementDirectionThresholds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FL = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FR = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BL = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BR = 0.0f;
};

/**
 * Mover custom inputs for GASP-specific movement data.
 * Must inherit from FMoverDataStructBase to work with Mover data collection.
 */
USTRUCT(BlueprintType)
struct FMoverCustomInputs : public FMoverDataStructBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERotationMode RotationMode = ERotationMode::OrientToMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGait Gait = EGait::Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovementDirection MovementDirection = EMovementDirection::F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ControlRotationRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WantsToCrouch = false;

	// FMoverDataStructBase overrides
	virtual FMoverDataStructBase* Clone() const override
	{
		return new FMoverCustomInputs(*this);
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
};

// Template specialization for NetSerialize support
template<>
struct TStructOpsTypeTraits<FMoverCustomInputs> : public TStructOpsTypeTraitsBase2<FMoverCustomInputs>
{
	enum
	{
		WithNetSerializer = true
	};
};