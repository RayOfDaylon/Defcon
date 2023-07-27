// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// flak.h



#include <vector>


#include "Common/PaintArguments.h"
#include "Common/util_geom.h"
#include "Common/util_color.h"

#include "GameObjects/gameobj.h"
#include "GameObjects/gameobjlive.h"

#include "Runtime/SlateCore/Public/Brushes/SlateColorBrush.h"



namespace Defcon
{
	class CFlak : public IGameObject
	{
		// A flak object is just a piece of debris used in explosions. 
		// It has a random appearance per instance.

		public:
			CFlak();
			virtual ~CFlak();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move      (float);
			virtual void Draw      (FPaintArguments&, const I2DCoordMapper&);
			virtual void DrawSmall (FPaintArguments&, const I2DCoordMapper&, FSlateBrush&);
			virtual void DrawBbox  (FPaintArguments&, const I2DCoordMapper&) {}

			virtual bool OccursFrequently () const { return true; }

			float	m_fLargestSize = 6;
			bool	m_bFade = true;

			int		m_eColorbaseOld;
			int		m_eColorbaseYoung;
			bool	m_bCold;

		protected:
			float            m_maxAge;
			//FSlateColorBrush Brush = FSlateColorBrush(C_WHITE);
			FSlateBrush* BrushPtr = nullptr;
			// todo: add brush that uses round bullet texture
	}; // CFlak


	class CGlowingFlak : public CFlak
	{
		public:
			//CGlowingFlak();
			//virtual ~CGlowingFlak();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			//virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			//virtual void DrawSmall(FPaintArguments&, const I2DCoordMapper&);
			//virtual void DrawBbox(FPaintArguments&, const I2DCoordMapper&) {}

			//virtual bool OccursFrequently() const { return true; }


	}; // CGlowingFlak


#if 0
	class CPuff : public IGameObject
	{
		// A puff object is a gray ellipse that
		// grows smaller and darker over time. It is used
		// to simulate fading smoke from an explosion. 
		// If the player is destroying a lot of targets,
		// the smoke can become pretty abdundant and
		// introduce a 'fog of war' condition.
		public:
			CPuff();
			virtual ~CPuff();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual void DrawSmall(FPaintArguments&, const I2DCoordMapper&, FSlateBrush&) override;
			virtual void DrawBbox(FPaintArguments&, const I2DCoordMapper&) {}

			virtual bool OccursFrequently() const { return true; }

			float	m_fLargestSize;
			float	m_fBrightest;
			float	m_maxAge;


	}; // CPuff
#endif
}



