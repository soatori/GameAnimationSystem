// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GASPInputTypes.generated.h"

/**
 * Player input state.
 */
USTRUCT(BlueprintType)
struct FPlayerInputState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WantsToSprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WantsToWalk = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WantsToStrafe = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WantsToAim = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool WantsToCrouch = false;
};