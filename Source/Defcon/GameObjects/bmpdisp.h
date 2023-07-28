// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once



#include "Common/PaintArguments.h"
#include "GameObjects/gameobjlive.h"


namespace Defcon
{
	// This class is used just as an IGameObject wrapper around a manually defined UE sprite.
	// e.g. the explosion fireball that precedes the debris pushing outward.

	class CBitmapDisplayer : public IGameObject
	{
		public:
			CBitmapDisplayer();
			virtual ~CBitmapDisplayer();

			virtual void InitBitmapDisplayer(size_t, size_t, float, float, float);

#ifdef _DEBUG
			virtual const char* GetClassname() const;
#endif

			virtual void Move(float);
			virtual void Draw(FPaintArguments&, const I2DCoordMapper&);
			virtual void DrawSmall(FPaintArguments&, const I2DCoordMapper&, FSlateBrush&) override;

		protected:
			size_t		m_nFrame, m_nBaseID, m_nFrameCount;

		private:
			float		m_fOrgLifespan;
			float		m_fAlphaScale;
			float		m_fAlphaMin;

	}; // CBitmapDisplayer
}

