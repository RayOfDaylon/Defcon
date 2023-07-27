#pragma once

#include "gameobjlive.h"


namespace Defcon
{
	class CBeacon : public ILiveGameObject
	{
		typedef ILiveGameObject Super;

		public:
			CBeacon();
			~CBeacon();

#ifdef _DEBUG
			virtual const char* GetClassname() const override { return "Beacon"; }
#endif
			virtual void	Draw					(FPaintArguments&, const I2DCoordMapper&) override;
			virtual void	DrawSmall				(FPaintArguments&, const I2DCoordMapper&, FSlateBrush&) override;

			virtual void    Move                    (float DeltaTime) override;

			size_t			CollisionCount			() const { return m_collision_count; }
			void			IncrementCollisionCount	() { m_collision_count++; }

		private:
			size_t	m_collision_count = 0;
	};
}
