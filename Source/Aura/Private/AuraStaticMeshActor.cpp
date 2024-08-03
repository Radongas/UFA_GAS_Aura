// Copyright Elish Li


#include "AuraStaticMeshActor.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/MaterialAssetInfo.h"
#include "Aura/Aura.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"

AAuraStaticMeshActor::AAuraStaticMeshActor()
{
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Block);
	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
}

void AAuraStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();
	if (GetStaticMeshComponent())
	{
		//given a mesh's original material, fetch its corresponding fade textures from material asset table, and store them in an array
		OriginalMaterials = GetStaticMeshComponent()->GetMaterials();
		if (OriginalMaterials.Num()>0)
		{
			UMaterialAssetInfo* MaterialAssetInfo = UAuraAbilitySystemLibrary::GetMaterialAssetInfo(this);
			for (const UMaterialInterface* Item : OriginalMaterials)
			{
				if(MaterialAssetInfo->MaterialToFadeMaterial.Contains(Item))
				{
					FadeMaterials.Add(*MaterialAssetInfo->MaterialToFadeMaterial.Find(Item));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT(" %s cannot find fade material of %s"), *GetActorNameOrLabel(), *Item->GetName());
				}
			}
		}
	}
	InitializeFadeDynamicMaterials();
}

void AAuraStaticMeshActor::InitializeFadeDynamicMaterials()
{
	for (int i=0; i < FadeMaterials.Num(); i++)
	{
		UMaterialInstanceDynamic* NewMaterialInst = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this,FadeMaterials[i]);
		DynamicsInstances.Add(NewMaterialInst);
	}
}


