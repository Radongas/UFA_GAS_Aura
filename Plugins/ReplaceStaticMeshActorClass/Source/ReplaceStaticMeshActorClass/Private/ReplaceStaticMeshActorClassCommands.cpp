// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReplaceStaticMeshActorClassCommands.h"

#define LOCTEXT_NAMESPACE "FReplaceStaticMeshActorClassModule"

void FReplaceStaticMeshActorClassCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "ReplaceStaticMeshActorClass", "Replace its Static Mesh Actor", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
