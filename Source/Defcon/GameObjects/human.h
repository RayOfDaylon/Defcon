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

			virtual void InitHuman     (const CFPoint& pt) { Position = pt; m_pCarrier = nullptr; Age = 0.0f; }

			virtual void Move          (float);
			virtual void Draw          (FPaintArguments&, const I2DCoordMapper&);
			IGameObject* GetCarrier    () const { return m_pCarrier; }
			bool         IsBeingCarried() const { return (this->GetCarrier() != nullptr); }
			bool         IsFalling     () const;
			bool         IsOnGround    () const { return !(this->IsFalling() || this->IsBeingCarried()); }
			void         SetToNotCarried () { m_pCarrier = nullptr; }

			void         Notify        (EMessage, void*);
			bool         Fireballs     () const { return false; }

			CGameObjectCollection*	Objects;
			CGameObjectCollection*	Objects2;

			virtual void  OnAboutToDie          ();
			virtual int   GetExplosionColorBase () const;
			virtual float GetExplosionMass      () const;

		private:
			float			m_fSwitchTime;
			float           m_fSwitchWalkDirectionTime;
			float           WalkingSpeed;
			CFPoint         Motion;
			IGameObject*	m_pCarrier;
	}; // CHuman
}

