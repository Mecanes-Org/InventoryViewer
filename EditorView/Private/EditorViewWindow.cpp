// Copyright 2025,  Mecanes . All Rights Reserved.

#include "EditorViewWindow.h"

//WIDGET
//#include "EditorStyleSet.h"
#include "Widgets/SCanvas.h"

#include "ProInventorySystem/Public/BPFL_ProInventorySystem.h"
#include "EngineUtils.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"

void SEditorViewWindow::Construct(const FArguments& InArgs)
{
	
	//UE_LOG(LogTemp, Warning, TEXT("Construct"));
	//VIEW
	GameStart();
	
}

SEditorViewWindow::~SEditorViewWindow()
{
	UnbindBagEvents();
}

void SEditorViewWindow::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	
	TimeSinceLastTick += InDeltaTime;
	
	if (TimeSinceLastTick >= TimerInterval && bAutomaticUpdate)
	{
		StartTimer();

		TimeSinceLastTick = 0;
	}
}

void SEditorViewWindow::ShowAllItems(UAC_ItemsBag* Bag)
{
	if (!SWrapBoxRight.IsValid())
	{
		return;
	}

	SWrapBoxRight->ClearChildren();

	if (!Bag)
	{
		SWrapBoxRight->AddSlot()
		.Padding(10)
		[
			SNew(STextBlock)
			.Text(FText::FromString("No bag selected"))
			.ColorAndOpacity(FLinearColor::White)
		];

		return;
	}
	
	AC_ItemsBag = Bag;

	if (AC_ItemsBag->ItemsBag.Num() <= 0)
	{
		SWrapBoxRight->AddSlot()
		.Padding(10)
		[
			SNew(STextBlock)
			.Text(FText::FromString("This bag is empty"))
			.ColorAndOpacity(FLinearColor::White)
		];

		return;
	}
	
	for (const FS_Item& Item : AC_ItemsBag->ItemsBag)
	{
		SWrapBoxRight->AddSlot()
		.Padding(8)
		[
			CreateButtonItem(Item)
		];
	}

}


void SEditorViewWindow::GameStart()
{
	//Invalidate(EInvalidateWidget::Layout);
	
	if (IsValidWorld(GetWorld()) )
	{
		ActorFromComponent();
		//UE_LOG(LogTemp, Warning, TEXT("GameStart"));
	}

	//After Game start
	AfterGameStart();

}

void SEditorViewWindow::AfterGameStart()
{
	
	ChildSlot[
	SNew(SCanvas)
	+ SCanvas::Slot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	.Size(FVector2D(1000, 600))
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.0f, 0.0f, 5.0f, 0.0f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill) // Permet au slot de remplir la hauteur disponible
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill) // Assure l'extension du border pour remplir la hauteur
			.Padding(3)
			.BorderBackgroundColor(FLinearColor::White)
			[
				SNew(SScrollBox)
				.ScrollBarVisibility(EVisibility::Visible)
				+ SScrollBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill) // S'assure que le ScrollBox s'étend également sur toute la hauteur
				[
					SAssignNew(ScrollBoxContentLeft, SVerticalBox)
				]
			]
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill) // Le deuxième slot remplit également toute la hauteur disponible
		[
			SNew(SBorder) // On utilise un SBorder pour harmoniser le rendu et bien encapsuler l'image
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(3) // Espacement entre le bord et le contenu
			.BorderBackgroundColor(FLinearColor::White) // Couleur d'arrière-plan optionnelle
			[
				SNew(SScrollBox)
				.ScrollBarVisibility(EVisibility::Visible)
				.Orientation(Orient_Vertical)
				
				+ SScrollBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)// S'assure que le ScrollBox s'étend également sur toute la hauteur
				[
					SAssignNew(SWrapBoxRight, SWrapBox)
					.UseAllottedSize(true)
					.HAlign(HAlign_Center)
				]
				
			]
		]
	]
	];
	

	if (ScrollBoxContentLeft.IsValid())
	{
		//UE_LOG(LogTemp, Warning, TEXT("ScrollBoxContentLeft"));
		
		if (ArrayAc_ItemsBag.Num() > 0)
		{
			for (UAC_ItemsBag* bag : ArrayAc_ItemsBag)
			{
				ScrollBoxContentLeft->AddSlot()
				.Padding(5) // Espacement entre les widgets
				.HAlign(HAlign_Center) // Alignement horizontal
				.VAlign(VAlign_Center) // Alignement vertical
				[
					CreateButtonBag(bag) 
				];
			}

			AC_ItemsBag = ArrayAc_ItemsBag[0];
			
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("ArrayAc_ItemsBag.Num() == 0"));
			ScrollBoxContentLeft->AddSlot()
			.Padding(5) // Espacement entre les widgets
			.HAlign(HAlign_Center) // Alignement horizontal
			.VAlign(VAlign_Center) // Alignement vertical
			[
				CreateButtonBag(NULL) 
			];
		}
		
	}

	//AFFICHE TOUTES LES ITEMS D UN SAC
	ShowAllItems(AC_ItemsBag);
}


void SEditorViewWindow::Set_WindowHandle(TSharedRef<SWindow> InWindowHandle)
{
	WindowHandle = InWindowHandle;
}

UWorld* SEditorViewWindow::GetWorld()
{
	if (GEditor && GEditor->IsPlayingSessionInEditor())  // Vérifiez que vous êtes en mode PIE
	{
		UWorld* World = GEditor->PlayWorld;
		
		if (World)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Le monde a été récupéré dans le module d'éditeur!"));
			CurrentWorld = World;
			return World;
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Le monde est invalide ou GEditor non initialisé"));
	return nullptr;
	
}

bool SEditorViewWindow::IsValidWorld(UWorld* World)
{
	
	return World != nullptr;
}

void SEditorViewWindow::ActorFromComponent()
{
	ArrayAc_ItemsBag.Empty();

	UWorld* ActiveWorld = CurrentWorld;

	if (!ActiveWorld)
	{
		return;
	}

	for (TActorIterator<AActor> ActorItr(ActiveWorld); ActorItr; ++ActorItr)
	{
		AActor* CurrentActor = *ActorItr;

		if (!CurrentActor)
		{
			continue;
		}

		TArray<UAC_ItemsBag*> FoundBags = UBPFL_ProInventorySystem::GetAllBags(CurrentActor);

		for (UAC_ItemsBag* Bag : FoundBags)
		{
			if (Bag)
			{
				ArrayAc_ItemsBag.AddUnique(Bag);
			}
		}
	}

	if (!AC_ItemsBag && ArrayAc_ItemsBag.Num() > 0)
	{
		AC_ItemsBag = ArrayAc_ItemsBag[0];
	}

	BindBagEvents();
}

FSlateBrush* SEditorViewWindow::CreateBrushFromTexture(UTexture2D* Texture)
{
	if (Texture)
	{
		FSlateBrush* Brush = new FSlateBrush();
		Brush->SetResourceObject(Texture);
		Brush->ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
		return Brush;
	}
	return nullptr;
}

void SEditorViewWindow::CloseWindow()
{
	if (WindowHandle.IsValid())
	{
		//UE_LOG(LogTemp, Warning, TEXT("CloseWindow"));
		FSlateApplication::Get().RequestDestroyWindow(WindowHandle.ToSharedRef());
	}
	//UE_LOG(LogTemp, Warning, TEXT("Don t CloseWindow"));
}

TSharedRef<SWidget> SEditorViewWindow::CreateButtonBag(UAC_ItemsBag* bag)
{
	if (bag != NULL)
	{
		return SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill) // Permet au slot de remplir la hauteur disponible
			[
				SNew(SBorder)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.BorderBackgroundColor(FLinearColor::White)
				.Padding(5)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill) // Permet au slot de remplir la largeur disponible
					[
						SNew(SBox)
						.WidthOverride(150)
						.HeightOverride(150)
						.Padding(0.0f, 0.0f, 0.0f, 5.0f)
						.VAlign(VAlign_Fill)
						[
							SNew(SImage)
							.Image(FCoreStyle::Get().GetBrush(TEXT("Icons.Warning")))
							.Image( CreateBrushFromTexture(bag->BagImage.Image_Item) )
							.ColorAndOpacity(FLinearColor::White)
						]
						
					]
					+ SVerticalBox::Slot()
					  .HAlign(HAlign_Fill)
					  .VAlign(VAlign_Bottom) // Permet au slot de remplir la largeur disponible
					  .AutoHeight()
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString::Printf(TEXT("%s"), *bag->getBagName() ))) 
						.ColorAndOpacity(FLinearColor::White)
						.Font(FCoreStyle::Get().GetFontStyle("BoldFont"))
						.Justification(ETextJustify::Center)
					]
				]
			]

			//NOMBRE DE SAC
			+ SOverlay::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			.Padding(1.0f, 1.0f, 0.0f, 5.0f)
			[
				SNew(SBorder)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.BorderBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.5f))
				.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")) // Utilisation d'une brosse blanche par défaut
				.Padding(4.0f, 2.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(FString::Printf(TEXT("%i"), bag->ItemsBag.Num() )))
					.ColorAndOpacity(FLinearColor::Black)
				]
			]

			//BUTTON
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill) // Permet au slot de remplir la hauteur disponible
			[
				SNew(SButton)
				.ButtonColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f))
				.Cursor(EMouseCursor::Hand)
				.OnClicked_Lambda([this, bag]() -> FReply
				{
					// Your action here
					//UE_LOG(LogTemp, Warning, TEXT("Button Clicked"));
					UpdateItemOfBag(bag);
					
					return FReply::Handled();
				})
			];
		
	}
	else
	{
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill) // Permet au slot de remplir la largeur disponible
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill) // Permet au slot de remplir la hauteur disponible
				[
					SNew(SBorder)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.BorderBackgroundColor(FLinearColor::Red)
					.Padding(5)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill) // Permet au slot de remplir la largeur disponible
						[
							SNew(SBox)
							.WidthOverride(150)
							.HeightOverride(150)
							.Padding(0.0f, 0.0f, 0.0f, 5.0f)
							.VAlign(VAlign_Fill)
							[
								SNew(SImage)
								.Image(FCoreStyle::Get().GetBrush(TEXT("Icons.Warning")))
								.ColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 0.5f))
							]
								
						]
						+ SVerticalBox::Slot()
						  .HAlign(HAlign_Fill)
						  .VAlign(VAlign_Bottom) // Permet au slot de remplir la largeur disponible
						  .AutoHeight()
						[
							SNew(STextBlock)
							.Text(FText::FromString("none"))
							.ColorAndOpacity(FLinearColor::White)
							.Font(FCoreStyle::Get().GetFontStyle("BoldFont"))
							.Justification(ETextJustify::Center)
						]
					]
				]
			];
	}
}


TSharedRef<SWidget> SEditorViewWindow::CreateButtonItem(FS_Item Item)
{
	return SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill) // Permet au slot de remplir la hauteur disponible
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.BorderBackgroundColor(FLinearColor(0.035f, 0.035f, 0.045f, 1.0f))
			.Padding(5)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill) // Permet au slot de remplir la largeur disponible
				[
					SNew(SBox)
					.WidthOverride(150)
					.HeightOverride(150)
					.Padding(10.0f, 0.0f, 10.0f, 5.0f)
					.VAlign(VAlign_Fill)
					[
						SNew(SImage)
						.Image( CreateBrushFromTexture(Item.Image.Image_Item) )
					]
					
				]
				
				+ SVerticalBox::Slot()
				  .HAlign(HAlign_Fill)
				  .VAlign(VAlign_Bottom) // Permet au slot de remplir la largeur disponible
				  .AutoHeight()
				[
					SNew(STextBlock)
					.Text(FText::FromString(FString::Printf(TEXT("%s"), *Item.Name ))) 
					.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.95f, 1.0f))
					.Font(FCoreStyle::Get().GetFontStyle("BoldFont"))
					.Justification(ETextJustify::Center)
				]
			]
		]

		+ SOverlay::Slot()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Left)
		.Padding(1.0f, 1.0f, 0.0f, 5.0f)
		[
			SNew(SBorder)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			.BorderBackgroundColor(FLinearColor(0.1f, 0.25f, 0.9f, 1.0f))
			.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush")) // Utilisation d'une brosse blanche par défaut
			.Padding(4.0f, 2.0f)
			[
				SNew(STextBlock)
				.Text(FText::FromString(FString::Printf(TEXT("%i"), Item.Quantity )))
				.ColorAndOpacity(FLinearColor(0.9f, 0.9f, 0.95f, 1.0f))
			]
		];
		
}


void SEditorViewWindow::UpdateItemOfBag(UAC_ItemsBag* bag)
{
	ShowAllItems(bag);
}

void SEditorViewWindow::UpdateWindow(bool bAutoUpdate)
{
	bAutomaticUpdate = bAutoUpdate;

	if (bAutomaticUpdate)
	{
		TimerInterval = 0.25f;
		TimeSinceLastTick = 0.0f;
		StartTimer();
	}
}

void SEditorViewWindow::StartTimer()
{	
	if (IsValidWorld(GetWorld()))
	{
		ActorFromComponent();
		RefreshBagList();
	}

	if (AC_ItemsBag)
	{
		ShowAllItems(AC_ItemsBag);
	}
	else if (ArrayAc_ItemsBag.Num() > 0)
	{
		AC_ItemsBag = ArrayAc_ItemsBag[0];
		ShowAllItems(AC_ItemsBag);
	}
}

void SEditorViewWindow::RefreshBagList()
{
	if (!ScrollBoxContentLeft.IsValid())
	{
		return;
	}

	ScrollBoxContentLeft->ClearChildren();

	if (ArrayAc_ItemsBag.Num() <= 0)
	{
		ScrollBoxContentLeft->AddSlot()
		.Padding(5)
		[
			CreateButtonBag(nullptr)
		];

		return;
	}

	for (UAC_ItemsBag* Bag : ArrayAc_ItemsBag)
	{
		if (!Bag)
		{
			continue;
		}

		ScrollBoxContentLeft->AddSlot()
		.Padding(5)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			CreateButtonBag(Bag)
		];
	}
}

void SEditorViewWindow::BindBagEvents()
{
	for (UAC_ItemsBag* Bag : ArrayAc_ItemsBag)
	{
		if (!Bag)
		{
			continue;
		}

		Bag->OnInventoryChangedNative.RemoveAll(this);
		Bag->OnInventoryChangedNative.AddRaw(
			this,
			&SEditorViewWindow::OnBagInventoryChanged
		);
	}
}

void SEditorViewWindow::UnbindBagEvents()
{
	for (UAC_ItemsBag* Bag : ArrayAc_ItemsBag)
	{
		if (Bag)
		{
			Bag->OnInventoryChangedNative.RemoveAll(this);
		}
	}
}

void SEditorViewWindow::OnBagInventoryChanged(UAC_ItemsBag* Bag, EProInventoryChangeType ChangeType, FS_Item Item)
{
	if (!Bag)
	{
		return;
	}

	if (Bag == AC_ItemsBag)
	{
		ShowAllItems(Bag);
	}

	RefreshBagList();
}
