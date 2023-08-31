// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// humans.h


#include "Common/Painter.h"

#include "GameObjects/gameobj.h"
#include "GameObjects/gameobjlive.h"
#include "GameObjects/obj_types.h"



namespace Defcon
{
	class CHuman : public ILiveGameObject
	{
		public:
			CHuman();
			virtual ~CHuman();

			virtual void   Tick                   (float DeltaTime) override;
			virtual void   Notify                 (EMessage, void*) override;
			virtual bool   ExplosionHasFireball   () const override { return false; }

			virtual void   OnAboutToDie           () override;
			virtual EColor GetExplosionColorBase  () const override;
			virtual float  GetExplosionMass       () const override;

			void           InitHuman              (const CFPoint& pt) { Position = pt; Carrier = nullptr; Age = 0.0f; }
			IGameObject*   GetCarrier             () const { return Carrier; }
			bool           IsBeingCarried         () const { return (GetCarrier() != nullptr); }
			bool           IsFalling              () const;
			bool           IsOnGround             () const { return !(IsFalling() || IsBeingCarried()); }
			void           SetToNotCarried        () { Carrier = nullptr; }
			void           ShowGratitude          () const;

			CGameObjectCollection*	Objects   = nullptr;
			CGameObjectCollection*	Objects2  = nullptr;


		private:
			CFPoint         Motion;
			IGameObject*	Carrier = nullptr;
			float			SwitchFacingDirectionCountdown;
			float           SwitchWalkingDirectionCountdown;
			float           WalkingSpeed;
	};
}

