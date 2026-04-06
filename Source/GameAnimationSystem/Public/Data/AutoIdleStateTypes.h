// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AutoIdleStateTypes.generated.h"

/**
 * Auto idle state configuration for idle animation blending.
 */
USTRUCT(BlueprintType)
struct FAutoIdleState
{
	GENERATED_BODY()

	/** Current idle blend weight */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float BlendWeight = 0.0f;

	/** Time remaining in current idle state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float TimeRemaining = 0.0f;

	/** Whether auto idle is enabled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsEnabled = false;
};