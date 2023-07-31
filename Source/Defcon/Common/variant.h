// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

/*
	Variants are like their VB namesake: they are 
	objects that can be of any scalar class depending 
	on their typeid. They are used for defining 
	preference variables.
*/

#include "CoreMinimal.h"
#include "util_str.h"


typedef enum
{
	type_bool,
	type_int,
	type_float,
	type_choice
} EVarType;


class CChoiceNames
{
	public:
		TArray<char*>	Strings;
};


class CPrefVar;

typedef void(*VARCHANGECALLBACK)(CPrefVar&, float, void*);

class CVarMetadata
{
	// A variable metadata instance provides 
	// descriptions and type id'ing of a variable.

	public:
		void Init(
			char* pszName, 
			char* pszDesc, 
			char* pszUnits, 
			EVarType InVarType, 
			float fmin, float fmax, 
			CChoiceNames* pChoiceNames = nullptr)
		{
			Name           = pszName;
			m_pszDesc      = pszDesc;
			m_pszUnits     = pszUnits;
			VarType        = InVarType;
			m_fMin         = fmin;
			m_fMax         = fmax;
			m_pChoiceNames = pChoiceNames;
		}

		FString			Name;
		char*			m_pszDesc;
		char*			m_pszUnits;

		EVarType		VarType;
		float			m_fMin, m_fMax;
		CChoiceNames*	m_pChoiceNames;
};


class CPrefVar
{
	public:
		void Init(
			float             InValue, 
			char*             pszUnits, 
			EVarType          VarType, 
			float             fmin, 
			float             fmax, 
			char*             pszName, 
			char*             pszDesc, 
			CChoiceNames*     pChoiceNames = nullptr, 
			VARCHANGECALLBACK fnCB = nullptr,
			void*             pvUser = nullptr)
		{
			Value = InValue;
			m_fnCB   = fnCB;
			m_pvUser = pvUser;

			m_metadata.Init(pszName, pszDesc, pszUnits, VarType, fmin, fmax, pChoiceNames);
		}


		float   GetValue        () const { return Value; }
		void    SetValue        (float InValue) { const float f = Value; Value = InValue; InvokeCallback(f); }
		char*   GetValueText    (char*) const;
		int32   GetChoiceCount  () const { return m_metadata.m_pChoiceNames->Strings.Num(); }
		bool    Is              (const FString& Str) const	{ return (0 == Str.Compare(m_metadata.Name, ESearchCase::IgnoreCase)); }
		bool    HasCallback     () const { return (m_fnCB != nullptr); }
		void    InvokeCallback  (float f) { if(this->HasCallback()) { m_fnCB(*this, f, m_pvUser); } }

		float               Value;
		VARCHANGECALLBACK   m_fnCB;
		void*               m_pvUser;
		CVarMetadata	    m_metadata;

};


