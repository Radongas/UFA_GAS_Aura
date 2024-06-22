// Copyright Elish Li

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UAuraAttributeSet();
	// a function that has to be implemented to allow a variable to be replicated
	virtual  void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//indicating that the variable is replicatable, when the replication happens from server to client, the client will have a replication notifier which is a function with convention OnRep_AttributeName
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes");
	FGameplayAttributeData Health;
	//this is accessors bring in a macro that defines and generates a set of helper function for accessing and initializing attributes. These helper function includes Get, Set, Initialize
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Health);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes");
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes");
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,Mana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes");
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet,MaxMana);
	

	//indicate that the following function is a Rep function, if the function take 1 argument, it must be the in the type of the replicated variable
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData OldHealth) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData OldMaxHealth) const;
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData OldMana) const;
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData OldMaxMana) const;
	
};
