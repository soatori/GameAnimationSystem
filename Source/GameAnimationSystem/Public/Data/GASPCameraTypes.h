// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GASPCameraTypes.generated.h"

/**
 * Camera movement mode types.
 */
UENUM(BlueprintType)
enum class ECameraMode : uint8
{
	FreeCam,
	Strafe,
	Aim,
	TwinStick
};

/**
 * Camera distance/style types.
 */
UENUM(BlueprintType)
enum class ECameraStyle : uint8
{
	Close,
	Medium,
	Far,
	Debug
};

/**
 * Character properties for camera system.
 */
USTRUCT(BlueprintType)
struct FCharacterPropertiesForCamera
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECameraStyle CameraStyle = ECameraStyle::Medium;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECameraMode CameraMode = ECameraMode::FreeCam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Gait = 0; // EGait - stored as uint8 to avoid dependency on GASPMovementTypes

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Stance = 0; // EStance - stored as uint8 to avoid dependency on GASPMovementTypes
};