// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconMissionPickerViewBase.h"
#include "DefconGameInstance.h"
#include "DefconLogging.h"
#include "DaylonUtils.h"
#include "Missions/MilitaryMissions/MilitaryMission.h"
#include "Globals/_sound.h"
#include "UMG/Public/Components/CanvasPanel.h"
#include "UMG/Public/Components/UniformGridSlot.h"
#include "UMG/Public/Components/Border.h"
//#include "UMG/Public/Blueprint/SlateBlueprintLibrary.h"


#define DEBUG_MODULE      1

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


const int32 CellsAcross      = 8;
const int32 CellsDown        = 5;
const float ExpanderLifetime = 1.1f;

// -----------------------------------------------------------------------------------------


void UDefconMissionPickerViewBase::NativeOnInitialized()
{
	LOG_UWIDGET_FUNCTION
	Super::NativeOnInitialized();


	// Get info on each mission.

	MissionInfos.Reserve((int32)Defcon::EMissionID::Count);

	Defcon::IMission* pMission = Defcon::CMissionFactory::Make(Defcon::EMissionID::First);

	while(pMission != nullptr)
	{
		FMissionInfo Missioninfo;
		Missioninfo.Name = pMission->GetName();
		Missioninfo.Desc = pMission->GetDesc();
		MissionInfos.Add(Missioninfo);
		Defcon::IMission* p = pMission;
		pMission = Defcon::CMissionFactory::MakeNext(pMission);
		delete p;
	}

	PopulateGrid();
}


void UDefconMissionPickerViewBase::OnActivate()
{
	LOG_UWIDGET_FUNCTION
	Super::OnActivate();

	Daylon::Hide(StartMissionExpander);
	Daylon::Show(RootCanvas);
	StartMissionExpander->SetRenderOpacity(0.0f);
	ExpanderAge = 0.0f;

	UpdateGridHighlights();
}


void UDefconMissionPickerViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	LOG_UWIDGET_FUNCTION
	Super::NativeTick(MyGeometry, DeltaTime);


	// Flash the highlight frame of the selected cell.

	auto BorderWidget = GetCellWidget(CurrentCell.X, CurrentCell.Y);

	if(BorderWidget == nullptr)
	{
		return;
	}

	const float Os = FMath::Sin(Age * 8) / 2 + 0.5f;

	BorderWidget->SetRenderOpacity(Os);

	if(StartMissionExpander->IsVisible())
	{
		ExpanderAge += DeltaTime;

		if(ExpanderAge >= ExpanderLifetime)
		{
			Daylon::Hide(RootCanvas);
			GDefconGameInstance->MissionID = (Defcon::EMissionID)(CurrentCell.Y * CellsAcross + CurrentCell.X);
			TransitionToArena(EDefconArena::Prewave);
		}
		else
		{
			const float T = ExpanderAge / ExpanderLifetime;
			StartMissionExpander->SetRenderOpacity(T);

			// Although our view layout is 1920 x 1080, the actual layout size can differ due to 
			// fitting an HD aspect ratio inside a different one, e.g. a full-size window that 
			// has a visible title bar (as happens when running in PIE mode). So we get the 
			// actual view layout size by taking the size of the root canvas.

			const auto CanvasSize = Daylon::GetWidgetSize(RootCanvas);

			auto CellWidget = GetCellWidget(CurrentCell.X, CurrentCell.Y);
			
			const auto CellP = Daylon::GetWidgetPosition(CellWidget);
			const auto CellS = Daylon::GetWidgetSize(CellWidget);

			auto CanvasSlot = Cast<UCanvasPanelSlot>(StartMissionExpander->Slot);

			const auto P = FVector2D(FMath::Lerp(CellP.X, 0, T), FMath::Lerp(CellP.Y, 0, T));
			const auto S = FVector2D(FMath::Lerp(CellS.X, CanvasSize.X, T), FMath::Lerp(CellS.Y, CanvasSize.Y, T));

			CanvasSlot->SetPosition(P);
			CanvasSlot->SetSize(S);
		}
	}
}


#if 0
int32 UDefconMissionPickerViewBase::NativePaint
(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled
) const
{
	LOG_UWIDGET_FUNCTION
	return Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);
}
#endif


UBorder* UDefconMissionPickerViewBase::GetCellWidget(int32 Column, int32 Row)
{
	if(Column < 0 || Column >= CellsAcross || Row < 0 || Row >= CellsDown)
	{
		UE_LOG(LogGame, Error, TEXT("%S: invalid cell address %d, %d"), __FUNCTION__, Column, Row);
		return nullptr;
	}

	auto GridChild = Cast<UUserWidget>(MissionsGrid->GetChildAt(Row * CellsAcross + Column));

	if(!IsValid(GridChild))
	{
		UE_LOG(LogGame, Error, TEXT("%S: no widget for cell %d, %d"), __FUNCTION__, Column, Row);
		return nullptr;
	}

	// todo: knowing the details of how the cell widget is built is not cool.
	// We need to set up a viewmodel etc. to abstract this away.

	// Border should be first child of canvas.
	auto Canvas = Cast<UCanvasPanel>(GridChild->GetRootWidget());

	if(!IsValid(Canvas))
	{
		UE_LOG(LogGame, Error, TEXT("%S: cell widget root is not a CanvasPanel widget at %d, %d"), __FUNCTION__, Column, Row);
		return nullptr;
	}

	auto BorderWidget = Cast<UBorder>(Canvas->GetChildAt(0));

	if(!IsValid(BorderWidget))
	{
		UE_LOG(LogGame, Error, TEXT("%S: cell widget root's child is not a Border widget at %d, %d"), __FUNCTION__, Column, Row);
		return nullptr;
	}

	return BorderWidget;
}


void UDefconMissionPickerViewBase::HighlightCell(int32 Column, int32 Row, bool bSelected)
{
	auto BorderWidget = GetCellWidget(Column, Row);

	if(BorderWidget == nullptr)
	{
		return;
	}

	BorderWidget->SetRenderOpacity(1.0f);
	const auto UnselectedColor = FLinearColor(0.0f, 0.0f, 1.0f, 0.5f);
	BorderWidget->SetBrushColor(bSelected ? FLinearColor::White : UnselectedColor);

	// Use thin margin if unselected, thicker otherwise.
	auto& Brush = BorderWidget->Background;

	//auto BorderWidth = bSelected ? 0.5f : 0.125f;
	const auto BorderWidth = (bSelected ? 0.25f : 0.0625f) * RootCanvas->GetPaintSpaceGeometry().Scale;

	Brush.Margin = FMargin(BorderWidth);
}


void UDefconMissionPickerViewBase::PopulateGrid()
{
	// Remove any design-time content from the grid.

	MissionsGrid->ClearChildren();

	// Populate grid.

	for(int32 Row = 0; Row < CellsDown; Row++)
	{
		for(int32 Column = 0; Column < CellsAcross; Column++)
		{
			auto GridChild = CreateWidget(this, MissionWidgetClass);
			check(IsValid(GridChild));
			auto ChildSlot = MissionsGrid->AddChildToUniformGrid(GridChild, Row, Column);
			ChildSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			ChildSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		}
	}
}


void UDefconMissionPickerViewBase::UpdateGridHighlights()
{
	for(int32 Row = 0; Row < CellsDown; Row++)
	{
		for(int32 Column = 0; Column < CellsAcross; Column++)
		{
			HighlightCell(Column, Row, false);
		}
	}

	HighlightCell(CurrentCell.X, CurrentCell.Y);

	const int32 Index = CurrentCell.Y * CellsAcross + CurrentCell.X;

	if(Index < (int32)Defcon::EMissionID::Count)
	{
		MissionName->SetText(FText::FromString(MissionInfos[Index].Name));
		MissionDesc->SetText(FText::FromString(MissionInfos[Index].Desc));
	}
	else
	{
		MissionName->SetText(FText::FromString(TEXT("Undefined")));
		MissionDesc->SetText(FText::FromString(TEXT("This mission has not been defined yet")));
	}
}


void UDefconMissionPickerViewBase::ChangeCurrentCell(int32 ColumnInc, int32 RowInc)
{
	if(ExpanderAge > 0.0f)
	{
		return;
	}

	GAudio->OutputSound(Defcon::EAudioTrack::Focus_changed);

	CurrentCell.X = ((CurrentCell.X + ColumnInc) + CellsAcross) % CellsAcross;
	CurrentCell.Y = ((CurrentCell.Y + RowInc)    + CellsDown  ) % CellsDown;
	UpdateGridHighlights();
}


void UDefconMissionPickerViewBase::OnSkipPressed()
{
	auto Where = CurrentCell;
	
	Where.X++;

	if(Where.X >= CellsAcross)
	{
		Where.X = 0;
		Where.Y = (Where.Y + 1) % CellsDown;
	}

	ChangeCurrentCell(Where.X - CurrentCell.X, Where.Y - CurrentCell.Y);
}


void UDefconMissionPickerViewBase::OnChooseMission()
{
	const int32 MissionID = CurrentCell.Y * CellsAcross + CurrentCell.X;

	if(MissionID >= (int32)Defcon::EMissionID::Count)
	{
		GAudio->OutputSound(Defcon::EAudioTrack::Invalid_selection);
		return;
	}

	if(!IsValid(StartMissionExpander))
	{
		return;
	}

	Daylon::Show(StartMissionExpander);

	GAudio->OutputSound(Defcon::EAudioTrack::Mission_chosen);

	GDefconGameInstance->SetScore(0);
	GDefconGameInstance->GetStats().Reset();

	GDefconGameInstance->SetCurrentMission((Defcon::EMissionID)MissionID);
}


void UDefconMissionPickerViewBase::OnEnterPressed()
{
	OnChooseMission();
}


void UDefconMissionPickerViewBase::OnNavEvent(ENavigationKey Key)
{
	switch(Key)
	{
		case ENavigationKey::Up:    ChangeCurrentCell(0, -1); break;
		case ENavigationKey::Down:  ChangeCurrentCell(0, 1);  break;
		case ENavigationKey::Left:  ChangeCurrentCell(-1, 0); break;
		case ENavigationKey::Right: ChangeCurrentCell(1, 0);  break;
	}
}


void UDefconMissionPickerViewBase::OnEscPressed()
{
	TransitionToArena(EDefconArena::MainMenu);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
