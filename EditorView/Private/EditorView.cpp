// Copyright 2025,  Mecanes . All Rights Reserved.

#include "EditorView.h"

#include "EditorViewWindow.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FEditorViewModule"

void FEditorViewModule::StartupModule()
{
}

void FEditorViewModule::ShutdownModule()
{
	
}


#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FEditorViewModule, EditorView)