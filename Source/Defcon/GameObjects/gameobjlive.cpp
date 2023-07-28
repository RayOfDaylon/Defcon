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
	m_fDrag = 0.1f;

	SetShieldStrength(1.0f);

	for(size_t i = 0; i < array_size(m_navCtls); i++)
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
	//UE_LOG(LogGame, Log, TEXT("%S: %s shields now at %d%%"), __FUNCTION__, *ObjectTypeManager.GetName(m_type), ROUND(m_fShields * 100));
	return b;
}


void Defcon::ILiveGameObject::SetShieldStrength(float f)	
{
	m_fShields = f; 
	//UE_LOG(LogGame, Log, TEXT("%S: %s shields now at %d%%"), __FUNCTION__, *ObjectTypeManager.GetName(m_type), ROUND(m_fShields * 100));
}


void Defcon::ILiveGameObject::OnAboutToDie() {}


void Defcon::ILiveGameObject::Notify(Defcon::Message msg, void* pObj)
{
	IGameObject::Notify(msg, pObj);
}


void Defcon::ILiveGameObject::Move(float fElapsedTime)
{
	m_fAge += fElapsedTime;

	m_inertia = m_pos;

	this->ComputeThrustTimings(fElapsedTime);
	this->ComputeForces(fElapsedTime);
	this->ImpartForces(fElapsedTime);

	m_inertia = m_pos - m_inertia;
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
		m_thrustVector = m_orient.fwd;

		if(m_orient.fwd.x > 0)
		{
			m_thrustVector.mul(m_fThrustDuration_Forwards * kHorzSpeedScaler);
			m_thrustVector.muladd(m_orient.fwd, -m_fThrustDuration_Backwards * kHorzSpeedScaler);
		}
		else
		{
			m_thrustVector.mul(m_fThrustDuration_Backwards * kHorzSpeedScaler);
			m_thrustVector.muladd(m_orient.fwd, -m_fThrustDuration_Forwards * kHorzSpeedScaler);
		}

		m_thrustVector.muladd(m_orient.up, m_fThrustDuration_Vertical * kVertSpeedScaler);
		m_thrustVector.mul(m_maxThrust / frametime);
	}
}


void Defcon::ILiveGameObject::ImpartForces(float frametime)
{
	check(frametime > 0.0f);
	check(m_fMass > 0.0f);

	if(!m_bCanMove)
		return;

	check(m_fDrag > 0);

	const float FT = frametime;//0.0001f;

	//do thrust & drag
	int count = (int)(frametime / FT);
	double r = fmod(frametime, FT);
	double k = r / FT;

	CFPoint accel = m_thrustVector;
	accel.div(m_fMass);

	while(count--)
	{
		m_velocity.add(accel);
		m_velocity.mul(1.0f - m_fDrag);
	}

	accel.mul((float)k);
	m_velocity.add(accel);
	m_velocity.mul(1.0f - (float)k * m_fDrag);

	// Now move the object.
	m_pos.muladd(m_velocity, frametime);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
