// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"

#include "GASPAnimationTypes.generated.h"

/**
 * Experimental state machine states for animation system.
 */
UENUM(BlueprintType)
enum class EExperimentalStateMachineState : uint8
{
	IdleLoop,
	TransitionToIdleLoop,
	LocomotionLoop,
	TransitionToLocomotionLoop,
	InAirLoop,
	TransitionToInAirLoop,
	IdleBreak,
	TransitionToSlide,
	SlideLoop
};

/**
 * Traversal action types for obstacle navigation.
 */
UENUM(BlueprintType)
enum class ETraversalActionType : uint8
{
	None,
	/** Traverse over a thin object and end on the ground at a similar level (Low fence). */
	Hurdle,
	/** Traverse over a thin object and end in a falling state (Tall fence, or elevated obstacle with no floor on the other side). */
	Vault,
	/** Traverse up and onto an object without passing over it. */
	Mantle
};

/**
 * Struct used in Experimental State Machine to drive Blend Stack inputs.
 */
USTRUCT(BlueprintType)
struct FBlendStackInputs
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimationAsset* Anim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Loop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlendTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBlendProfile* BlendProfile = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Tags;
};

/**
 * Outputs for animation chooser.
 */
USTRUCT(BlueprintType)
struct FChooserOutputs
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UseMM = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MMCostLimit = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlendTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BlendProfile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Tags;
};

/**
 * Debug graph line properties.
 */
USTRUCT(BlueprintType)
struct FDebugGraphLineProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Values;
};

/**
 * Pose history entry for motion matching.
 */
USTRUCT(BlueprintType)
struct FPoseHistoryEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Timestamp = 0.0f;
};

/**
 * Pose history for motion matching and traversal.
 */
USTRUCT(BlueprintType)
struct FPoseHistory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPoseHistoryEntry> Entries;
};