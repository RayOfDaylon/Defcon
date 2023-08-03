// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// flak.h

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
			virtual void Move      (float DeltaTime) override;
			virtual void Draw      (FPainter&, const I2DCoordMapper&) override;
			virtual void DrawSmall (FPainter&, const I2DCoordMapper&, FSlateBrush&) override {}

			virtual bool OccursFrequently () const override { return true; }

			float	LargestSize = 6;
			EColor	ColorbaseOld;
			EColor	ColorbaseYoung;
			bool	bFade = true;
			bool	bCold;

		protected:
			float          MaxAge;
			FSlateBrush*   BrushPtr = nullptr;
	};


	class CGlowingFlak : public CFlak
	{
		public:
			//CGlowingFlak();
			//virtual ~CGlowingFlak();

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif
			//virtual void Move(float);
			virtual void Draw(FPainter&, const I2DCoordMapper&) override;
			//virtual void DrawSmall(FPaintArguments&, const I2DCoordMapper&);

			//virtual bool OccursFrequently() const { return true; }


	};


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

			float	LargestSize;
			float	m_fBrightest;
			float	MaxAge;


	}; // CPuff
#endif
}



