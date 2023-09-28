// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "DefconPlayWidgetBase.h"
#include "Globals/MessageMediator.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Components/VerticalBox.h"

//#include "Runtime/SlateCore/Public/Brushes/SlateColorBrush.h"
#include "DefconPlayMessagesWidgetBase.generated.h"


// todo: remove
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
	todo: remove
*/
UCLASS()
class DEFCON_API UDefconPlayMessagesWidgetBase : public UDefconUserWidgetBase
{
	GENERATED_BODY()

	protected:

	virtual void NativeOnInitialized() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UTextBlock> Messages;

	float Age = 0.0f;

	TArray<FGameMessage>  MessagesBeingShown;

	public:

		void AddMessage (const FString& Str, float Duration = 0.0f);
		void Clear      ();

		TFunction<bool()> IsPaused = [](){ return false; };
};


// ----------------------------------------------------------------------------

struct FDisplayMessage
{
	float                   Duration = 0.0f;
	Defcon::EDisplayMessage Kind     = Defcon::EDisplayMessage::None;
};


// A better version of UDefconPlayMessagesWidgetBase
// todo: we could use SWidget::AddMetaData to track which FDisplayMessage goes with which text block
UCLASS()
class DEFCON_API UDefconDisplayMessagesBase : public UDefconUserWidgetBase
{
	GENERATED_BODY()

	protected:

		virtual void NativeOnInitialized() override;

		virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

		bool IsValid              () const;
		void RemoveTopmostMessage ();


		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSlateFontInfo Font;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSlateColor Color;

		UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
		TObjectPtr<UVerticalBox> MessagesWidget;


		TArray<FDisplayMessage>  Messages; // must correspond to children in MessagesWidget.


	public:

		void AddMessage (const FString& Str, float Duration = 0.0f, Defcon::EDisplayMessage DisplayMessage = Defcon::EDisplayMessage::None);
		void Clear      ();

		TFunction<bool()> IsPaused = [](){ return false; };
};
