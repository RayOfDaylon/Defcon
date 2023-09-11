// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// gameobjlive.h


#include "Common/Painter.h"
#include "Common/util_geom.h"
#include "Globals/MessageMediator.h"
#include "DaylonUtils.h"

#include "gameobj.h"



struct FNavControl
{
	float   Duration = 0.0f;
	bool	bActive  = false;
};


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

			virtual void       Tick                  (float DeltaTime) override;

			virtual bool       ExplosionHasFireball  () const override { return true; }

			virtual void       ChangeThrust          (const CFPoint&);
			void               EnableInput           (bool b = true) { bCanMove = b; }
			bool               IsInputEnabled        () const { return bCanMove; }
			bool               IsAlive               () const { return bAlive; }
			virtual void       SetIsAlive            (bool b) { bAlive = b; }

			const FNavControl& GetNavControl         (int32 Index) const { return NavControls[Index]; }
			void               SetNavControl         (int32 Index, bool bActive, float Duration);
			float              NavControlDuration    (int32 Index) const;
			void               ZeroThrust            () { ThrustVector.Set(0,0); }
			void               ZeroMotion            () { ZeroVelocity(); ZeroThrust(); }
			void               ZeroInput             () { for(auto& Ctl : NavControls) { Ctl.bActive = false; } }
			virtual void       ComputeThrustTimings  (float);
			virtual void       ComputeForces         (float);
			virtual void       ImpartForces          (float);
			const CFPoint&     GetThrustVector       () const { return ThrustVector; }

			virtual float      GetShieldStrength     () const    { return ShieldStrength.Get().Value; }
			virtual void       SetShieldStrength     (float f);
			virtual bool       RegisterImpact        (float f);
			//void               BindToShieldValue     (TFunction<void(const float& Val)> Delegate) { ShieldStrength.Bind(Delegate); }

			//bool               HasPassenger          () const { return bHasPassenger; }

			enum ENavControl { Up, Down, Fwd, Back, Count };


		protected:

			FNavControl NavControls[ENavControl::Count];
			CFPoint     ThrustVector;
			float       MaxThrust;
			float       ThrustDurationVertical;
			float       ThrustDurationForwards;
			float       ThrustDurationBackwards;
			bool        bCanMove;
			//bool        bHasPassenger = false;


		private:
			//Daylon::TBindableValue<float> ShieldStrength; // 0..1 value
			TBroadcastableValue<FShieldStrengthInfo>    ShieldStrength;
			bool                                        bAlive = true;
	};


	// Return the shortest position delta from p1 to p2 given a modulus range ArenaWidth.
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
}

