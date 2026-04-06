// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputTypes.generated.h"

/**
 * Player input state for input processing and buffering.
 */
USTRUCT(BlueprintType)
struct FPlayerInputState
{
	GENERATED_BODY()

	/** Movement input vector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FVector MovementInput = FVector::ZeroVector;

	/** Look input vector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FVector LookInput = FVector::ZeroVector;

	/** Whether jump input is active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bJumpPressed = false;

	/** Whether crouch input is active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bCrouchPressed = false;

	/** Whether sprint input is active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bSprintPressed = false;

	/** Input timestamp */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float InputTimestamp = 0.0f;

	/** Input hold duration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float HoldDuration = 0.0f;
};

/**
 * Mover custom inputs for extended movement system functionality.
 */
USTRUCT(BlueprintType)
struct FMoverCustomInputs
{
	GENERATED_BODY()

	/** Desired movement direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FVector DesiredMovementDirection = FVector::ZeroVector;

	/** Desired facing direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	FRotator DesiredFacingRotation = FRotator::ZeroRotator;

	/** Movement speed modifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float SpeedModifier = 1.0f;

	/** Whether auto rotate is enabled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool bAutoRotate = false;

	/** Custom movement mode request */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	uint8 CustomMovementMode = 0;

	/** Movement intent flags */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int32 MovementIntentFlags = 0;
};