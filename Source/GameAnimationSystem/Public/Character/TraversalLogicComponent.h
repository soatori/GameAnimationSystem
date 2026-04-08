// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/GASPTraversalTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimInstance.h"
#include "MoveLibrary/PlayMoverMontageCallbackProxy.h"

class UChooserTable;

#include "TraversalLogicComponent.generated.h"

class UCharacterMoverComponent;
class UAnimMontage;

UCLASS(Blueprintable, ClassGroup = "Movement", meta = (BlueprintSpawnableComponent))
class GAMEANIMATIONSYSTEM_API UTraversalLogicComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UTraversalLogicComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool TryTraversalAction(
		const FTraversalCheckInputs& Inputs,
		EDrawDebugTrace::Type DebugType,
		bool& TraversalCheckFailed,
		bool& MontageSelectionFailed);

	UFUNCTION(BlueprintCallable)
	void PerformTraversalAction();

	UFUNCTION(Server, Reliable)
	void PerformTraversalAction_Server(const FTraversalCheckResult& InTraversalResult);

	UFUNCTION(NetMulticast, Reliable)
	void PerformTraversalAction_Clients(const FTraversalCheckResult& InTraversalResult);

	UFUNCTION(BlueprintCallable)
	void SetWarpTargets();

	UFUNCTION(BlueprintCallable)
	void SetMovementMode(EMovementMode NewMovementMode);

	// --- Montage Callbacks (bound to PlayMoverMontageCallbackProxy) ---

	/** Called when traversal montage finished playing and wasn't interrupted */
	UFUNCTION()
	void OnTraversalMontageCompleted(FName NotifyName);

	/** Called when traversal montage starts blending out and is not interrupted */
	UFUNCTION()
	void OnTraversalMontageBlendOut(FName NotifyName);

	/** Called when traversal montage has been interrupted (or failed to play) */
	UFUNCTION()
	void OnTraversalMontageInterrupted(FName NotifyName);

	/** Called when a PlayMontageNotify or PlayMontageNotifyWindow Notify begins */
	UFUNCTION()
	void OnTraversalNotifyBegin(FName NotifyName);

	/** Called when a PlayMontageNotifyWindow Notify ends */
	UFUNCTION()
	void OnTraversalNotifyEnd(FName NotifyName);

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<UCharacterMoverComponent> Mover;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bDoingTraversalAction;

	/** Chooser table for selecting traversal montages. Assign in Blueprint. */
	UPROPERTY(EditDefaultsOnly, Category = "Traversal|Montage")
	TObjectPtr<UChooserTable> MontageChooserTable;

private:

	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly)
	FTraversalCheckResult TraversalResult;

	UPROPERTY(EditDefaultsOnly)
	FCharacterPropertiesForTraversal CharacterProperties;

	// Active montage proxy for cleanup
	UPROPERTY()
	TObjectPtr<UPlayMoverMontageCallbackProxy> ActiveMontageProxy;

};