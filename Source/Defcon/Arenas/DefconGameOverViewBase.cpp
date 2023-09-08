// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconGameOverViewBase.h"
#include "DaylonUtils.h"
#include "Globals/_sound.h"
#include "DefconGameInstance.h"
#include "Missions/MilitaryMissions/MilitaryMission.h"
#include "Common/util_color.h"
#include "DefconUtils.h"
#include "DefconLogging.h"

#include "UMG/Public/Components/CanvasPanelSlot.h"
#include "UMG/Public/Components/GridPanel.h"



// -----------------------------------------------------------------------------------------

constexpr float StartingTextX      = 3000.0f;
constexpr float TimeToCenterText   = 1.0f;
constexpr float TimeToRead         = 2.0f;
constexpr float TimeToIntroStats   = TimeToCenterText + TimeToRead;
constexpr float StatsIntroDuration = 3.0f;
constexpr float TimeStatsIntroEnds = TimeToIntroStats + StatsIntroDuration;
constexpr float TimeForOneHuman  = 0.1f;


void UDefconGameOverViewBase::NativeOnInitialized()
{
	LOG_UWIDGET_FUNCTION

	Super::NativeOnInitialized();

	StatItemShotsFired           = TEXT("ShotsFired");
	StatItemSmartbombsDetonated  = TEXT("SmartBombsDetonated");
	StatItemLaserKills           = TEXT("LaserKills");
	StatItemSmartbombKills       = TEXT("SmartbombKills");
	StatItemFFIs                 = TEXT("FFIncidents");
	StatItemHits                 = TEXT("Hits");
	StatItemCollisions           = TEXT("Collisions");
	StatItemDeaths               = TEXT("Deaths");
}


void UDefconGameOverViewBase::OnActivate()
{
	LOG_UWIDGET_FUNCTION
	Super::OnActivate();

	// Start the title texts way offstage.
	// Assume they are at center horizontally.

	auto Mission = GDefconGameInstance->GetMission();

	if(Mission == nullptr && Defcon::GGameMatch->GetHumans().Count() > 0)
	{
		SetTitle(TEXT("VICTORY"));
		SetSubtitle(TEXT("You defended the humans"));
	}
	else
	{
		SetTitle(TEXT("DEFEAT"));
		SetSubtitle(TEXT("All humans were killed or abducted"));
	}

	// todo: involve viewportsize.X
	PositionTexts(-StartingTextX, StartingTextX);


	auto Children = Stats->GetAllChildren();

	for(auto Child : Children)
	{
		Child->SetRenderOpacity(0.0f);
	}
}


void UDefconGameOverViewBase::OnFinishActivating()
{
	LOG_UWIDGET_FUNCTION
	Super::OnFinishActivating();


	const auto& GameStats = GDefconGameInstance->GetStats();

	int32 Index;


	#define SETSTAT(_key, _val)	\
		{	\
			auto TextBlock = Cast<UTextBlock>(WidgetTree->FindWidgetChild(Stats, _key, Index)); \
			const auto Str = FString::Printf(TEXT("%d"), (_val));	\
			TextBlock->SetText(FText::FromString(Str));	\
		}

	SETSTAT(StatItemShotsFired,           GameStats.ShotsFired)
	SETSTAT(StatItemSmartbombsDetonated,  GameStats.SmartbombsDetonated)
	SETSTAT(StatItemLaserKills,           GameStats.HostilesDestroyedByLaser)
	SETSTAT(StatItemSmartbombKills,       GameStats.HostilesDestroyedBySmartbomb)
	SETSTAT(StatItemFFIs,                 GameStats.FriendlyFireIncidents)
	SETSTAT(StatItemHits,                 GameStats.PlayerHits)
	SETSTAT(StatItemCollisions,           GameStats.PlayerCollisions)
	SETSTAT(StatItemDeaths,               GameStats.PlayerDeaths)
}


void UDefconGameOverViewBase::PositionTexts(float XTitle, float XSubtitle)
{
	auto CanvasSlot = Cast<UCanvasPanelSlot>(Title->Slot);

	auto P = CanvasSlot->GetPosition();
	P.X = XTitle;
	CanvasSlot->SetPosition(P);

	CanvasSlot = Cast<UCanvasPanelSlot>(Subtitle->Slot);

	P = CanvasSlot->GetPosition();
	P.X = XSubtitle; 
	CanvasSlot->SetPosition(P);
}


UWidget* FindValueLabel(const UGridPanel* GridPanel, const UWidget* ValueWidget)
{
	// Since UGridPanel has no way to find children by row/col, this function
	// will use Y-positions to find the label for a given value text block.

	const auto Children = GridPanel->GetAllChildren();

	for(const auto Child : Children)
	{
		if(Child != ValueWidget && Child->GetCachedGeometry().AbsolutePosition.Y == ValueWidget->GetCachedGeometry().AbsolutePosition.Y)
		{
			return Child;
		}
	}

	return nullptr;
}


void UDefconGameOverViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	LOG_UWIDGET_FUNCTION
	Super::NativeTick(MyGeometry, DeltaTime);
	
	const float T = FMath::Min(1.0f, Age / TimeToCenterText);

	const float X = FMath::Lerp(StartingTextX, 0.0f, T);

	PositionTexts(-X, X);

	// Pulse the subtitle.
	
	const float T2 = (float)fabs(sin(Age * PI * 2));
	const FSlateColor Color(FLinearColor::LerpUsingHSV(C_RED, C_YELLOW, T2));

	Subtitle->SetColorAndOpacity(Color);

	// Introduce the stats row by row.

	if(Age >= TimeToIntroStats && Age <= TimeStatsIntroEnds)
	{
		const TArray<FName> StatNames =
		{
			StatItemShotsFired,
			StatItemSmartbombsDetonated,
			StatItemLaserKills,
			StatItemSmartbombKills,
			StatItemFFIs,
			StatItemHits,
			StatItemCollisions,
			StatItemDeaths
		};

		const float TotalOpacities = array_size(StatNames);

		int32 ChildIndex;

		const float OsStep = (1.0f / StatsIntroDuration) / StatNames.Num();

		int32 Row = (int32)MAP(Age, TimeToIntroStats, TimeStatsIntroEnds, 0.0f, (float)StatNames.Num());
		Row = FMath::Min(Row, StatNames.Num() - 1);

		auto ValueTextBlock = WidgetTree->FindWidgetChild(Stats, StatNames[Row], ChildIndex);

		const float Os = ValueTextBlock->GetRenderOpacity() + OsStep;

		ValueTextBlock->SetRenderOpacity(Os);

		auto KeyTextBlock = FindValueLabel(Stats, ValueTextBlock);

		if(KeyTextBlock != nullptr)
		{
			KeyTextBlock->SetRenderOpacity(Os);
		}
	}

	// Animate the color of the stats.

	auto Children = Stats->GetAllChildren();

	for(auto Child : Children)
	{
		auto TextBlock = Cast<UTextBlock>(Child);
		if(TextBlock == nullptr)
		{
			continue;
		}
		
		TextBlock->SetColorAndOpacity(MakeBlendedColor(C_RED, C_YELLOW, PSIN(Age)));
	}
}


void UDefconGameOverViewBase::SetTexts(const FString& InTitle, const FString& InSubtitle)
{
	Title->SetText(FText::FromString(InTitle));
	Subtitle->SetText(FText::FromString(InSubtitle));
}


void UDefconGameOverViewBase::SetTitle    (const FString& Str) { Title->SetText(FText::FromString(Str)); }
void UDefconGameOverViewBase::SetSubtitle (const FString& Str) { Subtitle->SetText(FText::FromString(Str)); }


void UDefconGameOverViewBase::OnEscPressed()
{
	TransitionToArena(EDefconArena::MainMenu);
}

void UDefconGameOverViewBase::OnSkipPressed()
{
	if(Age < TimeToIntroStats)
	{
		// Still waiting for stats to be introduced, so skip ahead to that.
		Age = TimeToIntroStats;
		return;
	}

	if(Age < TimeStatsIntroEnds)
	{
		// Still waiting for stats to finish being introduced, so skip ahead to that.
		Age = TimeStatsIntroEnds;

		auto Children = Stats->GetAllChildren();

		for(auto Child : Children)
		{
			Child->SetRenderOpacity(1.0f);
		}

		return;
	}

	// Stats have been fully introduced, so skip to main menu.
	TransitionToArena(EDefconArena::MainMenu);
}

