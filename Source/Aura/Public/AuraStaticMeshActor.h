// Copyright Elish Li

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Engine/StaticMeshActor.h"
#include "AuraStaticMeshActor.generated.h"
/**
 * 
 */
UCLASS()
class AURA_API AAuraStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()

	AAuraStaticMeshActor();

public:
	UFUNCTION(BlueprintCallable, Category="Materials")
	void InitializeFadeDynamicMaterials();

	UPROPERTY(BlueprintReadWrite)
	TArray<UMaterialInterface*> OriginalMaterials;
	UPROPERTY(BlueprintReadWrite)
	TArray<UMaterialInterface*> FadeMaterials;
	UPROPERTY(BlueprintReadWrite)
	TArray<UMaterialInstanceDynamic*> DynamicsInstances;
	
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite)
	bool bHaveFadeBefore = false;
};
