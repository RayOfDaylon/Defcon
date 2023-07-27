// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "DefconPlayWidgetBase.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
//#include "Runtime/SlateCore/Public/Brushes/SlateColorBrush.h"
#include "DefconPlayMessagesWidgetBase.generated.h"


struct FGameMessage
{
	//FString Text; // don't really need this
	float   Duration;

	FGameMessage(const FString& MsgText, float InDuration)
	: /*Text(MsgText),*/ Duration(InDuration)
	{
	}
};

/*
	A small widget at the centered top of the main widget that shows text to the user.
	It uses multiple lines in case messages pile up. As time passes, the oldest 
	message (topmost) disappears, pulling any newer messages up.
*/
UCLASS()
class DEFCON_API UDefconPlayMessagesWidgetBase : public UDefconUserWidgetBase
{
	GENERATED_BODY()

	protected:

	//virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;


	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> Messages;

	float Age = 0.0f;

	TArray<FGameMessage>  MessagesBeingShown;

	public:

		void AddMessage (const FString& Str, float Duration = 0.0f);
		void Clear      ();
};


