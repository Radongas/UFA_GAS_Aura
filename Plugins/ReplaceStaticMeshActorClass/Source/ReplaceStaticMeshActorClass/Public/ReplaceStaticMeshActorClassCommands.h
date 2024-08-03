// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ReplaceStaticMeshActorClassStyle.h"

class FReplaceStaticMeshActorClassCommands : public TCommands<FReplaceStaticMeshActorClassCommands>
{
public:

	FReplaceStaticMeshActorClassCommands()
		: TCommands<FReplaceStaticMeshActorClassCommands>(TEXT("ReplaceStaticMeshActorClass"), NSLOCTEXT("Contexts", "ReplaceStaticMeshActorClass", "ReplaceStaticMeshActorClass Plugin"), NAME_None, FReplaceStaticMeshActorClassStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
