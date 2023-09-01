// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "DefconDetailsViewBase.h"
#include "DefconGameInstance.h"
#include "Globals/GameObjectResources.h"
#include "Common/util_color.h"
#include "DefconLogging.h"



#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


#if 0


void UDefconDetailsViewBase::OnActivate()
{
	LOG_UWIDGET_FUNCTION
	Super::OnActivate();
}


int32 UDefconDetailsViewBase::NativePaint
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


void UDefconDetailsViewBase::OnFinishActivating()
{
	LOG_UWIDGET_FUNCTION
	Super::OnFinishActivating();

	SafeToTick = false;

	// Until reassociated, all sprite install/uninstall takes place on us.

	Daylon::SetRootCanvas(RootCanvas);
	Daylon::SetWidgetTree(WidgetTree);

	// todo: should use a data table asset to store this stuff.

	if(Pages.IsEmpty())
	{
		FDetailsInfo Page;

#define ADD_PAGE(_ObjType, _Text)	\
	Page.ObjType = Defcon::EObjType::_ObjType; \
	Page.Text = FText::FromString(TEXT(_Text));	\
	Pages.Add(Page);

	ADD_PAGE(STARGATE, 
			"The Stargate: fly your ship into this artificial\n"
			"wormhole after destroying all mission targets\n"
			"to advance to the next mission.\n"
			"\n"
			"If you fly into it while humans are being abducted,\n"
			"your ship will teleport within attack range of\n"
			"the highest-priority target.\n"
			"\n"
			"With no abductions, the stargate will teleport\n"
			"your ship to the opposite side of the planet.");


	ADD_PAGE(LANDER, 
			"Landers descend towards the ground and\n"
			"look for humans to abduct. They fire round bullets\n"
			"whose frequency and aim increase over time.\n"
			"\n"
			"While hunting humans, landers stay a fixed distance\n"
			"above the ground as they hover above it,\n"
			"making them difficult to target.\n"
			"\n"
			"If you shoot an abducted human, the lander will\n"
			"become agressive and hunt you instead.\n"
			"\n"
			"If a lander carries a human to the top\n"
			"of the screen, both will be gone.");


		ADD_PAGE(DYNAMO,		
			"Dynamos float around harmlessly but spawn\n"
			"space hums, which try to collide with your ship.\n"
			"\n"
			"Dynamos spawn space hums more frequently over time.\n"
			"Destroy dynamos early to avoid having lots of\n"
			"space hums appear in later missions.");


		ADD_PAGE(SPACEHUM,		
			"Space hums try to collide with your ship.\n"
			"They fly faster over time.\n"
			"\n"
			"In large numbers they can pose a serious threat and\n"
			"also prevent other enemies from being shot.");


		ADD_PAGE(BAITER,	
			"Baiters appear late but are not mission targets.\n"
			"They move fast and fire round bullets.");


		ADD_PAGE(BOMBER,		
			"Bombers fly in an unpredictable wavy motion but\n"
			"always in their starting direction, and\n" 
			"occasionally drop multiple stationary mines.\n"
			"\n"
			"You should avoid following them too closely.\n"
			"Target them from a distance, or when they approach.");


		ADD_PAGE(POD,		
			"Pods float around harmlessly but when shot,\n"
			"explode into swarmers.");


		ADD_PAGE(SWARMER,		
			"Swarmers emerge from destroyed pods.\n"
			"They fly in a wavy motion and shoot round bullets.\n"
			"\n"
			"They can change direction to attack if your ship\n"
			"comes within range.\n"
			"\n"
			"Sometimes they materialize by themselves,\n"
			"in which case they are also mission targets.");


		ADD_PAGE(GUPPY,		
			"Yllabian space guppies hunt you slowly but erratically,\n"
			"shooting thin elongated bullets.\n"
			"\n"
			"They appear in large fleets during dogfights.");


		ADD_PAGE(FIREBOMBER_TRUE,		
			"Firebombers fly very erratically and can\n"
			"wraparound the screen vertically, making\n"
			"them particularly difficult to target.\n"
			"\n"
			"They shoot large reddish torpedos called fireballs.");


		ADD_PAGE(FIREBOMBER_WEAK,		
			"Weak firebombers look and move like firebombers\n"
			"but shoot normal round bullets.\n");


		ADD_PAGE(PHRED,	
			"Phreds appear late in a mission but are not\n"
			"mission targets. They move fast and try to collide\n"
			"with your ship.\n"
			"\n"
			"They will occasionally birth smaller versions\n"
			"of themselves called munchies.\n"
			"\n"
			"A red-colored phred is called Big Red.\n"
			"\n"
			"Like baiters, phreds will materialize with\n"
			"greater frequency over time, so it is in\n"
			"your interest to complete a mission quickly.");


		ADD_PAGE(HUNTER,		
			"Hunters hunt you slowly but erratically,\n"
			"shooting round bullets.\n");


		ADD_PAGE(GHOST,		
			"Ghosts are irregularly-shaped plasma beings\n"
			"that shoot round bullets.\n"
			"\n"
			"If approached too closely, ghosts will break apart and\n"
			"reform a short distance away. As the parts cannot\n"
			"be destroyed, you need to target this enemy\n"
			"from a distance.");


		ADD_PAGE(REFORMER,		
			"Refomers are machine beings that shoot round bullets.\n"
			"\n"
			"When shot, reformers will break apart and each part\n"
			"will seek out others to regroup with, creating a new reformer.\n"
			"The parts and new reformers are not mission targets.\n"
			"\n"
			"A reformer part all by itself will fly like a swarmer.");
	}

	#undef ADD_PAGE

	// Wait a moment before showing the first page, to give some time for widgets to establish positions etc.
	//StartupTask.When = 1.0f;
	//StartupTask.What = [this](){ ShowPage(0); StartupTask.What.Reset(); };
}


void UDefconDetailsViewBase::UninstallSprite()
{
	if(Sprite && Sprite->IsValid())
	{
		Daylon::Uninstall(Sprite);
		Sprite.Reset();
	}
}


void UDefconDetailsViewBase::OnDeactivate()
{
	LOG_UWIDGET_FUNCTION

	Super::OnDeactivate();

	UninstallSprite();

	TextReadout->SetText(FText::FromString(""));
}


void UDefconDetailsViewBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	LOG_UWIDGET_FUNCTION
	Super::NativeTick(MyGeometry, DeltaTime);

	//StartupTask.Tick(DeltaTime);

	// Wait until widgets have settled down before ticking.
	if(!SafeToTick && Daylon::GetWidgetPosition(TextReadout).X > 200)
	{
		SafeToTick = true;

		ShowPage(0);
	}

	if(SafeToTick)
	{
		if(Sprite && Sprite->IsValid())
		{
			Sprite->Update(DeltaTime);
		}

		TextReadout->Tick(DeltaTime);
	}
}


void UDefconDetailsViewBase::ShowNextPage()
{
	if(!TextReadout->IsFinished())
	{
		TextReadout->Finish();
		return;
	}

	CurrentPageIdx++;

	if(CurrentPageIdx >= Pages.Num())
	{
		TransitionToArena(EDefconArena::Help);
		return;
	}

	ShowPage(CurrentPageIdx);
}


void UDefconDetailsViewBase::ShowPage(int32 Idx)
{
	check(Pages.IsValidIndex(Idx));

	CurrentPageIdx = Idx;

	const auto& Page = Pages[CurrentPageIdx];

	// Change current sprite.

	UninstallSprite();

	// todo: need a "pose" texture atlas for ghost and reformer enemies.
	if(Page.ObjType != Defcon::EObjType::GHOST &&
	   Page.ObjType != Defcon::EObjType::REFORMER)
	{
		const auto& Info = GGameObjectResources.Get(Page.ObjType);
		Sprite = Daylon::SpawnSpritePlayObject2D(Info.Atlas->Atlas, Info.Size, Info.Radius);

		const auto TextReadoutP = Daylon::GetWidgetPosition(TextReadout);

		// Some sprites will be too large, so scale them down.
		const auto MaxSpriteWidth = TextReadoutP.X * 0.67f;
		auto SpriteSize = Info.Size * 2;

		if(SpriteSize.X > MaxSpriteWidth)
		{
			SpriteSize *= MaxSpriteWidth / SpriteSize.X;
		}

		Sprite->SetPosition(FVector2D(TextReadoutP.X / 2, TextReadoutP.Y + SpriteSize.Y / 2));
		Sprite->SetSize(SpriteSize);
		Sprite->UpdateWidgetSize();
	}

	// Change current text.

	TextReadout->SourceText = Page.Text;
	TextReadout->Reset();
}


void UDefconDetailsViewBase::OnEscPressed()
{
	TransitionToArena(EDefconArena::MainMenu);
}


void UDefconDetailsViewBase::OnSkipPressed()
{
	ShowNextPage();
}


void UDefconDetailsViewBase::OnEnterPressed()
{
	OnSkipPressed();
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
