// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// starfield.h


#include "Common/util_core.h"
#include "Common/PaintArguments.h"
#include "Common/util_geom.h"

#include "GameObjects/gameobjlive.h"

// todo: drop this

#if 0
namespace Defcon
{
	typedef struct
	{
		CFVector	m_pos;
		FLinearColor	m_colorFar;
		FLinearColor	m_colorNear;
	} star;


	class CStarfield : public ILiveGameObject
	{
		public:
			CStarfield();
			//virtual ~CStarfield() {}

			void InitStarfield(size_t, size_t, float fRange = 7.0f, bool bMorphColor = false, FLinearColor nearColor = C_BLACK);

#ifdef _DEBUG
			virtual const char* GetClassname() const override { return "Starfield"; }
#endif

			virtual void Move		(float) override;
			virtual void Draw		(FPaintArguments&, const I2DCoordMapper&) override;
			virtual void DrawSmall	(FPaintArguments&, const I2DCoordMapper&, FSlateBrush&) override {}

			void	SetSpeedScale			(float f) { m_fSpeedMul = f; }
			void	SetRotationSpeedScale	(float f) { m_fSpeedRotMul = f; }
			void	SetNumStars				(float f) { m_count = (size_t)FMath::Min(f * array_size(m_stars), array_size(m_stars)); }
			size_t	GetNumStars				() const { return m_count; }
			void	SetFat					(bool b = true) { m_bFat = b; }

		private:
			bool		m_bFat			= false;
			bool		m_bMorphColor	= false;
			size_t		m_count;
			float		m_fSpeedMul		= 1.0f;
			float		m_fSpeedRotMul	= 1.0f;

			float		m_fAngle		= 0.0f;
			CFVector	m_starpos[2000];
			size_t		m_w				= 0;
			size_t		m_h				= 0;
			star		m_stars[4000];


	}; // CStarfield

}


#endif // 0
