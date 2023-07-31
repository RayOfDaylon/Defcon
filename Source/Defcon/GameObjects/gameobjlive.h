// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// gameobjlive.h


#include "Common/PaintArguments.h"
#include "Common/util_geom.h"
#include "DaylonUtils.h"

#include "gameobj.h"



struct FNavControl
{
	float	TimeDown;
	bool	bActive;
};


// Return the shortest position delta from p1 to p2 given a modulus range aw.
inline void PositionDelta(CFPoint& Result, const CFPoint& P1, const CFPoint& P2, float ArenaWidth)
{
	Result = P2;
	Result.Sub(P1);

	const float D1    = ABS(Result.x);
	const float Tail1 = ArenaWidth - P1.x;
	const float Tail2 = ArenaWidth - P2.x;
	const float D2    = FMath::Min(P1.x + Tail2, P2.x + Tail1);

	if(D2 < D1)
	{
		// The shortest distance lies across the origin.
		Result.x = D2;

		if(P2.x > P1.x)
		{
			Result.x *= -1;
		}
	}
}


namespace Defcon
{
	class ILiveGameObject : public IGameObject
	{
		// A live game object is a game entity that 
		// has shields, weapons, and is autonomously moveable.
		// Normally when an object dies it is deleted, but 
		// in cases where an object remains in memory (like the player ship),
		// the bAlive flag can be used.

		typedef IGameObject Super;

		public:
			ILiveGameObject();

#ifdef _DEBUG
			virtual const char* GetClassname() const = 0;
#endif
			virtual void Notify                  (EMessage, void*) override;
			virtual void Move                    (float DeltaTime) override;
			virtual void Draw                    (FPaintArguments&, const I2DCoordMapper&) override = 0;

			virtual bool Fireballs               () const { return true; }

			virtual void   ChangeThrust          (const CFPoint&);
			void           EnableInput           (bool b = true) { bCanMove = b; }
			bool           IsInputEnabled        () const { return bCanMove; }
			bool           IsAlive               () const { return bAlive; }
			virtual void   SetIsAlive            (bool b) { bAlive = b; }


			void           ZeroThrust            ()   { ThrustVector.Set(0,0); }
			void           ZeroMotion            ()   { this->ZeroVelocity(); this->ZeroThrust(); }
			void           ZeroInput             ()    { for(auto& Ctl : NavControls) { Ctl.bActive = false; } }

			virtual void   ComputeThrustTimings  (float);
			virtual void   ComputeForces         (float);
			virtual void   ImpartForces          (float);

			const CFPoint& GetThrustVector       () const { return ThrustVector; }

			virtual float  NavControl_Duration   (int) const;

			void           SetNavControl         (int, bool, float);

			virtual void   OnAboutToDie          ();

			virtual float  GetShieldStrength     () const    { return ShieldStrength; }
			virtual void   SetShieldStrength     (float f);
			virtual bool   RegisterImpact        (float f);
			void           BindToShieldValue     (TFunction<void(const float& Val)> Delegate) { ShieldStrength.Bind(Delegate); }

			enum { ctlUp, ctlDown, ctlFwd, ctlBack, numCtls };


		protected:

			bool        bCanMove;
			CFPoint     ThrustVector;
			float       MaxThrust;

			float       ThrustDurationVertical;
			float       ThrustDurationForwards;
			float       ThrustDurationBackwards;

			FNavControl NavControls[numCtls];

		private:
			Daylon::TBindableValue<float> ShieldStrength; // 0..1 value
			bool                          bAlive = true;
	};
}
