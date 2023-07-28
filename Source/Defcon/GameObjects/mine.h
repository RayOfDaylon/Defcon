// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


#include "Common/PaintArguments.h"
#include "Common/util_geom.h"

#include "GameObjects/gameobjlive.h"
#include "GameObjects/obj_types.h"
#include "GameObjects/weapon.h"



namespace Defcon
{
	class CMine : public IGameObject
	{
		// Laid by bombers.

		public:
			CMine();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			//void Create(const CFPoint&, const Orient2D&);
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual void DrawSmall(FPaintArguments&, const I2DCoordMapper&, FSlateBrush&) override {}
			virtual void DrawBbox(FPaintArguments&, const I2DCoordMapper&);

			virtual void GetInjurePt(CFPoint&) const;
			virtual bool TestInjury(const CFRect&) const;

			virtual float GetCollisionForce() const
				{
					return 0.01f * MINE_DAMAGE; 
				}


		private:
			float	     m_maxAge;
			FLinearColor m_color;

	}; // CMine
}
