// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UMG/Public/Blueprint/WidgetTree.h"
#include "UMG/Public/Components/Widget.h"
#include "UMG/Public/Components/CanvasPanel.h"
#include "DaylonWidgetUtils.generated.h"



class UTextBlock;


namespace Daylon
{
	extern UWidgetTree*  WidgetTree;
	extern UCanvasPanel* RootCanvas;

	DAYLONGRAPHICSLIBRARY_API void          SetWidgetTree             (UWidgetTree* InWidgetTree);
	DAYLONGRAPHICSLIBRARY_API UWidgetTree*  GetWidgetTree             ();
	DAYLONGRAPHICSLIBRARY_API void          SetRootCanvas             (UCanvasPanel* InCanvas);
	DAYLONGRAPHICSLIBRARY_API UCanvasPanel* GetRootCanvas             ();

	DAYLONGRAPHICSLIBRARY_API FVector2D     GetWidgetPosition         (const UWidget* Widget);
	DAYLONGRAPHICSLIBRARY_API FVector2D     GetWidgetSize             (const UWidget* Widget);
	DAYLONGRAPHICSLIBRARY_API FVector2D     GetWidgetDirectionVector  (const UWidget* Widget);
	DAYLONGRAPHICSLIBRARY_API void          Show                      (UWidget*, bool Visible = true);
	DAYLONGRAPHICSLIBRARY_API void          Show                      (SWidget*, bool Visible = true);
	DAYLONGRAPHICSLIBRARY_API void          Hide                      (UWidget* Widget);
	DAYLONGRAPHICSLIBRARY_API void          Hide                      (SWidget* Widget);
	DAYLONGRAPHICSLIBRARY_API void          UpdateRoundedReadout      (UTextBlock* Readout, float Value, int32& OldValue);

	// ----------------------------------------------------------------------------------------------------------------------------

	template<class WidgetT> static WidgetT* MakeWidget()
	{
		auto Widget = GetWidgetTree()->ConstructWidget<WidgetT>();
		check(Widget);
		return Widget;
	}

	// ----------------------------------------------------------------------------------------------------------------------------

	UENUM()
	enum class EListNavigationDirection : int32
	{
		// These values are casted to int
		Backwards = -1,
		Forwards  =  1,
	};

	// ----------------------------------------------------------------------------------------------------------------------------
	
	UENUM()
	enum class ERotationDirection : int32
	{
		// These values are casted to int
		CounterClockwise = -1,
		Clockwise        =  1,
	};
} // namespace Daylon
