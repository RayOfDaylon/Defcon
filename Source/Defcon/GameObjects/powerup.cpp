#include "powerup.h"
#include "Common/util_color.h"


Defcon::IPowerup::IPowerup()
{
	ParentType = Type;
	Type       = EObjType::POWERUP;
}


void Defcon::IPowerup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Make our radar blip flash.
	RadarColor.A = PSIN(Age * TWO_PI * 2);
}

// --------------------------------------------------------------------------------

Defcon::CInvincibilityPowerup::CInvincibilityPowerup()
{
	ParentType = Type;
	Type       = EObjType::POWERUP_INVINCIBILITY;

	RadarColor = C_WHITE;

	CreateSprite(Type);

	const auto& Info = GGameObjectResources.Get(Type);
	BboxRadius = Info.Size * 0.5f;
}
