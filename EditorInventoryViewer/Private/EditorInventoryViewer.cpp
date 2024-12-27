// Copyright 2024,  Mecanes . All Rights Reserved.

#include "EditorInventoryViewer.h"

//ADD

#include "ToolMenus.h"
#include "MyCommands.h" // Classe des commandes personnalisées
#include "ToolMenuSection.h"
#include "ToolMenuEntry.h"\
/*
#include "EditorStyleSet.h"
#include "LevelEditor.h"
*/


#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"


#define LOCTEXT_NAMESPACE "FEditorInventoryViewerModule"

void FEditorInventoryViewerModule::StartupModule()
{

	// Vérifier si le plugin principal est actif
	if (!FModuleManager::Get().IsModuleLoaded("ProInventorySystem"))
	{
		return;
	}
	
	// Enregistrez les commandes
	FMyCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMyCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FEditorInventoryViewerModule::PluginButtonClicked),
		FCanExecuteAction()
	);

	// Enregistrez les menus
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FEditorInventoryViewerModule::RegisterMenus));
}

void FEditorInventoryViewerModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FMyCommands::Unregister();
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
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "PropertyWindow.Button_PickAsset") // Icône du sous-menu
		);
	}

	// Ajouter un bouton à la barre d'outils
	{

		// Ajouter un bouton avec une icône personnalisée à la barre d'outils
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("WindowLayout");
		
		
		// Ajouter une entrée avec une icône personnalisée
		FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitComboButton(
			"InventoryViewer", // ID de l'entrée
			FUIAction(),    // Pas d'action directe, car on utilise un menu contextuel
			FOnGetContent::CreateRaw(this, &FEditorInventoryViewerModule::GenerateDropdownMenu), // Fonction pour générer le menu
			LOCTEXT("InventoryViewer_Label", "Inventory Viewer"),     // Nom affiché sur le bouton
			LOCTEXT("InventoryViewer_ToolTip", "Inventory viewer"), // Info-bulle
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "PropertyWindow.Button_PickAsset") // Icône personnalisée
		));
	}
}



void FEditorInventoryViewerModule::PluginButtonClicked()
{
	FText Message = LOCTEXT("Click", "Active");
	FMessageDialog::Open(EAppMsgType::Ok, Message);
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

	// Catégorie 4
	MenuBuilder.BeginSection("Infos", LOCTEXT("InfosHeader", "Infos"));
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
					.Text(LOCTEXT("InventoryViewerVersionLabel", "v0.0.1"))
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
	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("Dashboard", "We are working on it "));
}

void FEditorInventoryViewerModule::OnView()
{
	//FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("View", "It will be available soon !"));

	// Créez une nouvelle fenêtre
	TSharedRef<SWindow> CustomWindow = SNew(SWindow)
		.Title(FText::FromString(TEXT("Fenêtre Personnalisée")))  // Titre de la fenêtre
		.ClientSize(FVector2D(1000, 700))  // Taille de la fenêtre
		.IsPopupWindow(false)  // Cette fenêtre est une fenêtre contextuelle
		.SupportsMinimize(false)
		.AutoCenter(EAutoCenter::None)
		.SupportsMaximize(true);

	// Ajouter des widgets personnalisés à la fenêtre
	CustomWindow->SetContent(
		SNew(SVerticalBox)  // Conteneur vertical
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)  // Un simple texte
			.Text(FText::FromString(TEXT("Bienvenue dans ma fenêtre personnalisée !")))
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SButton)  // Un bouton qui ferme la fenêtre
			.Text(FText::FromString(TEXT("Fermer")))
			.OnClicked_Lambda([CustomWindow]() -> FReply {
				CustomWindow->RequestDestroyWindow();  // Ferme la fenêtre lorsque le bouton est cliqué
				return FReply::Handled();
			})
		]
	);

	// Afficher la fenêtre dans l'application
	FSlateApplication::Get().AddWindow(CustomWindow);
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
























#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FEditorInventoryViewerModule, EditorInventoryViewer)