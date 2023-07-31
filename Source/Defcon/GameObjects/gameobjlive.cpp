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
	m_bCanMove(true),
	m_maxThrust(1.0f),
	m_fThrustDuration_Vertical(0.0f),
	m_fThrustDuration_Forwards(0.0f),
	m_fThrustDuration_Backwards(0.0f)
{
	Drag = 0.1f;

	SetShieldStrength(1.0f);

	for(int32 i = 0; i < array_size(m_navCtls); i++)
	{
		m_navCtls[i].bActive = false;
	}
}


bool Defcon::ILiveGameObject::RegisterImpact(float f)
{
	// Lower shields by <f>, and return true if shields go below zero.

  	m_fShields -= f;
	const bool b = (m_fShields < 0.0f);
	m_fShields = FMath::Max(0.0f, m_fShields);
	//UE_LOG(LogGame, Log, TEXT("%S: %s shields now at %d%%"), __FUNCTION__, *ObjectTypeManager.GetName(Type), ROUND(m_fShields * 100));
	return b;
}


void Defcon::ILiveGameObject::SetShieldStrength(float f)	
{
	m_fShields = f; 
	//UE_LOG(LogGame, Log, TEXT("%S: %s shields now at %d%%"), __FUNCTION__, *ObjectTypeManager.GetName(Type), ROUND(m_fShields * 100));
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

	if(!m_bCanMove)
	{
		return 0.0f;
	}

	if(!m_navCtls[i].bActive)
	{
		return 0.0f;
	}

	const float f = (float)(GameTime() - m_navCtls[i].fTimeDown);
	return FMath::Max(0.0f, f);
}


void Defcon::ILiveGameObject::SetNavControl(int i, bool b, float f)
{
	m_navCtls[i].bActive = b;
	m_navCtls[i].fTimeDown = f;
}


void Defcon::ILiveGameObject::ComputeThrustTimings(float frameTime)
{
	// Determine how long the object has had a thrust 
	// applied to it. If it is longer than the frametime, 
	// make it equal to the frametime.

	check(frameTime > 0.0f);

	m_fThrustDuration_Forwards  = this->NavControl_Duration(ctlFwd);
	m_fThrustDuration_Backwards = this->NavControl_Duration(ctlBack);
	m_fThrustDuration_Forwards  = FMath::Min(m_fThrustDuration_Forwards, frameTime);
	m_fThrustDuration_Backwards = FMath::Min(m_fThrustDuration_Backwards, frameTime);
	

	// If neither up or down thrusts active, set the vert. thrust to zero.
	if(!m_navCtls[ctlUp].bActive && !m_navCtls[ctlDown].bActive)
	{
		m_fThrustDuration_Vertical = 0;
		return;
	}

	m_fThrustDuration_Vertical += this->NavControl_Duration(ctlUp);
	m_fThrustDuration_Vertical -= this->NavControl_Duration(ctlDown);

	// Clamp vertical thrust duration to the frametime.
	m_fThrustDuration_Vertical = FMath::Min(m_fThrustDuration_Vertical, frameTime);
	m_fThrustDuration_Vertical = FMath::Max(m_fThrustDuration_Vertical, -frameTime);
}


void Defcon::ILiveGameObject::ComputeForces(float frametime)
{
	check(frametime > 0.0f);

	const float kHorzSpeedScaler = 1.66f;
	const float kVertSpeedScaler = 0.75f;

	if(frametime > 0.0f)
	{
		m_thrustVector = Orientation.fwd;

		if(Orientation.fwd.x > 0)
		{
			m_thrustVector.Mul(m_fThrustDuration_Forwards * kHorzSpeedScaler);
			m_thrustVector.MulAdd(Orientation.fwd, -m_fThrustDuration_Backwards * kHorzSpeedScaler);
		}
		else
		{
			m_thrustVector.Mul(m_fThrustDuration_Backwards * kHorzSpeedScaler);
			m_thrustVector.MulAdd(Orientation.fwd, -m_fThrustDuration_Forwards * kHorzSpeedScaler);
		}

		m_thrustVector.MulAdd(Orientation.up, m_fThrustDuration_Vertical * kVertSpeedScaler);
		m_thrustVector.Mul(m_maxThrust / frametime);
	}
}


void Defcon::ILiveGameObject::ImpartForces(float DeltaTime)
{
	check(Mass > 0.0f);

	if(!m_bCanMove)
	{
		return;
	}

	check(Drag > 0);

	const float FT = DeltaTime;

	//do thrust & drag
	int count = (int)(DeltaTime / FT);
	const double r = fmod(DeltaTime, FT);
	const double k = r / FT;

	CFPoint accel = m_thrustVector;
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
