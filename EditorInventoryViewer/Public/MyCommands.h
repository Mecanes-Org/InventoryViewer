// Copyright 2024,  Mecanes . All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

//ADD 
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

/**
 * 
 */
class FMyCommands : public TCommands<FMyCommands>
{
public:
	FMyCommands()
		: TCommands<FMyCommands>(
			TEXT("MyCommands"),
			NSLOCTEXT("Contexts", "MyCommands", "My Module Commands"),
			NAME_None,
			FAppStyle::GetAppStyleSetName())
	{}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> PluginAction;
};
