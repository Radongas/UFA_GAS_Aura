// Copyright Elish Li


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
	bool bLogNotFound) const
{
	for (const FAuraInputAction& IA : AbilityInputActions)
	{
		if (IA.InputAction && IA.InputTag == InputTag)
		{
			return IA.InputAction;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find tags associated with [%s] gameplay tag, in InputConfig [%s]"), *InputTag.ToString(), *GetNameSafe(this));;
	}
	return nullptr;
}
