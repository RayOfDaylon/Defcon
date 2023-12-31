// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// starfield.h


#include "Common/util_core.h"
#include "Common/Painter.h"
#include "Common/util_geom.h"

#include "GameObjects/gameobjlive.h"

// todo: drop this

#if 0
namespace Defcon
{
	typedef struct
	{
		CFVector	Position;
		FLinearColor	m_colorFar;
		FLinearColor	m_colorNear;
	} star;


	class CStarfield : public ILiveGameObject
	{
		public:
			CStarfield();
			//virtual ~CStarfield() {}

			void InitStarfield(int32, int32, float fRange = 7.0f, bool bMorphColor = false, FLinearColor nearColor = C_BLACK);

			virtual void Tick		(float DeltaTime) override;
			virtual void Draw		(FPaintArguments&, const I2DCoordMapper&) override;
			virtual void DrawSmall	(FPaintArguments&, const I2DCoordMapper&, FSlateBrush&) const override {}

			void	SetSpeedScale			(float f) { m_fSpeedMul = f; }
			void	SetRotationSpeedScale	(float f) { m_fSpeedRotMul = f; }
			void	SetNumStars				(float f) { m_count = (int32)FMath::Min(f * array_size(m_stars), array_size(m_stars)); }
			int32	GetNumStars				() const { return m_count; }
			void	SetFat					(bool b = true) { m_bFat = b; }

		private:
			bool		m_bFat			= false;
			bool		m_bMorphColor	= false;
			int32		m_count;
			float		m_fSpeedMul		= 1.0f;
			float		m_fSpeedRotMul	= 1.0f;

			float		m_fAngle		= 0.0f;
			CFVector	m_starpos[2000];
			int32		m_w				= 0;
			int32		m_h				= 0;
			star		m_stars[4000];


	}; // CStarfield

}


#endif // 0
