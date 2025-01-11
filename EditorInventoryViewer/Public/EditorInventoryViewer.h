// Copyright 2025,  Mecanes . All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorViewWindow.h"
#include "Modules/ModuleManager.h"

class FEditorInventoryViewerModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    //FUNCTION

    //INTERFACE
    void RegisterMenus();
    void PluginButtonClicked();
    TSharedRef<SWidget> GenerateDropdownMenu();
    void AddMenus(FMenuBuilder& MenuBuilder);

    void OnDashboard();
    void OnView();
    void OnDoc();
    void OnDocFR();
    void OnDiscord();
    void OnWebsite();


    //CHECKED BOX
    void OnToggleFeature();
    bool IsFeatureEnabled() const;
    
    //VARIABLE
    bool bAutomaticUpdate = false;
    
    TSharedPtr<SEditorViewWindow> Widget;
    TSharedPtr<FUICommandList> PluginCommands;
    
    FString Link_EN_Doc = TEXT("https://docs.google.com/document/d/1w9rfn3r3VupmIiWL5DZvSWVL0J2-YafrEuMUIPjQqdI/edit?tab=t.0");
    FString Link_FR_Doc = TEXT("https://docs.google.com/document/d/1m9_HEgKOQaagL8SrWstJR0DzjOps7KxcTRoHi3myskw/edit?tab=t.0");
    FString Link_Discord = TEXT("https://discord.gg/xb78rVQ7F2");
    FString Link_Website = TEXT("https://mecanes.github.io/Mecanes.fr");
};
