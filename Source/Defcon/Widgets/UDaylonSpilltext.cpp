// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.


#include "UDaylonSpilltext.h"


#if WITH_EDITOR

const FText UDaylonSpilltext::GetPaletteCategory()
{
	return FText::FromString(TEXT("Daylon"));
}

#endif


void UDaylonSpilltext::Reset()
{
	InlineCode      = FString::Printf(TEXT("<img id=\"%s\"/>"), *ImageID);
	SecondsPerChar  = 1.0f / CharsPerSecond;
	Age             = 0.0f;
	SourceTextLen   = SourceText.ToString().Len();
	NumCharsSpilled = 0;
	IsSpilling      = true;
}


bool UDaylonSpilltext::IsFinished() const
{
	return (NumCharsSpilled >= SourceTextLen);
}


void UDaylonSpilltext::Finish()
{
	SetText(SourceText);
	NumCharsSpilled = SourceTextLen;
	IsSpilling = false;
}


void UDaylonSpilltext::Tick(float DeltaTime)
{
	if(!IsSpilling || SourceTextLen == 0) 
	{
		return; 
	}

	Age += DeltaTime;

	if(Age < SecondsPerChar)
	{
		return;
	}

	Age -= SecondsPerChar;

	NumCharsSpilled++;

	FString Str = SourceText.ToString().Left(NumCharsSpilled);

	if(NumCharsSpilled == SourceTextLen)
	{
		IsSpilling = false;
	}
	else
	{
		Str += InlineCode;
	}

	SetText(FText::FromString(Str));
}

