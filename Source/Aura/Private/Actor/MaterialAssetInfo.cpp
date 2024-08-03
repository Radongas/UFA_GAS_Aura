// Copyright Elish Li


#include "Actor/MaterialAssetInfo.h"

UMaterialInterface* UMaterialAssetInfo::GetItsFadeMaterial(UMaterialInterface* OriginalMaterial)
{
	for (const TTuple<UMaterialInterface*, UMaterialInterface*> Item : MaterialToFadeMaterial)
	{
		if (Item.Key == OriginalMaterial)
		{
			return Item.Value;
		}
	}
	return nullptr;
}

UMaterialInterface* UMaterialAssetInfo::GetItsOriginalMaterial(UMaterialInterface* FadeMaterial)
{
	for (const TTuple<UMaterialInterface*, UMaterialInterface*> Item : MaterialToFadeMaterial)
	{
		if (Item.Value == FadeMaterial)
		{
			return Item.Key;
		}
	}
	return nullptr;
}
