// Copyright Elish Li

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AuraCharacterBase.generated.h"

UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();

protected:
	virtual void BeginPlay() override;
	// TObjectPtr is a raw pointer that support access tracking and optional lazy load behavior
	//access tracking means tracking how often a pointer is accessed or deferenced
	// lazy load behavior means an asset can not be loaded until it's actually needed or used in the program
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;


};
