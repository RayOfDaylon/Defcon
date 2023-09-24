// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


#include "DefconPlayStatsWidgetBase.h"
#include "Globals/MessageMediator.h"
#include "GameObjects/gameobjlive.h"
#include "Common/util_color.h"


void UDefconPlayStatsWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Build shield gradient colormap.
	
	const FLinearColor Colors[] = { C_RED, C_RED, /*C_ORANGE,*/ C_YELLOW, C_DARKGREEN, C_BLUE };

	float X = 0;
	const int32 Subsize = 10;
	ShieldGradient.Reserve(Subsize * array_size(Colors));

	for(int32 i = 0; i < array_size(Colors) - 1; i++)
	{
		for(int32 Xlocal = 0; Xlocal < Subsize; Xlocal++)
		{
			ShieldGradient.Add(FLinearColor::LerpUsingHSV/*MakeBlendedColor*/(Colors[i], Colors[i + 1], Xlocal / (Subsize - 1.0f)));
		}
	}

	
	// Scale shield readout progress bar's border width to match display DPI
	const float BorderWidth = 0.1f * GetPaintSpaceGeometry().Scale;
	auto Style = ShieldReadout->GetWidgetStyle();
	Style.BackgroundImage.Margin = FMargin(BorderWidth);
	ShieldReadout->SetWidgetStyle(Style);



	// Subscribe to player ship shield strength
	{
		Defcon::FMessageConsumer MessageConsumer(this, Defcon::EMessageEx::ShieldStrengthChanged, 
			[This = TWeakObjectPtr<UDefconPlayStatsWidgetBase>(this)](void* Payload)
			{
				if(!This.IsValid())
				{
					return;
				}

				check(Payload != nullptr);

				const auto& StrengthInfo = *static_cast<Defcon::FShieldStrengthInfo*>(Payload);

				check(StrengthInfo.Object != nullptr && StrengthInfo.Value >= 0.0f);

				if(StrengthInfo.Object->GetType() != Defcon::EObjType::PLAYER)
				{
					return;
				}

				// todo: for strengths above 1.0, blend towards white color.
				const auto Val = FMath::Min(StrengthInfo.Value, 1.0f);

				auto Readout = This.Get()->ShieldReadout;

				Readout->SetPercent(StrengthInfo.Value);

				const auto& Color = This->ShieldGradient[ROUND(Val * This->ShieldGradient.Num() - 1)];
				auto Style = Readout->GetWidgetStyle();
				Style.BackgroundImage.TintColor = FSlateColor(Color);
				Readout->SetWidgetStyle(Style);
				Readout->SetFillColorAndOpacity(Color);
				This->ShieldLabel->SetColorAndOpacity(Color);
			}
		);
		Defcon::GMessageMediator.RegisterConsumer(MessageConsumer);
	}


	// Subscribe to human states
	{
		Defcon::FMessageConsumer MessageConsumer(this, Defcon::EMessageEx::AbductionCountChanged, 
			[This = TWeakObjectPtr<UDefconPlayStatsWidgetBase>(this)](void* Payload)
			{
				if(!This.IsValid())
				{
					return;
				}

				check(Payload != nullptr);

				const TArray<bool>& AbductionStates = *static_cast<TArray<bool>*>(Payload);

				This->FlashHumansLabel = AbductionStates.Contains(true);

				if(This->FlashHumansLabel == false)
				{
					This->HumansLabel->SetRenderOpacity(1.0f);
				}
			}
		);
		Defcon::GMessageMediator.RegisterConsumer(MessageConsumer);
	}

}


void UDefconPlayStatsWidgetBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if(FlashHumansLabel)
	{
		HumansLabel->SetRenderOpacity(PSIN(Age * TWO_PI * 2));
	}
}


int32 UDefconPlayStatsWidgetBase::NativePaint
(
	const FPaintArgs&          Args,
	const FGeometry&           AllottedGeometry,
	const FSlateRect&          MyCullingRect,
	FSlateWindowElementList&   OutDrawElements,
	int32                      LayerId,
	const FWidgetStyle&        InWidgetStyle,
	bool                       bParentEnabled
) const
{
	return Super::NativePaint(
		Args,
		AllottedGeometry,
		MyCullingRect,
		OutDrawElements,
		LayerId,
		InWidgetStyle,
		bParentEnabled);
}

