// Copyright 2025,  Mecanes . All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

//WORLD
#include "GameFramework/Actor.h"
/**
 * 
 */

//WIDGET
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"

//BAG
#include "ProInventorySystem/Public/AC_ItemsBag.h"

class EDITORVIEW_API SEditorViewWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEditorViewWindow) {}
	SLATE_END_ARGS()

	/** Constructeur de la fenêtre */
	void Construct(const FArguments& InArgs);

	void GameStart();

	
	void Set_WindowHandle(TSharedRef<SWindow> InWindowHandle);
	UWorld* GetWorld();
	bool IsValidWorld(UWorld *World);
	void ActorFromComponent();

	FSlateBrush* CreateBrushFromTexture(UTexture2D* Texture);
	
	/** Fermer la fenêtre */
	void CloseWindow();

private:
	TSharedPtr<SWindow> WindowHandle; // Référence à la fenêtre
	TSharedPtr<SVerticalBox> ScrollBoxContent;
	TSharedRef<SWidget> CreateButtonBag(UAC_ItemsBag* bag);
	AActor *current_actor;
	UWorld* CurrentWorld;

	UAC_ItemsBag* AC_ItemsBag;
	TArray<UAC_ItemsBag*> ArrayAc_ItemsBag;
};
