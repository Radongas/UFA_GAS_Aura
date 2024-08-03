// Copyright Elish Li

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MaterialAssetInfo.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMaterialAssetInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Fade Materials")
	TMap<UMaterialInterface*, UMaterialInterface*> MaterialToFadeMaterial;
	
	UFUNCTION(BlueprintCallable)
	UMaterialInterface* GetItsFadeMaterial(UMaterialInterface* OriginalMaterial);

	UFUNCTION(BlueprintCallable)
	UMaterialInterface* GetItsOriginalMaterial(UMaterialInterface* FadeMaterial);
};
