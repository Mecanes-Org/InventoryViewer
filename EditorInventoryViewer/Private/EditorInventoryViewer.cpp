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

// DASHBOARD
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Containers/Set.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"

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

// CUSTOM CLASS POUR ON DASHBOARD
class SInventoryDashboardWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SInventoryDashboardWindow) {}
	SLATE_END_ARGS()

	~SInventoryDashboardWindow()
	{
		UnbindBagEvents();
	}

	void Construct(const FArguments& InArgs)
	{
		ChildSlot
		[
			SNew(SBorder)
			.Padding(16)
			.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor(FLinearColor(0.025f, 0.025f, 0.03f, 1.0f))
			[
				SNew(SScrollBox)

				+ SScrollBox::Slot()
				[
					SAssignNew(ContentBox, SVerticalBox)
				]
			]
		];

		BindBagEvents();
		RefreshDashboard();
	}

private:
	struct FBagDashboardData
	{
		UAC_ItemsBag* Bag = nullptr;
		FString BagName = TEXT("Unknown");
		FString OwnerName = TEXT("Unknown");

		int32 DistinctItems = 0;
		int32 TotalQuantity = 0;
		int32 Capacity = 0;

		float FillPercent = 0.0f;

		int32 AddedOps = 0;
		int32 RemovedOps = 0;
		int32 UpdatedOps = 0;
		int32 TotalActivity = 0;

		bool bIsCurrentBag = false;
	};

	struct FItemDashboardData
	{
		FString Name = TEXT("Unknown");
		int32 TotalQuantity = 0;
		int32 StackCount = 0;
	};

private:
	TSharedPtr<SVerticalBox> ContentBox;

private:
	TSharedRef<SWidget> MakeMetricCard(
		const FString& Title,
		const FString& Value,
		const FString& Subtitle,
		const FLinearColor& Accent
	)
	{
		return SNew(SBorder)
			.Padding(12)
			.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor(FLinearColor(0.07f, 0.07f, 0.09f, 1.0f))
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 6)
				[
					SNew(STextBlock)
					.Text(FText::FromString(Title))
					.ColorAndOpacity(Accent)
					.Font(FCoreStyle::Get().GetFontStyle("BoldFont"))
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 4)
				[
					SNew(STextBlock)
					.Text(FText::FromString(Value))
					.ColorAndOpacity(FLinearColor::White)
					.Font(FCoreStyle::Get().GetFontStyle("BoldFont"))
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::FromString(Subtitle))
					.ColorAndOpacity(FLinearColor(0.68f, 0.68f, 0.68f, 1.0f))
				]
			];
	}

	TSharedRef<SWidget> MakeInfoRow(
		const FString& Label,
		const FString& Value,
		const FLinearColor& ValueColor = FLinearColor::White
	)
	{
		return SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Label))
				.ColorAndOpacity(FLinearColor(0.75f, 0.75f, 0.75f, 1.0f))
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(Value))
				.ColorAndOpacity(ValueColor)
				.Font(FCoreStyle::Get().GetFontStyle("BoldFont"))
			];
	}

	TSharedRef<SWidget> MakePanel(const FString& Title, TSharedRef<SWidget> Content)
	{
		return SNew(SBorder)
			.Padding(14)
			.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor(FLinearColor(0.05f, 0.05f, 0.06f, 1.0f))
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 10)
				[
					SNew(STextBlock)
					.Text(FText::FromString(Title))
					.ColorAndOpacity(FLinearColor(0.90f, 0.90f, 1.0f, 1.0f))
					.Font(FCoreStyle::Get().GetFontStyle("BoldFont"))
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					Content
				]
			];
	}

	TArray<UAC_ItemsBag*> GetLiveBags()
	{
		TArray<UAC_ItemsBag*> Result;

		if (!GEditor || !GEditor->PlayWorld)
		{
			return Result;
		}

		for (TActorIterator<AActor> ActorItr(GEditor->PlayWorld); ActorItr; ++ActorItr)
		{
			AActor* Actor = *ActorItr;

			if (!Actor)
			{
				continue;
			}

			const TArray<UAC_ItemsBag*> FoundBags = UBPFL_ProInventorySystem::GetAllBags(Actor);

			for (UAC_ItemsBag* Bag : FoundBags)
			{
				if (Bag)
				{
					Result.AddUnique(Bag);
				}
			}
		}

		return Result;
	}

	void RefreshDashboard()
	{
		if (!ContentBox.IsValid())
		{
			return;
		}

		ContentBox->ClearChildren();

		TArray<FBagDashboardData> BagStats;
		TArray<FItemDashboardData> ItemStats;

		TMap<FString, int32> ItemQuantityMap;
		TMap<FString, int32> ItemStackMap;

		int32 TotalBags = 0;
		int32 EmptyBags = 0;
		int32 FullBags = 0;
		int32 CurrentBags = 0;

		int32 TotalDistinctItems = 0;
		int32 TotalQuantity = 0;
		int32 TotalCapacity = 0;

		int32 TotalAddedOps = 0;
		int32 TotalRemovedOps = 0;
		int32 TotalUpdatedOps = 0;

		const TArray<UAC_ItemsBag*> Bags = GetLiveBags();

		for (UAC_ItemsBag* Bag : Bags)
		{
			if (!Bag)
			{
				continue;
			}

			FBagDashboardData Data;

			Data.Bag = Bag;
			Data.BagName = Bag->getBagName().IsEmpty() ? TEXT("Unnamed Bag") : Bag->getBagName();
			Data.OwnerName = Bag->GetOwner() ? Bag->GetOwner()->GetName() : TEXT("Unknown Owner");

			Data.DistinctItems = Bag->ItemsBag.Num();
			Data.Capacity = FMath::Max(Bag->getBagCapacity(), 0);

			Data.AddedOps = Bag->DashboardAddedCount;
			Data.RemovedOps = Bag->DashboardRemovedCount;
			Data.UpdatedOps = Bag->DashboardUpdatedCount;
			Data.TotalActivity = Data.AddedOps + Data.RemovedOps + Data.UpdatedOps;

			Data.bIsCurrentBag = Bag->getIsCurrentBag();

			for (const FS_Item& Item : Bag->ItemsBag)
			{
				Data.TotalQuantity += Item.Quantity;

				const FString ItemName = Item.Name.IsEmpty() ? TEXT("Unnamed Item") : Item.Name;

				ItemQuantityMap.FindOrAdd(ItemName) += Item.Quantity;
				ItemStackMap.FindOrAdd(ItemName) += 1;
			}

			Data.FillPercent = Data.Capacity > 0
				? static_cast<float>(Data.DistinctItems) / static_cast<float>(Data.Capacity) * 100.0f
				: 0.0f;

			if (Data.DistinctItems == 0)
			{
				EmptyBags++;
			}

			if (Data.Capacity > 0 && Data.DistinctItems >= Data.Capacity)
			{
				FullBags++;
			}

			if (Data.bIsCurrentBag)
			{
				CurrentBags++;
			}

			TotalBags++;
			TotalDistinctItems += Data.DistinctItems;
			TotalQuantity += Data.TotalQuantity;
			TotalCapacity += Data.Capacity;

			TotalAddedOps += Data.AddedOps;
			TotalRemovedOps += Data.RemovedOps;
			TotalUpdatedOps += Data.UpdatedOps;

			BagStats.Add(Data);
		}

		for (const TPair<FString, int32>& Pair : ItemQuantityMap)
		{
			FItemDashboardData ItemData;
			ItemData.Name = Pair.Key;
			ItemData.TotalQuantity = Pair.Value;
			ItemData.StackCount = ItemStackMap.FindRef(Pair.Key);

			ItemStats.Add(ItemData);
		}

		BagStats.Sort([](const FBagDashboardData& A, const FBagDashboardData& B)
		{
			return A.TotalQuantity > B.TotalQuantity;
		});

		ItemStats.Sort([](const FItemDashboardData& A, const FItemDashboardData& B)
		{
			return A.TotalQuantity > B.TotalQuantity;
		});

		const float GlobalFillPercent = TotalCapacity > 0
			? static_cast<float>(TotalDistinctItems) / static_cast<float>(TotalCapacity) * 100.0f
			: 0.0f;

		const float AvgItemsPerBag = TotalBags > 0
			? static_cast<float>(TotalDistinctItems) / static_cast<float>(TotalBags)
			: 0.0f;

		const float AvgQuantityPerBag = TotalBags > 0
			? static_cast<float>(TotalQuantity) / static_cast<float>(TotalBags)
			: 0.0f;

		const FBagDashboardData* MostLoadedBag = BagStats.Num() > 0 ? &BagStats[0] : nullptr;

		const FBagDashboardData* MostActiveBag = nullptr;

		for (const FBagDashboardData& BagData : BagStats)
		{
			if (!MostActiveBag || BagData.TotalActivity > MostActiveBag->TotalActivity)
			{
				MostActiveBag = &BagData;
			}
		}

		ContentBox->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 14)
		[
			SNew(SBorder)
			.Padding(14)
			.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor(FLinearColor(0.045f, 0.045f, 0.055f, 1.0f))
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 8)
				[
					SNew(SHorizontalBox)

					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Dashboard"))
						.ColorAndOpacity(FLinearColor::White)
						.Font(FCoreStyle::Get().GetFontStyle("BoldFont"))
					]

					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Refresh / Rebind"))
						.ToolTipText(FText::FromString("Refresh dashboard and rebind delegates to all current bags."))
						.OnClicked_Lambda([this]()
						{
							BindBagEvents();
							RefreshDashboard();
							return FReply::Handled();
						})
					]
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Updates only when inventory delegates are fired. No Tick refresh."))
					.ColorAndOpacity(FLinearColor(0.75f, 0.75f, 0.80f, 1.0f))
				]
			]
		];

		TSharedRef<SUniformGridPanel> MetricsGrid = SNew(SUniformGridPanel)
			.SlotPadding(8.0f);

		MetricsGrid->AddSlot(0, 0)
		[
			MakeMetricCard(
				TEXT("Bags"),
				FString::Printf(TEXT("%d"), TotalBags),
				FString::Printf(TEXT("%d current / %d empty"), CurrentBags, EmptyBags),
				FLinearColor(0.45f, 0.85f, 1.0f, 1.0f)
			)
		];

		MetricsGrid->AddSlot(1, 0)
		[
			MakeMetricCard(
				TEXT("Distinct Items"),
				FString::Printf(TEXT("%d"), TotalDistinctItems),
				TEXT("Total occupied item slots"),
				FLinearColor(0.70f, 0.95f, 0.45f, 1.0f)
			)
		];

		MetricsGrid->AddSlot(2, 0)
		[
			MakeMetricCard(
				TEXT("Total Quantity"),
				FString::Printf(TEXT("%d"), TotalQuantity),
				TEXT("All quantities combined"),
				FLinearColor(1.0f, 0.75f, 0.35f, 1.0f)
			)
		];

		MetricsGrid->AddSlot(3, 0)
		[
			MakeMetricCard(
				TEXT("Capacity Usage"),
				FString::Printf(TEXT("%d / %d"), TotalDistinctItems, TotalCapacity),
				FString::Printf(TEXT("%.0f%% global fill"), GlobalFillPercent),
				FLinearColor(0.95f, 0.45f, 0.65f, 1.0f)
			)
		];

		MetricsGrid->AddSlot(0, 1)
		[
			MakeMetricCard(
				TEXT("Added Ops"),
				FString::Printf(TEXT("%d"), TotalAddedOps),
				TEXT("Tracked add operations"),
				FLinearColor(0.35f, 0.85f, 0.45f, 1.0f)
			)
		];

		MetricsGrid->AddSlot(1, 1)
		[
			MakeMetricCard(
				TEXT("Removed Ops"),
				FString::Printf(TEXT("%d"), TotalRemovedOps),
				TEXT("Tracked remove operations"),
				FLinearColor(0.95f, 0.35f, 0.35f, 1.0f)
			)
		];

		MetricsGrid->AddSlot(2, 1)
		[
			MakeMetricCard(
				TEXT("Updated Ops"),
				FString::Printf(TEXT("%d"), TotalUpdatedOps),
				TEXT("Tracked update operations"),
				FLinearColor(0.95f, 0.80f, 0.35f, 1.0f)
			)
		];

		MetricsGrid->AddSlot(3, 1)
		[
			MakeMetricCard(
				TEXT("Full Bags"),
				FString::Printf(TEXT("%d"), FullBags),
				TEXT("Bags at max slot capacity"),
				FLinearColor(0.85f, 0.45f, 1.0f, 1.0f)
			)
		];

		ContentBox->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 14)
		[
			MetricsGrid
		];

		TSharedRef<SVerticalBox> InsightsBox = SNew(SVerticalBox);

		InsightsBox->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 8)
		[
			MakeInfoRow(
				TEXT("Most loaded bag"),
				MostLoadedBag
					? FString::Printf(TEXT("%s (%d qty)"), *MostLoadedBag->BagName, MostLoadedBag->TotalQuantity)
					: TEXT("N/A"),
				FLinearColor(0.45f, 0.85f, 1.0f, 1.0f)
			)
		];

		InsightsBox->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 8)
		[
			MakeInfoRow(
				TEXT("Most active bag"),
				MostActiveBag
					? FString::Printf(TEXT("%s (%d ops)"), *MostActiveBag->BagName, MostActiveBag->TotalActivity)
					: TEXT("N/A"),
				FLinearColor(1.0f, 0.75f, 0.35f, 1.0f)
			)
		];

		InsightsBox->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 8)
		[
			MakeInfoRow(
				TEXT("Global fill"),
				FString::Printf(TEXT("%.0f%%"), GlobalFillPercent),
				FLinearColor(0.75f, 0.95f, 0.55f, 1.0f)
			)
		];

		InsightsBox->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 8)
		[
			MakeInfoRow(
				TEXT("Average stacks / bag"),
				FString::Printf(TEXT("%.1f"), AvgItemsPerBag)
			)
		];

		InsightsBox->AddSlot()
		.AutoHeight()
		[
			MakeInfoRow(
				TEXT("Average quantity / bag"),
				FString::Printf(TEXT("%.1f"), AvgQuantityPerBag)
			)
		];

		ContentBox->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 14)
		[
			MakePanel(TEXT("Quick Insights"), InsightsBox)
		];

		TSharedRef<SVerticalBox> TopBagsBox = SNew(SVerticalBox);

		if (BagStats.Num() <= 0)
		{
			TopBagsBox->AddSlot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString("No bags detected. Start PIE and make sure your actors own UAC_ItemsBag components."))
				.ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f))
			];
		}
		else
		{
			const int32 MaxDisplayedBags = FMath::Min(5, BagStats.Num());

			for (int32 i = 0; i < MaxDisplayedBags; ++i)
			{
				const FBagDashboardData& Bag = BagStats[i];

				TopBagsBox->AddSlot()
				.AutoHeight()
				.Padding(0, 0, 0, 8)
				[
					SNew(SBorder)
					.Padding(10)
					.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
					.BorderBackgroundColor(FLinearColor(0.08f, 0.08f, 0.10f, 1.0f))
					[
						SNew(SVerticalBox)

						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							MakeInfoRow(
								FString::Printf(TEXT("#%d  %s"), i + 1, *Bag.BagName),
								FString::Printf(TEXT("%d qty"), Bag.TotalQuantity),
								FLinearColor(0.45f, 0.85f, 1.0f, 1.0f)
							)
						]

						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 4, 0, 0)
						[
							MakeInfoRow(TEXT("Owner"), Bag.OwnerName)
						]

						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 2, 0, 0)
						[
							MakeInfoRow(
								TEXT("Slots"),
								FString::Printf(TEXT("%d / %d (%.0f%%)"), Bag.DistinctItems, Bag.Capacity, Bag.FillPercent),
								FLinearColor(0.75f, 0.95f, 0.55f, 1.0f)
							)
						]

						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 2, 0, 0)
						[
							MakeInfoRow(
								TEXT("Activity"),
								FString::Printf(TEXT("%d ops"), Bag.TotalActivity),
								FLinearColor(1.0f, 0.75f, 0.35f, 1.0f)
							)
						]
					]
				];
			}
		}

		TSharedRef<SVerticalBox> TopItemsBox = SNew(SVerticalBox);

		if (ItemStats.Num() <= 0)
		{
			TopItemsBox->AddSlot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString("No items found."))
				.ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f, 1.0f))
			];
		}
		else
		{
			const int32 MaxDisplayedItems = FMath::Min(5, ItemStats.Num());

			for (int32 i = 0; i < MaxDisplayedItems; ++i)
			{
				const FItemDashboardData& Item = ItemStats[i];

				TopItemsBox->AddSlot()
				.AutoHeight()
				.Padding(0, 0, 0, 8)
				[
					SNew(SBorder)
					.Padding(10)
					.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
					.BorderBackgroundColor(FLinearColor(0.08f, 0.08f, 0.10f, 1.0f))
					[
						SNew(SVerticalBox)

						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							MakeInfoRow(
								FString::Printf(TEXT("#%d  %s"), i + 1, *Item.Name),
								FString::Printf(TEXT("%d qty"), Item.TotalQuantity),
								FLinearColor(0.55f, 0.85f, 1.0f, 1.0f)
							)
						]

						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 4, 0, 0)
						[
							MakeInfoRow(
								TEXT("Stacks"),
								FString::Printf(TEXT("%d"), Item.StackCount)
							)
						]
					]
				];
			}
		}

		ContentBox->AddSlot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(0, 0, 7, 0)
			[
				MakePanel(TEXT("Top Bags"), TopBagsBox)
			]

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(7, 0, 0, 0)
			[
				MakePanel(TEXT("Top Items"), TopItemsBox)
			]
		];
	}

	TArray<TWeakObjectPtr<UAC_ItemsBag>> BoundBags;

	void BindBagEvents()
	{
		UnbindBagEvents();

		const TArray<UAC_ItemsBag*> Bags = GetLiveBags();

		for (UAC_ItemsBag* Bag : Bags)
		{
			if (!Bag)
			{
				continue;
			}

			Bag->OnInventoryChangedNative.RemoveAll(this);

			Bag->OnInventoryChangedNative.AddRaw(
				this,
				&SInventoryDashboardWindow::OnInventoryChanged
			);

			BoundBags.Add(Bag);
		}
	}

	void UnbindBagEvents()
	{
		for (TWeakObjectPtr<UAC_ItemsBag> WeakBag : BoundBags)
		{
			if (WeakBag.IsValid())
			{
				WeakBag->OnInventoryChangedNative.RemoveAll(this);
			}
		}

		BoundBags.Empty();
	}

	void OnInventoryChanged(
		UAC_ItemsBag* Bag,
		EProInventoryChangeType ChangeType,
		FS_Item Item
	)
	{
		RefreshDashboard();
	}
};

// FIN CLASS CUSTOM

void FEditorInventoryViewerModule::OnDashboard()
{
	TSharedRef<SWindow> DashboardWindow = SNew(SWindow)
		.Title(FText::FromString("Inventory Dashboard"))
		.ClientSize(FVector2D(1180, 760))
		.MinHeight(600)
		.MinWidth(800)
		.MaxHeight(800)
		.MaxWidth(1280)
		.SupportsMaximize(true)
		.SupportsMinimize(true)
		[
			SNew(SInventoryDashboardWindow)
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