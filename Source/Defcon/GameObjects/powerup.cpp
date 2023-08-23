#include "powerup.h"


Defcon::IPowerup::IPowerup()
{
	ParentType = Type;
	Type       = EObjType::POWERUP;
}


Defcon::CInvincibilityPowerup::CInvincibilityPowerup()
{
	ParentType = Type;
	Type       = EObjType::POWERUP_INVINCIBILITY;

	CreateSprite(Type);

	const auto& Info = GGameObjectResources.Get(Type);
	BboxRadius = Info.Size * 0.5f;
}
