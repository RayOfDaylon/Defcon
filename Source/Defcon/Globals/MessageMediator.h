// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DaylonUtils.h"



namespace Defcon
{
	// Defcon-specific message mediator.

	struct FShieldStrengthInfo 
	{
		class ILiveGameObject* Object; 
		float                  Value; 

		bool operator == (const FShieldStrengthInfo& Rhs) const { return (Object == Rhs.Object && Value == Rhs.Value); }
		bool operator != (const FShieldStrengthInfo& Rhs) const { return !(*this == Rhs); }
	};

	/*
	struct FTakenAboardInfo
	{
		ILiveGameObject* Carrier;
		CHuman*          Human;
	};*/

	struct FNormalMessage
	{
		FString Text;
		float   Duration;
	};

	/*struct FPodIntersectionInfo
	{
		float Time;
	};*/


	enum class EMessageEx
	{
		// Message                Payload type

		Unknown = 0,             // nullptr
		AbductionCountChanged,   // TArray<bool>*
		SmartbombCountChanged,   // int32*
		ShieldStrengthChanged,   // FShieldStrengthInfo*
		SetTopMessage,           // FText*
		NormalMessage,           // FNormalMessage*
		ClearNormalMessages,     // nullptr
		//PodIntersectionStarted   // FPodIntersectionInfo*
		//TakenAboard,           // FTakenAboardInfo*  Carrier has taken human aboard
		//CarrierKilled,         // ILiveGameObject*   Carrier has been destroyed
		//Released,              // nullptr    Player ship has released human
		//HumanKilled            // CHuman*   Human is about to die
		//HumanTakenAboard       // CHuman*   Human has been taken aboard (redundant, use TakenAboard)
		Count
	};


	typedef Daylon::FMessageConsumer<EMessageEx> FMessageConsumer;


	class CMessageMediator : public Daylon::TFastMessageMediator<EMessageEx> 
	{
		CMessageMediator (const CMessageMediator&) = delete;
		CMessageMediator& operator= (const CMessageMediator&) = delete;

		public:
			
			CMessageMediator() {}

			// Convenience message encoders/abstractors.

			void TellUser(const FString& Str, float Duration = 0.0f) const
			{
				FNormalMessage Msg = { Str, Duration };
				Send(EMessageEx::NormalMessage, &Msg);
			}
	};


	extern CMessageMediator GMessageMediator;


	template <typename Tval> class TMessageableValue : public Daylon::TMessageableValue<Tval, CMessageMediator, EMessageEx>
	{
		typedef Daylon::TMessageableValue<Tval, CMessageMediator, EMessageEx> Super;

		public:

			void Bind(EMessageEx InMessage)
			{
				Super::Bind(&GMessageMediator, InMessage);
			}
	};
}
