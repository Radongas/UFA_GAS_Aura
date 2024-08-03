// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReplaceStaticMeshActorClassStyle.h"
#include "ReplaceStaticMeshActorClass.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FReplaceStaticMeshActorClassStyle::StyleInstance = nullptr;

void FReplaceStaticMeshActorClassStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FReplaceStaticMeshActorClassStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FReplaceStaticMeshActorClassStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ReplaceStaticMeshActorClassStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FReplaceStaticMeshActorClassStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ReplaceStaticMeshActorClassStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ReplaceStaticMeshActorClass")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ReplaceStaticMeshActorClass.PluginAction", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	return Style;
}

void FReplaceStaticMeshActorClassStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FReplaceStaticMeshActorClassStyle::Get()
{
	return *StyleInstance;
}
