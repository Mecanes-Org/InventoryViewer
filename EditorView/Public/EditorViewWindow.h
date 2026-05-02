// Copyright 2025,  Mecanes . All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

//WORLD
#include "GameFramework/Actor.h"
#include "Engine/World.h"

//WIDGET
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

//BAG
#include "ProInventorySystem/Public/AC_ItemsBag.h"
#include "Widgets/Layout/SWrapBox.h"

//TIMER
#include "HAL/PlatformTime.h"
#include "HAL/PlatformApplicationMisc.h"

class EDITORVIEW_API SEditorViewWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEditorViewWindow) {}
	SLATE_END_ARGS()

	/** Constructeur de la fenêtre */
	void Construct(const FArguments& InArgs);
	~SEditorViewWindow();

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;


	//FUNCTION
	void GameStart();
	void AfterGameStart();

	
	void Set_WindowHandle(TSharedRef<SWindow> InWindowHandle);
	UWorld* GetWorld();
	bool IsValidWorld(UWorld *World);
	void ActorFromComponent();
	FSlateBrush* CreateBrushFromTexture(UTexture2D* Texture);

	//AFFICHE TOUTES LES ITEMS D UN SAC
	void ShowAllItems(UAC_ItemsBag* Bag);

	/** Fermer la fenêtre */
	void CloseWindow();
	void UpdateItemOfBag(UAC_ItemsBag* bag);
	void UpdateWindow(bool bAutomaticUpdate);

	//TIMER
	void StartTimer();

	void RefreshBagList();

private:

	void BindBagEvents();
	void UnbindBagEvents();

	void OnBagInventoryChanged(
		UAC_ItemsBag* Bag,
		EProInventoryChangeType ChangeType,
		FS_Item Item
	);

	/** Timer interval in seconds */
	float TimerInterval = 5.0f;
	/** Accumulated time since the last timer trigger */
	float TimeSinceLastTick = 0.0f;
	
	bool bAutomaticUpdate = false;
	
	TSharedRef<SWidget> CreateButtonBag(UAC_ItemsBag* bag);
	TSharedRef<SWidget> CreateButtonItem(FS_Item Item);
	
	AActor *current_actor;
	UWorld* CurrentWorld;
	TSharedPtr<SWindow> WindowHandle; // Référence à la fenêtre
	TSharedPtr<SVerticalBox> ScrollBoxContentLeft;
	TSharedPtr<SWrapBox> SWrapBoxRight;
	UAC_ItemsBag* AC_ItemsBag;
	TArray<UAC_ItemsBag*> ArrayAc_ItemsBag;
};

