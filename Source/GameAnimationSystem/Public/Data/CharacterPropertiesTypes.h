// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MoverMovementTypes.h"
#include "MoverAnimationTypes.h"
#include "CharacterPropertiesTypes.generated.h"

/**
 * Character properties passed to animation system for animation selection.
 */
USTRUCT(BlueprintType)
struct FCharacterPropertiesForAnimation
{
	GENERATED_BODY()

	/** Character velocity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FVector Velocity = FVector::ZeroVector;

	/** Character acceleration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FVector Acceleration = FVector::ZeroVector;

	/** Character rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FRotator Rotation = FRotator::ZeroRotator;

	/** Aim rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FRotator AimRotation = FRotator::ZeroRotator;

	/** Current gait */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	EGait Gait = EGait::Walking;

	/** Current stance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	EStance Stance = EStance::Standing;

	/** Current rotation mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	ERotationMode RotationMode = ERotationMode::VelocityDirection;

	/** Current movement state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	EMovementState MovementState = EMovementState::Idle;

	/** Current movement direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	EMovementDirection MovementDirection = EMovementDirection::Forward;

	/** Whether character is in air */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool bIsInAir = false;

	/** Speed percentage (0-1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	float SpeedPercentage = 0.0f;
};

/**
 * Character properties passed to camera system.
 */
USTRUCT(BlueprintType)
struct FCharacterPropertiesForCamera
{
	GENERATED_BODY()

	/** Character position */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FVector Position = FVector::ZeroVector;

	/** Character velocity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FVector Velocity = FVector::ZeroVector;

	/** Character rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FRotator Rotation = FRotator::ZeroRotator;

	/** Current stance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	EStance Stance = EStance::Standing;

	/** Current gait */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	EGait Gait = EGait::Walking;

	/** Camera target offset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FVector CameraOffset = FVector::ZeroVector;

	/** Camera distance multiplier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	float DistanceMultiplier = 1.0f;

	/** Whether camera should follow rotation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	bool bFollowRotation = true;
};

/**
 * Debug graph line properties for visualization.
 */
USTRUCT(BlueprintType)
struct FDebugGraphLineProperties
{
	GENERATED_BODY()

	/** Line color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FLinearColor LineColor = FLinearColor::White;

	/** Line thickness */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float LineThickness = 1.0f;

	/** Line start point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FVector2D StartPoint = FVector2D::ZeroVector;

	/** Line end point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FVector2D EndPoint = FVector2D::ZeroVector;

	/** Whether line is visible */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bIsVisible = true;

	/** Line label */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FString Label = TEXT("");
};