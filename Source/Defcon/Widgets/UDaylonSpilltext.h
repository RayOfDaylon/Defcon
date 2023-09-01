// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UMG/Public/Components/Widget.h"
#include "UMG/Public/Components/RichTextBlock.h"
#include "UDaylonSpilltext.generated.h"

/* 
	UDaylonSpilltext - displays text that appears one character at a time
	with a block cursor at the leading edge.
*/
UCLASS(meta=(DisplayName="Spilltext Widget"))
class DEFCON_API UDaylonSpilltext : public URichTextBlock
{
	GENERATED_BODY()

	public:

		// The text to spill
		UPROPERTY(EditAnywhere, meta=( MultiLine="true" ))
		FText SourceText;

		// How quickly to spill the text
		UPROPERTY(EditAnywhere)
		int32 CharsPerSecond;

		// Inline style text img id argument
		UPROPERTY(EditAnywhere)
		FString ImageID;

		// Must be called before ticking.
		UFUNCTION(BlueprintCallable, Category="DaylonSpilltext")
		void Reset();

		UFUNCTION(BlueprintCallable, Category="DaylonSpilltext")
		void Tick(float DeltaTime);

		bool IsFinished() const;

		void Finish();

#if WITH_EDITOR
		virtual const FText GetPaletteCategory() override;
#endif

	protected:

		FString  InlineCode;
		float    Age             = 0.0f;
		float    SecondsPerChar  = 0.0f;
		int32    SourceTextLen   = 0;
		int32    NumCharsSpilled = 0;
		bool     IsSpilling      = false;
};

