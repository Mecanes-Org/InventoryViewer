// Copyright 2025,  Mecanes . All Rights Reserved.

#include "EditorInventoryViewer.h"

//ADD

#include "ToolMenus.h"
#include "MyCommands.h" // Classe des commandes personnalisées
#include "ToolMenuSection.h"
#include "ToolMenuEntry.h"

#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

//MODULE
#include "BPFL_ProInventorySystem.h"
#include "AC_ItemsBag.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "FEditorInventoryViewerModule"
// Vous devez enregistrer cet événement


void FEditorInventoryViewerModule::StartupModule()
{
	
	// Vérifier si le plugin principal est actif
	if (!FModuleManager::Get().IsModuleLoaded("ProInventorySystem"))
	{
		return;
	}

	/*
	// Enregistrez les commandes
	FMyCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMyCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FEditorInventoryViewerModule::PluginButtonClicked),
		FCanExecuteAction()
	);

	*/
	
	// Enregistrez les menus
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FEditorInventoryViewerModule::RegisterMenus));
}

void FEditorInventoryViewerModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FMyCommands::Unregister();

	FEditorDelegates::PostPIEStarted.RemoveAll(this);
}


void FEditorInventoryViewerModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	// Ajouter au menu principal
	{
		/*
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
		Section.AddMenuEntryWithCommandList(FMyCommands::Get().PluginAction, PluginCommands);

		*/
		
		// Ajouter vos items au menu principal
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");

		Section.AddSeparator(FName("Separator"));
		
		Section.AddSubMenu(
			"Inventory Viewer",                                      // ID de votre menu
			LOCTEXT("InventoryViewer_Menu", "Inventory Viewer"),      // Texte affiché
			LOCTEXT("InventoryViewer_ToolTip", "Inventory viewer"),    // Tooltip
			FNewMenuDelegate::CreateRaw(this, &FEditorInventoryViewerModule::AddMenus), // Contenu
			false,                                                  // Ne pas épingler
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Viewports") // Icône du sous-menu
		);
	}

	// Ajouter un bouton à la barre d'outils
	{

		// Ajouter un bouton avec une icône personnalisée à la barre d'outils
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		//FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("WindowLayout");
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
		
		
		// Ajouter une entrée avec une icône personnalisée
		FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitComboButton(
			"InventoryViewer", // ID de l'entrée
			FUIAction(),    // Pas d'action directe, car on utilise un menu contextuel
			FOnGetContent::CreateRaw(this, &FEditorInventoryViewerModule::GenerateDropdownMenu), // Fonction pour générer le menu
			LOCTEXT("InventoryViewer_Label", "Inventory Viewer"),     // Nom affiché sur le bouton
			LOCTEXT("InventoryViewer_ToolTip", "Inventory viewer"), // Info-bulle
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Viewports") // Icône personnalisée
		));
	}
}



void FEditorInventoryViewerModule::PluginButtonClicked()
{
	/*
	FText Message = LOCTEXT("Click", "Active");
	FMessageDialog::Open(EAppMsgType::Ok, Message);
	*/
}


TSharedRef<SWidget> FEditorInventoryViewerModule::GenerateDropdownMenu()
{
	// Crée un nouveau builder de menu
	FMenuBuilder MenuBuilder(true, nullptr);

	// Ajoutez des sections et des options à ce menu
	AddMenus(MenuBuilder);

	return MenuBuilder.MakeWidget();
}


void FEditorInventoryViewerModule::AddMenus(FMenuBuilder& MenuBuilder)
{
	// Catégorie 1 - InventorySystem
	MenuBuilder.BeginSection("InventorySystem", LOCTEXT("Inventory System Header", "Inventory System"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("InventoryViewer_Dashboard", "Dashboard"),
			LOCTEXT("InventoryViewer_DashboardTooltip", "Go to dashboard"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "DeveloperTools.MenuIcon"),
			FUIAction(FExecuteAction::CreateRaw(this, &FEditorInventoryViewerModule::OnDashboard))
		);
		
		MenuBuilder.AddMenuEntry(
			LOCTEXT("InventoryViewer_View", "View"),
			LOCTEXT("InventoryViewer_ViewTooltip", "view inventory"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Visibility"),
			FUIAction(FExecuteAction::CreateRaw(this, &FEditorInventoryViewerModule::OnView))
		);
	}
	MenuBuilder.EndSection();


	// Catégorie 2 - Documentation
	MenuBuilder.BeginSection("Documentation", LOCTEXT("DocumentationHeader", "Documentation"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("InventoryViewer_DocLabel", "Doc "),
			LOCTEXT("InventoryViewer_DocTooltip", "Go to Documentation"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.GoToDocumentation"),
			FUIAction(FExecuteAction::CreateRaw(this, &FEditorInventoryViewerModule::OnDoc))
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("InventoryViewer_DocLabel", "Doc [ FR ]"),
			LOCTEXT("InventoryViewer_DocTooltip", "Go to Documentation"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.GoToDocumentation"),
			FUIAction(FExecuteAction::CreateRaw(this, &FEditorInventoryViewerModule::OnDocFR))
		);
		
	}
	MenuBuilder.EndSection();

	// Catégorie 3 - Help
	MenuBuilder.BeginSection("Help", LOCTEXT("HelpHeader", "Help"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("InventoryViewer_DiscordLabel", "Discord"),
			LOCTEXT("InventoryViewer_DiscordTooltip", "Go to discord"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Help"),
			FUIAction(FExecuteAction::CreateRaw(this, &FEditorInventoryViewerModule::OnDiscord))
		);

		MenuBuilder.AddMenuEntry(
			LOCTEXT("InventoryViewer_website", "Website"),
			LOCTEXT("InventoryViewer_websiteTooltip", "Website"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "MessageLog.Url"),
			FUIAction(FExecuteAction::CreateRaw(this, &FEditorInventoryViewerModule::OnWebsite))
		);
		
	}
	MenuBuilder.EndSection();


	// Catégorie 3 - Checked box
	MenuBuilder.BeginSection("Settings", LOCTEXT("SettingsHeader", "Settings"));
	{
		MenuBuilder.AddSubMenu(
		LOCTEXT("InventoryViewer_SettingsLabel", "Settings"),
		LOCTEXT("InventoryViewer_SettingsTooltip", "Settings"),
		FNewMenuDelegate::CreateLambda([this](FMenuBuilder& SubMenuBuilder)
			{
				// Checkbox entry
				SubMenuBuilder.AddMenuEntry(
					LOCTEXT("InventoryViewer_AutomaticUpdateLabel", "Automatic Update"),
					LOCTEXT("InventoryViewer_AutomaticUpdateTooltip", "it is in the test phase and not yet optimized"),
					FSlateIcon(), // Optional icon
					FUIAction(
						FExecuteAction::CreateRaw(this, &FEditorInventoryViewerModule::OnToggleFeature),
						FCanExecuteAction(),
						FIsActionChecked::CreateRaw(this, &FEditorInventoryViewerModule::IsFeatureEnabled)
					),
					NAME_None,
					EUserInterfaceActionType::ToggleButton
				);
			})
		);

		
	}
	MenuBuilder.EndSection();

	// Version
	MenuBuilder.BeginSection("Versions", LOCTEXT("VersionsHeader", "Versions"));
	{
		MenuBuilder.AddWidget(
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Fill) // Remplir horizontalement pour permettre un alignement correct
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f) // Le premier texte occupe tout l'espace restant
				.HAlign(HAlign_Left)
				[
					SNew(STextBlock)
					.Margin(FMargin(10, 0, 0, 5)) // Espacement à droite
					.Text(LOCTEXT("DashboardLabel", "Dashboard"))
					.ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.5f))) // Texte avec opacité
				]
				+ SHorizontalBox::Slot()
				.AutoWidth() // Le second texte prend uniquement l'espace nécessaire
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Margin(FMargin(0, 0, 10, 5)) // Espacement inférieur
					.Text(LOCTEXT("DashboardVersionLabel", "v0.0.1"))
					.ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.5f))) // Texte avec opacité
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Fill) // Remplir horizontalement pour permettre un alignement correct
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f) // Le premier texte occupe tout l'espace restant
				.HAlign(HAlign_Left)
				[
					SNew(STextBlock)
					.Margin(FMargin(10, 0, 0, 5)) // Espacement à droite
					.Text(LOCTEXT("ViewLabel", "View"))
					.ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.5f))) // Texte avec opacité
				]
				+ SHorizontalBox::Slot()
				.AutoWidth() // Le second texte prend uniquement l'espace nécessaire
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Margin(FMargin(0, 0, 10, 5)) // Espacement inférieur
					.Text(LOCTEXT("ViewVersionLabel", "v0.1.2"))
					.ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.5f))) // Texte avec opacité
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Fill) // Remplir horizontalement pour permettre un alignement correct
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f) // Le premier texte occupe tout l'espace restant
				.HAlign(HAlign_Left)
				[
					SNew(STextBlock)
					.Margin(FMargin(10, 0, 0, 0)) // Espacement à droite
					.Text(LOCTEXT("InventoryViewerVersionLabel", "Inventory Viewer"))
					.ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.5f))) // Texte avec opacité
				]
				+ SHorizontalBox::Slot()
				.AutoWidth() // Le second texte prend uniquement l'espace nécessaire
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Margin(FMargin(0, 0, 10, 5)) // Espacement inférieur
					.Text(LOCTEXT("InventoryViewerVersionLabel", "v0.1.1"))
					.ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.5f))) // Texte avec opacité
				]
			],
			
			FText() // Pas de label
		);
		
		
	}
	
	MenuBuilder.EndSection();
}


void FEditorInventoryViewerModule::OnDashboard()
{
	int32 TotalBags = 0;
	int32 TotalItems = 0;
	int32 TotalQuantity = 0;
	int32 Added = 0;
	int32 Removed = 0;
	int32 Updated = 0;

	if (GEditor && GEditor->PlayWorld)
	{
		for (TActorIterator<AActor> ActorItr(GEditor->PlayWorld); ActorItr; ++ActorItr)
		{
			AActor* Actor = *ActorItr;
			if (!Actor)
			{
				continue;
			}

			TArray<UAC_ItemsBag*> Bags = UBPFL_ProInventorySystem::GetAllBags(Actor);

			for (UAC_ItemsBag* Bag : Bags)
			{
				if (!Bag)
				{
					continue;
				}

				TotalBags++;
				TotalItems += Bag->ItemsBag.Num();

				Added += Bag->DashboardAddedCount;
				Removed += Bag->DashboardRemovedCount;
				Updated += Bag->DashboardUpdatedCount;

				for (const FS_Item& Item : Bag->ItemsBag)
				{
					TotalQuantity += Item.Quantity;
				}
			}
		}
	}

	TSharedRef<SWindow> DashboardWindow = SNew(SWindow)
		.Title(FText::FromString("Inventory Dashboard"))
		.ClientSize(FVector2D(600, 360))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		[
			SNew(SBorder)
			.Padding(20)
			.BorderBackgroundColor(FLinearColor(0.02f, 0.02f, 0.025f, 1.0f))
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 20)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Inventory Dashboard"))
					.Font(FCoreStyle::Get().GetFontStyle("BoldFont"))
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 10)
				[
					SNew(STextBlock)
					.Text(FText::FromString(FString::Printf(TEXT("Bags detected: %d"), TotalBags)))
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 10)
				[
					SNew(STextBlock)
					.Text(FText::FromString(FString::Printf(TEXT("Unique items: %d"), TotalItems)))
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 20)
				[
					SNew(STextBlock)
					.Text(FText::FromString(FString::Printf(TEXT("Total quantity: %d"), TotalQuantity)))
					.ColorAndOpacity(FLinearColor::White)
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.Padding(5)
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString::Printf(TEXT("Added: %d"), Added)))
						.ColorAndOpacity(FLinearColor(0.2f, 1.0f, 0.4f, 1.0f))
					]

					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.Padding(5)
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString::Printf(TEXT("Removed: %d"), Removed)))
						.ColorAndOpacity(FLinearColor(1.0f, 0.25f, 0.25f, 1.0f))
					]

					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.Padding(5)
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString::Printf(TEXT("Updated: %d"), Updated)))
						.ColorAndOpacity(FLinearColor(0.4f, 0.7f, 1.0f, 1.0f))
					]
				]
			]
		];

	FSlateApplication::Get().AddWindow(DashboardWindow);
}

void FEditorInventoryViewerModule::OnView()
{
	//FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("View", "It will be available soon !"));

	TSharedRef<SWindow> ViewWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("VIEW")))
		.ClientSize(FVector2D(1000, 600))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::FixedSize)
		[
			SNew(SEditorViewWindow)
		];

	// Mémoriser une référence à la fenêtre si nécessaire
	Widget = StaticCastSharedRef<SEditorViewWindow>(ViewWindow->GetContent());
	Widget->Set_WindowHandle(ViewWindow);
	
	FSlateApplication::Get().AddWindow(ViewWindow);
}


void FEditorInventoryViewerModule::OnDoc()
{
	// Ouvrir le lien dans le navigateur par défaut
	FPlatformProcess::LaunchURL(*Link_EN_Doc, nullptr, nullptr);
}

void FEditorInventoryViewerModule::OnDocFR()
{
	// Ouvrir le lien dans le navigateur par défaut
	FPlatformProcess::LaunchURL(*Link_FR_Doc, nullptr, nullptr);
}

void FEditorInventoryViewerModule::OnDiscord()
{
	// Ouvrir le lien dans le navigateur par défaut
	FPlatformProcess::LaunchURL(*Link_Discord, nullptr, nullptr);
}

void FEditorInventoryViewerModule::OnWebsite()
{
	//FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("Website", "Go to Website !"));

	// Ouvrir le lien dans le navigateur par défaut
	FPlatformProcess::LaunchURL(*Link_Website, nullptr, nullptr);
}

void FEditorInventoryViewerModule::OnToggleFeature()
{
	bAutomaticUpdate = !bAutomaticUpdate;

	if (Widget.IsValid())
	{
		Widget->UpdateWindow(bAutomaticUpdate);
	}

}

bool FEditorInventoryViewerModule::IsFeatureEnabled() const
{
	return bAutomaticUpdate;
}


#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FEditorInventoryViewerModule, EditorInventoryViewer)