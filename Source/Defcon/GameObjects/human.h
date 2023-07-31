// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// humans.h


#include "Common/PaintArguments.h"

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


#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif

			virtual void   InitHuman              (const CFPoint& pt) { Position = pt; Carrier = nullptr; Age = 0.0f; }
						   				          
			virtual void   Move                   (float);
			virtual void   Draw                   (FPaintArguments&, const I2DCoordMapper&);
			IGameObject*   GetCarrier             () const { return Carrier; }
			bool           IsBeingCarried         () const { return (this->GetCarrier() != nullptr); }
			bool           IsFalling              () const;
			bool           IsOnGround             () const { return !(this->IsFalling() || this->IsBeingCarried()); }
			void           SetToNotCarried        () { Carrier = nullptr; }
						   
			void           Notify                 (EMessage, void*);
			bool           Fireballs              () const { return false; }

			virtual void   OnAboutToDie           ();
			virtual EColor GetExplosionColorBase  () const override;
			virtual float  GetExplosionMass       () const;

			CGameObjectCollection*	Objects;
			CGameObjectCollection*	Objects2;


		private:
			float			SwitchFacingDirectionCountdown;
			float           SwitchWalkingDirectionCountdown;
			float           WalkingSpeed;
			CFPoint         Motion;
			IGameObject*	Carrier = nullptr;
	};
}

