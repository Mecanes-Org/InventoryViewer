// Copyright 2024,  Mecanes . All Rights Reserved.


#include "MyCommands.h"

#define LOCTEXT_NAMESPACE "MyCommands"

void FMyCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Inventory System", "Execute My Plugin Action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE