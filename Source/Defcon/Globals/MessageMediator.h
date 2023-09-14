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
		float Value; 

		bool operator == (const FShieldStrengthInfo& Rhs) const { return (Object == Rhs.Object && Value == Rhs.Value); }
		bool operator != (const FShieldStrengthInfo& Rhs) const { return !(*this == Rhs); }
	};

	/*
	struct FTakenAboardInfo
	{
		ILiveGameObject* Carrier;
		CHuman*          Human;
	};*/


	enum class EMessageEx
	{
		// Message                Payload type

		Unknown = 0,            // nullptr
		AbductionCountChanged,  // TArray<bool>*
		SmartbombCountChanged,  // int32*
		ShieldStrengthChanged,   // FShieldStrengthInfo*
		TakenAboard,            // FTakenAboardInfo*  Carrier has taken human aboard
		CarrierKilled,          // ILiveGameObject*   Carrier has been destroyed
		Released,               // nullptr    Player ship has released human
		HumanKilled            // CHuman*   Human is about to die
		//HumanTakenAboard        // CHuman*   Human has been taken aboard (redundant, use TakenAboard)
	};


	typedef Daylon::FMessageConsumer<EMessageEx> FMessageConsumer;


	class CMessageMediator : public Daylon::TMessageMediator<EMessageEx> 
	{
		CMessageMediator (const CMessageMediator&) = delete;
		CMessageMediator& operator= (const CMessageMediator&) = delete;

		public:
			
			CMessageMediator() {}
	};


	extern CMessageMediator GMessageMediator;


	template <typename Tval> class TMessageableValue : public Daylon::TMessageableValue<Tval, EMessageEx>
	{
		public:

			void Bind(EMessageEx InMessage)
			{
				Daylon::TMessageableValue<Tval, EMessageEx>::Bind(&GMessageMediator, InMessage);
			}
	};
}
