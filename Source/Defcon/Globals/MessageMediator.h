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


	enum class EMessageEx
	{
		// Message                Payload type

		Unknown = 0,            // nullptr
		AbductionCountChanged,  // TArray<bool>*
		SmartbombCountChanged,  // int32*
		ShieldStrengthChanged   // FShieldStrengthInfo*
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
