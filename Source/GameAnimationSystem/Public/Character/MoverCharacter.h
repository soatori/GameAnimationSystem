// Copyright Soatori Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "MoverCharacter.generated.h"


class UNavMoverComponent;
class UCapsuleComponent;
class UCharacterMoverComponent;

UCLASS(Config = Game, BlueprintType)
class GAMEANIMATIONSYSTEM_API AMoverCharacter : public APawn
{
	GENERATED_BODY()

public:

	AMoverCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:

	/** The main skeletal mesh associated with this Character (optional sub-object). */
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterMoverComponent> CharacterMover;

	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UNavMoverComponent> NavMover;

	/** The CapsuleComponent being used for movement collision (by CharacterMovement). Always treated as being vertically aligned in simple collision check functions. */
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

public:

	/** Returns Mesh subobject */
	USkeletalMeshComponent* GetMesh() const { return Mesh; }

	/** Returns CharacterMover subobject */
	UCharacterMoverComponent* GetCharacterMover() const { return CharacterMover; }

	/** Returns NavMover subobject */
	UNavMoverComponent* GetNavMover() const { return NavMover; }

	/** Returns CapsuleComponent subobject */
	UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }

};