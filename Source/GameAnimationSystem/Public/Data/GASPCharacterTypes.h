// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GASPMovementTypes.h"
#include "GASPInputTypes.h"
#include "GASPCameraTypes.h"

#include "GASPCharacterTypes.generated.h"

/**
 * Character properties for animation system.
 */
USTRUCT(BlueprintType)
struct FCharacterPropertiesForAnimation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerInputState InputState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EMovementMode> MovementMode = MOVE_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStance Stance = EStance::Stand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERotationMode RotationMode = ERotationMode::OrientToMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGait Gait = EGait::Walk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovementDirection MovementDirection = EMovementDirection::F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform ActorTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector InputAcceleration = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentMaxAcceleration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentMaxDeceleration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator OrientationIntent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator AimingRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bJustLanded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LandVelocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SteeringTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector GroundNormal = FVector::UpVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector GroundLocation = FVector::ZeroVector;
};

/**
 * Input parameters for rotation offset curve chooser.
 */
USTRUCT(BlueprintType)
struct FRotationOffsetCurveChooser_Inputs
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGASPMovementMode MovementMode = EGASPMovementMode::OnGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovementDirection MovementDirection = EMovementDirection::F;
};