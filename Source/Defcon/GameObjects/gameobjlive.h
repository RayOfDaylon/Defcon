#pragma once

// gameobjlive.h


#include "Common/PaintArguments.h"
#include "Common/util_geom.h"
#include "DaylonUtils.h"

#include "gameobj.h"


typedef float (*TerrainElevFunc)(float, void*);
typedef Defcon::IGameObject* (*HumanFinderFunc)(float, void*);

typedef struct
{
	float	fTimeDown;
	bool	bActive;
} NavControl;

// Return the shortest position delta from p1 to p2 given a modulus range aw.
inline void pos_delta
(
	CFPoint& result, 
	const CFPoint& p1, 
	const CFPoint& p2, 
	float aw
)
{
	result = p2;
	result.sub(p1);

	float d1 = ABS(result.x);
	float tail1 = aw - p1.x;
	float tail2 = aw - p2.x;

	float d2 = FMath::Min(p1.x + tail2, p2.x + tail1);

	if(d2 < d1)
	{
		// The shortest distance lies across the origin.
		result.x = d2;
		if(p2.x > p1.x)
			result.x *= -1;
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
		// the m_bAlive flag can be used.

		typedef IGameObject Super;

		public:
			ILiveGameObject();

#ifdef _DEBUG
			virtual const char* GetClassname() const = 0;
#endif
			virtual void Notify	   (Message, void*);
			virtual void Move	   (float);
			virtual void Draw	   (FPaintArguments&, const I2DCoordMapper&) = 0;
			//virtual void DrawSmall (FPaintArguments& FrameBuffer, const I2DCoordMapper& CoordMapper, FSlateBrush& Brush) override;


			virtual bool  Fireballs	() const { return true; }

			virtual void ChangeThrust	(const CFPoint&);
			void		 EnableInput	(bool b = true) { m_bCanMove = b; }
			bool		 IsInputEnabled	() const { return m_bCanMove; }
			bool         IsAlive        () const { return m_bAlive; }
			virtual void SetIsAlive     (bool b) { m_bAlive = b; }


			void ZeroThrust()				{ m_thrustVector.set(0,0); }
			void ZeroMotion()				{ this->ZeroVelocity(); this->ZeroThrust();	}
			void ZeroInput()
			{
				for(auto& Ctl : m_navCtls)
				{
					Ctl.bActive = false;
				}
			}

			virtual void ComputeThrustTimings	(float);
			virtual void ComputeForces			(float);
			virtual void ImpartForces			(float);

			const CFPoint& GetThrustVector() const { return m_thrustVector; }

			virtual float NavControl_Duration(int) const;

			enum { ctlUp, ctlDown, ctlFwd, ctlBack, numCtls };
			void SetNavControl(int, bool, float);

			virtual void OnAboutToDie();

			virtual float GetShieldStrength	() const	{ return m_fShields; }
			virtual void  SetShieldStrength	(float f);
			virtual bool  RegisterImpact    (float f);
			void          BindToShieldValue (TFunction<void(const float& Val)> Delegate) { m_fShields.Bind(Delegate); }




		protected:

			bool		m_bCanMove;
			CFPoint		m_thrustVector;
			float		m_maxThrust;

			float		m_fBirthDuration;

			float		m_fThrustDuration_Vertical;
			float		m_fThrustDuration_Forwards;
			float		m_fThrustDuration_Backwards;

			NavControl	m_navCtls[numCtls];

		private:
			//float		m_fShields; // 0...1 or possibly more (supershields)
			Daylon::TBindableValue<float> m_fShields; // 0..1 value
			bool                          m_bAlive = true;

	};
}
