// Copyright Elish Li

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "AuraCharacterBase.generated.h"

class UAuraAbilitySystemComponent;
class UAttributeSet;

UCLASS(Abstract)
//IAbilitySystemInterface is an interface that allows quick access to an object's Ability System
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}
protected:
	virtual void BeginPlay() override;
	// TObjectPtr is a raw pointer that support access tracking and optional lazy load behavior
	//access tracking means tracking how often a pointer is accessed or deferenced
	// lazy load behavior means an asset can not be loaded until it's actually needed or used in the program
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};
