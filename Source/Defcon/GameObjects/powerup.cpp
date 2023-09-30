#include "powerup.h"
#include "Common/util_color.h"


Defcon::IPowerup::IPowerup()
{
	ParentType = Type;
	Type       = EObjType::POWERUP;
}


Defcon::IPowerup* Defcon::IPowerup::Make(EObjType Kind)
{
	switch(Kind)
	{
		case EObjType::POWERUP_DUALCANNON:     return new CDoubleGunsPowerup;
		case EObjType::POWERUP_INVINCIBILITY:  return new CInvincibilityPowerup;
		case EObjType::POWERUP_SHIELDS:        return new CShieldPowerup;
	}

	check(false);
	return nullptr;
}


void Defcon::IPowerup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Make our radar blip flash.
	RadarColor.A = PSIN(Age * TWO_PI * 2);
}

// --------------------------------------------------------------------------------

Defcon::CShieldPowerup::CShieldPowerup()
{
	ParentType = Type;
	Type       = EObjType::POWERUP_SHIELDS;

	RadarColor = C_BLUE;

	CreateSprite(Type);

	const auto& Info = GGameObjectResources.Get(Type);
	BboxRadius = Info.Size * 0.5f;
}


bool Defcon::CShieldPowerup::Apply(CPlayerShip& PlayerShip)
{
	float Amt = PlayerShip.GetShieldStrength();

	if(Amt == 1.0f)
	{
		Defcon::GMessageMediator.TellUser(TEXT("SHIELDS ALREADY AT MAXIMUM"), 0.0f, Defcon::EDisplayMessage::ShieldLevelChanged);
		return false;
	}

	Amt = FMath::Min(1.0f, Amt + 0.25f);
	PlayerShip.SetShieldStrength(Amt);
	auto Str = FString::Printf(TEXT("SHIELDS INCREASED TO %d PERCENT"), ROUND(Amt * 100.0f));
	Defcon::GMessageMediator.TellUser(Str, 0.0f, Defcon::EDisplayMessage::ShieldLevelChanged);

	return true;
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


bool Defcon::CInvincibilityPowerup::Apply(CPlayerShip& PlayerShip)
{
	const float OldAmt = PlayerShip.GetInvincibility();

	if(OldAmt == 1.0f)
	{
		Defcon::GMessageMediator.TellUser(TEXT("INVINCIBILITY ALREADY AT MAXIMUM"), 0.0f, Defcon::EDisplayMessage::InvincibilityLevelChanged);
		return false;
	}
	
	float NewAmt = FMath::Min(1.0f, OldAmt + 0.25f);
						
	PlayerShip.AddInvincibility(NewAmt - OldAmt);
	auto Str = FString::Printf(TEXT("INVINCIBILITY INCREASED TO %d PERCENT"), ROUND(NewAmt * 100.0f));
	Defcon::GMessageMediator.TellUser(Str, 0.0f, Defcon::EDisplayMessage::InvincibilityLevelChanged);

	return true;
}

// --------------------------------------------------------------------------------

Defcon::CDoubleGunsPowerup::CDoubleGunsPowerup()
{
	ParentType = Type;
	Type       = EObjType::POWERUP_DUALCANNON;

	RadarColor = C_YELLOW;

	CreateSprite(Type);

	const auto& Info = GGameObjectResources.Get(Type);
	BboxRadius = Info.Size * 0.5f;
}


bool Defcon::CDoubleGunsPowerup::Apply(CPlayerShip& PlayerShip)
{
	const float OldAmt = PlayerShip.GetDoubleGunPower();

	if(OldAmt == 1.0f)
	{
		Defcon::GMessageMediator.TellUser(TEXT("DUAL LASER CANNON ENERGY ALREADY AT MAXIMUM"), 0.0f, Defcon::EDisplayMessage::DualCannonsLevelChanged);
		return false;
	}

	float NewAmt = FMath::Min(1.0f, OldAmt + 0.25f);
						
	PlayerShip.AddDoubleGunPower(NewAmt - OldAmt);
	auto Str = FString::Printf(TEXT("DUAL LASER CANNON ENERGY INCREASED TO %d PERCENT"), ROUND(NewAmt * 100.0f));
	Defcon::GMessageMediator.TellUser(Str, 0.0f, Defcon::EDisplayMessage::DualCannonsLevelChanged);

	return true;
}

