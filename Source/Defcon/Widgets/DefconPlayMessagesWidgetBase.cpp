// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


#include "DefconPlayMessagesWidgetBase.h"
#include "Globals/MessageMediator.h"
#include "Globals/prefs.h"
#include "UMG/Public/Components/VerticalBoxSlot.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



void UDefconPlayMessagesWidgetBase::NativeOnInitialized()
{
	{
		Defcon::FMessageConsumer Consumer(this, Defcon::EMessageEx::NormalMessage, 
			[This = TWeakObjectPtr<UDefconPlayMessagesWidgetBase>(this)](void* Payload)
			{
				check(Payload != nullptr);

				if(This.IsValid())
				{
					const auto& Msg = *(Defcon::FNormalMessage*)Payload;
					This->AddMessage(Msg.Text, Msg.Duration); 
				} 
			});

		Defcon::GMessageMediator.RegisterConsumer(Consumer);
	}

	{
		Defcon::FMessageConsumer Consumer(this, Defcon::EMessageEx::ClearNormalMessages, 
			[This = TWeakObjectPtr<UDefconPlayMessagesWidgetBase>(this)](void* Payload)
			{
				if(This.IsValid())
				{
					This->Clear();
				} 
			});

		Defcon::GMessageMediator.RegisterConsumer(Consumer);
	}
}


void UDefconPlayMessagesWidgetBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	if(MessagesBeingShown.IsEmpty() || IsPaused())
	{
		return;
	}

	Age += DeltaTime;

	if(Age < MessagesBeingShown[0].Duration)
	{
		return;
	}

	Age = 0.0f;

	FString Str = Messages->GetText().ToString();


	// Remove first line of text and any following empty lines.

	while(true)
	{
		const int32 EolIndex = Str.Find(TEXT("\n"), ESearchCase::CaseSensitive, ESearchDir::FromStart, 0);

		if(EolIndex == INDEX_NONE)
		{
			// Last line has been shown, so empty everything.
			Str.Empty();
			MessagesBeingShown.Empty();
			break;
		}
		else
		{
			// Still some more lines.
			Str.RightChopInline(EolIndex + 1);
			MessagesBeingShown.RemoveAt(0);

			// If the new first line isn't a blank line, then we're done.
			if(!Str.StartsWith(TEXT("\n")))
			{
				break;
			}
		}
	}

	Messages->SetText(FText::FromString(Str));
}


void UDefconPlayMessagesWidgetBase::AddMessage(const FString& StrToAdd, float Duration)
{
	if(Duration == 0.0f)
	{
		Duration = MESSAGE_DURATION;
	}


	// EOL characters not allowed in message strings.
	check(INDEX_NONE == StrToAdd.Find(TEXT("\n"), ESearchCase::CaseSensitive, ESearchDir::FromStart, 0));

	MessagesBeingShown.Add(FGameMessage(StrToAdd, Duration));

	FString Str = Messages->GetText().ToString();

	// If this is the only line of text that will be in the widget, reset the age counter
	// so it gets shown for the message's duration.

	if(Str.IsEmpty())
	{
		Age = 0.0f;
	}
	else
	{
		Str += TEXT("\n");
	}

	Str += StrToAdd.Replace(TEXT("--"), TEXT("\x2014"), ESearchCase::CaseSensitive);

	Messages->SetText(FText::FromString(Str));
}



void UDefconPlayMessagesWidgetBase::Clear()
{
	MessagesBeingShown.Empty();
	Messages->SetText(FText::FromString(TEXT("")));
}

// ----------------------------------------------------------------------------------------------------

bool UDefconDisplayMessagesBase::IsValid() const
{
	return (MessagesWidget->GetChildrenCount() == Messages.Num());
}


void UDefconDisplayMessagesBase::Clear()
{
	MessagesWidget->ClearChildren();
	Messages.Empty();
	Messages.Reserve(20);
}


void UDefconDisplayMessagesBase::RemoveTopmostMessage()
{
	MessagesWidget->RemoveChildAt(0);
	Messages.RemoveAt(0);
	check(IsValid());
}


void UDefconDisplayMessagesBase::NativeOnInitialized()
{
	Clear();

	{
		Defcon::FMessageConsumer Consumer(this, Defcon::EMessageEx::NormalMessage, 
			[This = TWeakObjectPtr<UDefconDisplayMessagesBase>(this)](void* Payload)
			{
				check(Payload != nullptr);

				if(This.IsValid())
				{
					const auto& Msg = *(Defcon::FNormalMessage*)Payload;
					This->AddMessage(Msg.Text, Msg.Duration, Msg.Type); 
				} 
			});

		Defcon::GMessageMediator.RegisterConsumer(Consumer);
	}

	{
		Defcon::FMessageConsumer Consumer(this, Defcon::EMessageEx::ClearNormalMessages, 
			[This = TWeakObjectPtr<UDefconDisplayMessagesBase>(this)](void* Payload)
			{
				if(This.IsValid())
				{
					This->Clear();
				} 
			});

		Defcon::GMessageMediator.RegisterConsumer(Consumer);
	}
}


void UDefconDisplayMessagesBase::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	if(IsPaused())
	{
		return;
	}

	check(IsValid());

	while(!Messages.IsEmpty() && Cast<UTextBlock>(MessagesWidget->GetChildAt(0))->GetText().IsEmptyOrWhitespace())
	{
		RemoveTopmostMessage();
	}

	if(Messages.IsEmpty())
	{
		return;
	}

	Messages[0].Duration -= DeltaTime;

	if(Messages[0].Duration > 0.0f)
	{
		return;
	}

	RemoveTopmostMessage();
}


void UDefconDisplayMessagesBase::AddMessage(const FString& Str, float Duration, Defcon::EDisplayMessage MessageKind)
{
	check(IsValid());

	// Strip out any existing messages that match our kind.

	if(MessageKind != Defcon::EDisplayMessage::None)
	{
		for(int32 Index = Messages.Num() - 1; Index >= 0; Index--)
		{
			if(Messages[Index].Kind == MessageKind)
			{
				Messages.RemoveAt(Index);
				MessagesWidget->RemoveChildAt(Index);
			}
		}

		check(IsValid());
	}

	// Add message.

	if(Duration == 0.0f)
	{
		Duration = MESSAGE_DURATION;
	}

	const FDisplayMessage Message = { Duration, MessageKind };

	Messages.Add(Message);

	auto TextBlock = Daylon::MakeWidget<UTextBlock>();

	auto TheSlot = MessagesWidget->AddChildToVerticalBox(TextBlock);

	TheSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);

	TextBlock->SetFont             (Font);
	TextBlock->SetColorAndOpacity  (Color);
	
	const FString StrFinal = Str.Replace(TEXT("--"), TEXT("\x2014"), ESearchCase::CaseSensitive);

	TextBlock->SetText(FText::FromString(StrFinal)); 

	check(IsValid());
}



#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
