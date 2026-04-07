// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/GASPDataTypes.h"

#include "GASPCharacterABPInterface.generated.h"

/**
 * Interface for character animation blueprint interaction.
 */
UINTERFACE(Blueprintable)
class UGASPCharacterABPInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for character animation blueprint interaction.
 */
class GAMEANIMATIONSYSTEM_API IGASPCharacterABPInterface
{
	GENERATED_BODY()

public:
	/** Category= Getters */

	/**
	 * Get the pose history for motion matching.
	 * @return The pose history containing trajectory entries.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Getters")
	FPoseHistory Get_PoseHistory() const;
	virtual FPoseHistory Get_PoseHistory_Implementation() const { return FPoseHistory(); }

	/**
	 * Get the interaction transform for traversal actions.
	 * @return The transform for interaction positioning.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Getters")
	FTransform Get_InteractionTransform() const;
	virtual FTransform Get_InteractionTransform_Implementation() const { return FTransform(); }

	/**
	 * Get the current gait state.
	 * @return The current gait (Walk, Run, Sprint).
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Getters")
	EGait Get_Gait() const;
	virtual EGait Get_Gait_Implementation() const { return EGait::Walk; }

	/** Category= Setters */

	/**
	 * Set the interaction transform for traversal actions.
	 * @param InteractionTransform The transform to set for interaction positioning.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Setters")
	void Set_InteractionTransform(FTransform InteractionTransform);
	virtual void Set_InteractionTransform_Implementation(FTransform InteractionTransform) { }

	/**
	 * Set the re-transition notification flag.
	 * @param ReTransition Whether to trigger re-transition behavior.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Setters")
	void Set_NotifyTransition_ReTransition(bool ReTransition);
	virtual void Set_NotifyTransition_ReTransition_Implementation(bool ReTransition) { }

	/**
	 * Set the to-loop transition notification flag.
	 * @param ToLoop Whether to transition to loop state.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Setters")
	void Set_NotifyTransition_ToLoop(bool ToLoop);
	virtual void Set_NotifyTransition_ToLoop_Implementation(bool ToLoop) { }
};