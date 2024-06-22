// Copyright Elish Li

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();

	/* Enemy Interface*/
	virtual void HighlightActor() override;
	virtual void UnhighlightActor() override;
	/* End Enemy Interface */
	virtual void BeginPlay() override;
};
