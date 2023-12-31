// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DaylonMessageMediator.h"
#include "_sound.h"


namespace Defcon
{
	// Defcon-specific message mediator.

	struct FShieldStrengthInfo 
	{
		class IGameObject* Object; 
		float              Value; 

		bool operator == (const FShieldStrengthInfo& Rhs) const { return (Object == Rhs.Object && Value == Rhs.Value); }
		bool operator != (const FShieldStrengthInfo& Rhs) const { return !(*this == Rhs); }
	};

	/*
	struct FTakenAboardInfo
	{
		ILiveGameObject* Carrier;
		CHuman*          Human;
	};*/

	/*struct FPodIntersectionInfo
	{
		float Time;
	};*/


	enum class EMessageEx
	{
		// Message                Payload type

		Unknown = 0,                // nullptr
		PlaySound,                  // EAudioTrack*
		AbductionCountChanged,      // TArray<bool>*
		SmartbombCountChanged,      // int32*
		ShieldStrengthChanged,      // FShieldStrengthInfo*
		SetTopMessage,              // FText*
		NormalMessage,              // FNormalMessage*
		ClearNormalMessages,        // nullptr
		DoubleGunsLevelChanged,     // float*
		DoubleGunsActivated,        // bool*
		InvincibilityLevelChanged,  // float*
		InvincibilityActivated,     // float*
		//PodIntersectionStarted    // FPodIntersectionInfo*
		//TakenAboard,              // FTakenAboardInfo*  Carrier has taken human aboard
		//CarrierKilled,            // ILiveGameObject*   Carrier has been destroyed
		//Released,                 // nullptr    Player ship has released human
		//HumanKilled               // CHuman*   Human is about to die
		//HumanTakenAboard          // CHuman*   Human has been taken aboard (redundant, use TakenAboard)
		Count
	};


	typedef Daylon::FMessageConsumer<EMessageEx> FMessageConsumer;


	enum class EDisplayMessage
	{
		None,
		GodModeChanged,
		XpChanged,
		BboxModeChanged,
		HumanBeingAbducted,
		HumansRemainingChanged,
		BulletTimeChanged,
		DualCannonsChanged,
		DualCannonsLevelChanged,
		InvincibilityChanged,
		InvincibilityLevelChanged,
		ShieldLevelChanged,
		CurrentDebugEnemyChanged,
		SmartbombOrdnanceCountChanged,
		TargetsRemainingChanged
	};


	struct FPlaySoundMessage
	{
		EAudioTrack Track;
		float       Volume = 1.0f;
	};


	struct FNormalMessage
	{
		FString         Text;
		float           Duration;
		EDisplayMessage Type;
	};


	class CMessageMediator : public Daylon::TFastMessageMediator<EMessageEx> 
	{
		CMessageMediator (const CMessageMediator&) = delete;
		CMessageMediator& operator= (const CMessageMediator&) = delete;

		public:
			
			CMessageMediator() {}

			// Convenience message encoders/abstractors.

			void PlaySound(EAudioTrack Track, float Volume = 1.0f)
			{
				FPlaySoundMessage Msg = { Track, Volume };
				Send(EMessageEx::PlaySound, &Msg);
			}

			void TellUser(const FString& Str, float Duration = 0.0f, EDisplayMessage Kind = EDisplayMessage::None) const
			{
				FNormalMessage Msg = { Str, Duration, Kind };
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
