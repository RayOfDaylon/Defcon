// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	gameobjlive.cpp
*/


#include "gameobjlive.h"
#include "Common/util_core.h"

#include "DefconLogging.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



Defcon::ILiveGameObject::ILiveGameObject()
	:
	bCanMove(true),
	MaxThrust(1.0f),
	ThrustDurationVertical(0.0f),
	ThrustDurationForwards(0.0f),
	ThrustDurationBackwards(0.0f)
{
	Drag = 0.1f;

	SetShieldStrength(1.0f);

	for(int32 i = 0; i < array_size(NavControls); i++)
	{
		NavControls[i].bActive = false;
	}
}


bool Defcon::ILiveGameObject::RegisterImpact(float f)
{
	// Lower shields by <f>, and return true if shields go below zero.

  	ShieldStrength -= f;
	const bool b = (ShieldStrength < 0.0f);
	ShieldStrength = FMath::Max(0.0f, ShieldStrength);
	//UE_LOG(LogGame, Log, TEXT("%S: %s shields now at %d%%"), __FUNCTION__, *ObjectTypeManager.GetName(Type), ROUND(ShieldStrength * 100));
	return b;
}


void Defcon::ILiveGameObject::SetShieldStrength(float f)	
{
	ShieldStrength = f; 
	//UE_LOG(LogGame, Log, TEXT("%S: %s shields now at %d%%"), __FUNCTION__, *ObjectTypeManager.GetName(Type), ROUND(ShieldStrength * 100));
}


void Defcon::ILiveGameObject::OnAboutToDie() {}


void Defcon::ILiveGameObject::Notify(Defcon::EMessage msg, void* pObj)
{
	IGameObject::Notify(msg, pObj);
}


void Defcon::ILiveGameObject::Move(float fElapsedTime)
{
	Age += fElapsedTime;

	Inertia = Position;

	this->ComputeThrustTimings(fElapsedTime);
	this->ComputeForces(fElapsedTime);
	this->ImpartForces(fElapsedTime);

	Inertia = Position - Inertia;
}


void Defcon::ILiveGameObject::ChangeThrust(const CFPoint& f)
{
}


float Defcon::ILiveGameObject::NavControl_Duration(int i) const
{
	// Report how long a nav control has been continually used, in seconds.

	if(!bCanMove)
	{
		return 0.0f;
	}

	if(!NavControls[i].bActive)
	{
		return 0.0f;
	}

	const float f = (float)(GameTime() - NavControls[i].TimeDown);
	return FMath::Max(0.0f, f);
}


void Defcon::ILiveGameObject::SetNavControl(int i, bool b, float f)
{
	NavControls[i].bActive = b;
	NavControls[i].TimeDown = f;
}


void Defcon::ILiveGameObject::ComputeThrustTimings(float frameTime)
{
	// Determine how long the object has had a thrust 
	// applied to it. If it is longer than the frametime, 
	// make it equal to the frametime.

	check(frameTime > 0.0f);

	ThrustDurationForwards  = this->NavControl_Duration(ctlFwd);
	ThrustDurationBackwards = this->NavControl_Duration(ctlBack);
	ThrustDurationForwards  = FMath::Min(ThrustDurationForwards, frameTime);
	ThrustDurationBackwards = FMath::Min(ThrustDurationBackwards, frameTime);
	

	// If neither up or down thrusts active, set the vert. thrust to zero.
	if(!NavControls[ctlUp].bActive && !NavControls[ctlDown].bActive)
	{
		ThrustDurationVertical = 0;
		return;
	}

	ThrustDurationVertical += this->NavControl_Duration(ctlUp);
	ThrustDurationVertical -= this->NavControl_Duration(ctlDown);

	// Clamp vertical thrust duration to the frametime.
	ThrustDurationVertical = FMath::Min(ThrustDurationVertical, frameTime);
	ThrustDurationVertical = FMath::Max(ThrustDurationVertical, -frameTime);
}


void Defcon::ILiveGameObject::ComputeForces(float frametime)
{
	check(frametime > 0.0f);

	const float kHorzSpeedScaler = 1.66f;
	const float kVertSpeedScaler = 0.75f;

	if(frametime > 0.0f)
	{
		ThrustVector = Orientation.Fwd;

		if(Orientation.Fwd.x > 0)
		{
			ThrustVector.Mul(ThrustDurationForwards * kHorzSpeedScaler);
			ThrustVector.MulAdd(Orientation.Fwd, -ThrustDurationBackwards * kHorzSpeedScaler);
		}
		else
		{
			ThrustVector.Mul(ThrustDurationBackwards * kHorzSpeedScaler);
			ThrustVector.MulAdd(Orientation.Fwd, -ThrustDurationForwards * kHorzSpeedScaler);
		}

		ThrustVector.MulAdd(Orientation.Up, ThrustDurationVertical * kVertSpeedScaler);
		ThrustVector.Mul(MaxThrust / frametime);
	}
}


void Defcon::ILiveGameObject::ImpartForces(float DeltaTime)
{
	check(Mass > 0.0f);

	if(!bCanMove)
	{
		return;
	}

	check(Drag > 0);

	const float FT = DeltaTime;

	//do thrust & drag
	int count = (int)(DeltaTime / FT);
	const double r = fmod(DeltaTime, FT);
	const double k = r / FT;

	CFPoint accel = ThrustVector;
	accel.Div(Mass);

	while(count--)
	{
		Velocity.Add(accel);
		Velocity.Mul(1.0f - Drag);
	}

	accel.Mul((float)k);
	Velocity.Add(accel);
	Velocity.Mul(1.0f - (float)k * Drag);

	// Now move the object.
	Position.MulAdd(Velocity, DeltaTime);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
