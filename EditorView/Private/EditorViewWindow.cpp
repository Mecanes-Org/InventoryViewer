// Copyright 2025,  Mecanes . All Rights Reserved.

#include "EditorViewWindow.h"

//WIDGET
//#include "EditorStyleSet.h"
#include "Widgets/SCanvas.h"

#include "ProInventorySystem/Public/BPFL_ProInventorySystem.h"
#include "EngineUtils.h"

void SEditorViewWindow::Construct(const FArguments& InArgs)
{
	GameStart();
	
	//UE_LOG(LogTemp, Warning, TEXT("Construct"));
	//VIEW
	
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
			.BorderBackgroundColor(FLinearColor::Blue)
			[
				SNew(SScrollBox)
				.ScrollBarVisibility(EVisibility::Collapsed)
				+ SScrollBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill) // S'assure que le ScrollBox s'étend également sur toute la hauteur
				[
					SAssignNew(ScrollBoxContent, SVerticalBox)
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
			.BorderBackgroundColor(FLinearColor::Green) // Couleur d'arrière-plan optionnelle
			[
				SNew(SImage)
				.ColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f))
			]
		]
	]
];

	if (ScrollBoxContent.IsValid())
	{
		//UE_LOG(LogTemp, Warning, TEXT("ScrollBoxContent"));
		
		if (ArrayAc_ItemsBag.Num() > 1)
		{
			for (UAC_ItemsBag* bag : ArrayAc_ItemsBag)
			{
				ScrollBoxContent->AddSlot()
				.Padding(5) // Espacement entre les widgets
				.HAlign(HAlign_Center) // Alignement horizontal
				.VAlign(VAlign_Center) // Alignement vertical
				[
					CreateButtonBag(bag) 
				];
			}
			
		}
		else if (ArrayAc_ItemsBag.Num() == 1)
		{
			//UE_LOG(LogTemp, Warning, TEXT("ArrayAc_ItemsBag.Num() == 1"));
			ScrollBoxContent->AddSlot()
				.Padding(5) // Espacement entre les widgets
				.HAlign(HAlign_Center) // Alignement horizontal
				.VAlign(VAlign_Center) // Alignement vertical
				[
					CreateButtonBag(AC_ItemsBag) 
				];
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("ArrayAc_ItemsBag.Num() == 0"));
			ScrollBoxContent->AddSlot()
				.Padding(5) // Espacement entre les widgets
				.HAlign(HAlign_Center) // Alignement horizontal
				.VAlign(VAlign_Center) // Alignement vertical
				[
					CreateButtonBag(NULL) 
				];
		}
		
	}
	
}


void SEditorViewWindow::GameStart()
{
	if (IsValidWorld(GetWorld()) )
	{
		ActorFromComponent();
		//UE_LOG(LogTemp, Warning, TEXT("GameStart"));
	}

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
	
	UE_LOG(LogTemp, Warning, TEXT("GetWorld"));
	//return GEditor->GetEditorWorldContext().World();
}

bool SEditorViewWindow::IsValidWorld(UWorld* World)
{
	//UE_LOG(LogTemp, Warning, TEXT("IsValidWorld"));
	/*
	if (World)
	{
		UE_LOG(LogTemp, Warning, TEXT("EXISTE IsValidWorld"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IN IsValidWorld"));
		//return World->IsGameWorld();
	}*/
	
	return World != nullptr;
}

void SEditorViewWindow::ActorFromComponent()
{
	//UE_LOG(LogTemp, Warning, TEXT("ActorFromComponent"));
	
	UWorld* ActiveWorld = CurrentWorld;


	// Vérifiez si le monde est valide avant de l'utiliser
	if (ActiveWorld)
	{
		for (TActorIterator<AActor> ActorItr(ActiveWorld); ActorItr; ++ActorItr)
		{
			AActor* CurrentActor = *ActorItr;
			if (!CurrentActor) continue;

			// Vérifiez si le composant UAC_ItemsBag existe pour cet acteur
			AC_ItemsBag = CurrentActor->FindComponentByClass<UAC_ItemsBag>();
			
			if (AC_ItemsBag)
			{
				// Obtenez le propriétaire (si nécessaire)
				AActor* CurrentOwner = AC_ItemsBag->GetOwner();

				if (CurrentOwner)
				{
					// Vérifiez de nouveau pour le composant UAC_ItemsBag dans l'acteur propriétaire
					AC_ItemsBag = CurrentOwner->FindComponentByClass<UAC_ItemsBag>();

					// Appelez une fonction personnalisée si au moins un sac est présent
					if (UBPFL_ProInventorySystem::GetNumberOfBags(CurrentOwner) > 0)
					{
						// Obtenez tous les sacs de l'acteur si nécessaire
						ArrayAc_ItemsBag = UBPFL_ProInventorySystem::GetAllBags(CurrentOwner);

						if (ArrayAc_ItemsBag.Num() == 1)
						{
							AC_ItemsBag = ArrayAc_ItemsBag[0];
						}
					}
				}
			}
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("Impossible de récupérer le monde actif!"));
	}
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
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill) // Permet au slot de remplir la hauteur disponible
				[
					SNew(SButton)
					.ButtonColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f))
					.Cursor(EMouseCursor::Hand)
					.OnClicked_Lambda([]() -> FReply
					{
						// Your action here
						//UE_LOG(LogTemp, Warning, TEXT("Button Clicked"));
						return FReply::Handled();
					})
				]
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
								.ColorAndOpacity(FLinearColor::Blue)
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

