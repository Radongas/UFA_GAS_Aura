// Copyright Elish Li


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

AAuraPlayerState::AAuraPlayerState()
{
	//make a constructor of the AbilitySystemComponent
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	/**EGameplayEffectReplicationMode
	 * Full： used in single player, Gameplay Effect are replicated to all clients
	 * Mixed: Multiplayer, player controlled, Gameplay Effects are replicated to the owning clinet only. Gameplay Cues and Gameplay Tags replicated to all clients
	 * Minimal multiplayer, AI-Controlled, Gameplay Effects are NOT replicated, Gameplay Cues and Gameplay Tags replicated to all clients
	**/
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	//make a constructor of the AuraAttributeSet
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	//The frequency that the server will update data to the clients. 100 times per second
	NetUpdateFrequency = 100.f;
	
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
