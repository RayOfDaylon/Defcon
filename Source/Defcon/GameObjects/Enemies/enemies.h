// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// enemies.h

#include "Common/PaintArguments.h"
#include "Common/util_geom.h"
#include "Common/util_color.h"

#include "GameObjects/gameobjlive.h"
#include "Runtime/SlateCore/Public/Brushes/SlateColorBrush.h"



namespace Defcon
{
	class CEnemy : public ILiveGameObject
	{
		// Base class for all hostile enemy objects.

		typedef ILiveGameObject Super;


		public:

			CEnemy();
			virtual ~CEnemy();

			virtual	void	Init					(const CFPoint& ArenaSize, const CFPoint& ScreenSize) override;
			virtual void	Notify					(Message, void*);

			virtual void	Move					(float);
			virtual void	Draw					(FPaintArguments&, const I2DCoordMapper&);
			virtual void	OnAboutToDie			();

			virtual void	Explode					(CGameObjectCollection& goc) { Super::Explode(goc); }
			virtual int		GetExplosionColorBase	() const { return Super::GetExplosionColorBase(); }
			virtual float	GetExplosionMass		() const { return Super::GetExplosionMass(); }

			virtual void	SetTarget				(IGameObject* p)	{ m_pTarget = p; }

			virtual bool	Fireballs				() const { return true; }


		protected:

			IGameObject*	m_pTarget = nullptr;

			float			m_fBrightness;
			float			m_ctlStartTime[Super::numCtls];
	};
}
