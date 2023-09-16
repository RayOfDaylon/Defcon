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

			virtual void   Tick                    (float DeltaTime) override;
			virtual void   Notify                  (EMessage, void*) override;
			virtual bool   ExplosionHasFireball    () const override { return false; }

			virtual void   OnAboutToDie            () override;
			virtual EColor GetExplosionColorBase   () const override;
			virtual float  GetExplosionMass        () const override;
			virtual bool   CanBeInjuredBySmartbomb () const override { return false; }


			void           InitHuman               (CGameObjectCollection* Objs1, CGameObjectCollection* Objs2) { Carrier = nullptr; Age = 0.0f; Objects = Objs1; Objects2 = Objs2; }
			IGameObject*   GetCarrier              () const { return Carrier; }
			bool           IsBeingCarried          () const { return (GetCarrier() != nullptr); }
			bool           IsBeingAbducted         () const { return (IsBeingCarried() && GetCarrier()->GetType() != EObjType::PLAYER); }
			bool           IsFalling               () const;
			bool           IsOnGround              () const { return !(IsFalling() || IsBeingCarried()); }
			void           SetToNotCarried         () { Carrier = nullptr; }
			void           ShowGratitude           () const;


		private:

			CFPoint                 Motion;
			CGameObjectCollection*	Objects   = nullptr;
			CGameObjectCollection*	Objects2  = nullptr;
			IGameObject*	        Carrier   = nullptr;
			float			        SwitchFacingDirectionCountdown;
			float                   SwitchWalkingDirectionCountdown;
			float                   WalkingSpeed;
	};
}

