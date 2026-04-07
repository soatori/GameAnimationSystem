// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/SplineComponent.h"
#include "Data/GASPTraversalTypes.h"

#include "TraversableLedgesInterface.generated.h"

UINTERFACE(Blueprintable)
class UTraversableLedgesInterface : public UInterface
{
	GENERATED_BODY()
};


class GAMEANIMATIONSYSTEM_API ITraversableLedgesInterface
{
	GENERATED_BODY()

public:

	/**
	 * Get ledge transforms from hit location and actor location.
	 * @param HitLocation The location of the hit.
	 * @param ActorLocation The location of the actor.
	 * @param TraversalTraceResultInOut The traversal trace result to process and update.
	 * @return The updated traversal check result with ledge transforms.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Transforms")
	FTraversalCheckResult GetLedgeTransforms(FVector HitLocation, FVector ActorLocation, FTraversalCheckResult& TraversalTraceResultInOut);
	virtual FTraversalCheckResult GetLedgeTransforms_Implementation(FVector HitLocation, FVector ActorLocation, FTraversalCheckResult& TraversalTraceResultInOut) { return FTraversalCheckResult(); }

	/**
	 * Find the ledge spline closest to the actor.
	 * @param ActorLocation The location of the actor to find the closest ledge from.
	 * @return The spline component representing the closest ledge.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Transforms")
	USplineComponent* FindLedgeClosestToActor(const FVector& ActorLocation);
	virtual USplineComponent* FindLedgeClosestToActor_Implementation(const FVector& ActorLocation) { return nullptr; }

	/**
	 * Check if the ledge is traversable.
	 * @return True if the ledge can be traversed.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Transforms")
	bool IsTraversable();
	virtual bool IsTraversable_Implementation() { return false; }
};