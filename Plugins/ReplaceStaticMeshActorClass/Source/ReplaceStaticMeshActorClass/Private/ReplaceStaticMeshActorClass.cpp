// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReplaceStaticMeshActorClass.h"

#include "AuraStaticMeshActor.h"
#include "ReplaceStaticMeshActorClassStyle.h"
#include "ReplaceStaticMeshActorClassCommands.h"
#include "Selection.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName ReplaceStaticMeshActorClassTabName("ReplaceStaticMeshActorClass");

#define LOCTEXT_NAMESPACE "FReplaceStaticMeshActorClassModule"

void FReplaceStaticMeshActorClassModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FReplaceStaticMeshActorClassStyle::Initialize();
	FReplaceStaticMeshActorClassStyle::ReloadTextures();

	FReplaceStaticMeshActorClassCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FReplaceStaticMeshActorClassCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FReplaceStaticMeshActorClassModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FReplaceStaticMeshActorClassModule::RegisterMenus));
}

void FReplaceStaticMeshActorClassModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FReplaceStaticMeshActorClassStyle::Shutdown();

	FReplaceStaticMeshActorClassCommands::Unregister();
}

void FReplaceStaticMeshActorClassModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	// FText DialogText = FText::Format(
	// 						LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
	// 						FText::FromString(TEXT("FReplaceStaticMeshActorClassModule::PluginButtonClicked()")),
	// 						FText::FromString(TEXT("ReplaceStaticMeshActorClass.cpp"))
	// 				   );
	// FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	TSet<FString> ComponentsToConsider;
	ComponentsToConsider.Add("StaticMeshComponent");

	TArray<AActor*> SelectedActors;
	GEditor->GetSelectedActors()->GetSelectedObjects(SelectedActors);

	UClass* ReplaceWithClass = AAuraStaticMeshActor::StaticClass();

	GEditor->ConvertActors(SelectedActors, ReplaceWithClass, ComponentsToConsider, true);
	
}

void FReplaceStaticMeshActorClassModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FReplaceStaticMeshActorClassCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FReplaceStaticMeshActorClassCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FReplaceStaticMeshActorClassModule, ReplaceStaticMeshActorClass)