// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GASPAutoIdleTypes.generated.h"

/**
 * Auto idle state configuration.
 */
USTRUCT(BlueprintType)
struct FAutoIdleState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsAutoIdle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LastMoveTime = 0.0f;
};