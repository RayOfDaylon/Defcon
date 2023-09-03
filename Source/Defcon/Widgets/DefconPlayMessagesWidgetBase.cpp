// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


#include "DefconPlayMessagesWidgetBase.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif




constexpr float MessageLifetime = 1.5f;


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
		Duration = MessageLifetime;
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


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
