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
} VarType;


class CChoiceNames
{
	public:
		TArray<char*>	m_strings;
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
			VarType vtype, 
			float fmin, float fmax, 
			CChoiceNames* pChoiceNames = nullptr)
		{
			m_pszName = pszName;
			m_pszDesc = pszDesc;
			m_pszUnits = pszUnits;
			m_eVarType = vtype;
			m_fMin = fmin;
			m_fMax = fmax;
			m_pChoiceNames = pChoiceNames;
		}

/*		enum { maxNameLen = 50 };
		enum { maxDescLen = 150 };
		enum { maxUnitsLen = 40 };

		char			m_szName[maxNameLen];
		char			m_szDesc[maxDescLen];
		char			m_szUnits[maxUnitsLen];
*/
		FString			m_pszName;
		char*			m_pszDesc;
		char*			m_pszUnits;

		VarType			m_eVarType;
		float			m_fMin, m_fMax;
		CChoiceNames*	m_pChoiceNames;
};


class CPrefVar
{
	public:
		void Init(
			float             v, 
			char*             pszUnits, 
			VarType           vtype, 
			float             fmin, 
			float             fmax, 
			char*             pszName, 
			char*             pszDesc, 
			CChoiceNames*     pChoiceNames = nullptr, 
			VARCHANGECALLBACK fnCB = nullptr,
			void*             pvUser = nullptr)
		{
			m_fValue = v;
			m_fnCB   = fnCB;
			m_pvUser = pvUser;

			m_metadata.Init(pszName, pszDesc, pszUnits, vtype, fmin, fmax, pChoiceNames);
		}


		float   GetValue        () const { return m_fValue; }
		void    SetValue        (float val) { float f = m_fValue; m_fValue = val; this->InvokeCallback(f); }
		char*   GetValueText    (char*) const;
		size_t  GetChoiceCount  () const { return m_metadata.m_pChoiceNames->m_strings.Num(); }
		bool    Is              (const FString& psz) const	{ return (0 == psz.Compare(m_metadata.m_pszName, ESearchCase::IgnoreCase)); }
		bool    HasCallback     () const { return (m_fnCB != nullptr); }
		void    InvokeCallback  (float f) { if(this->HasCallback()) { m_fnCB(*this, f, m_pvUser); } }

		float               m_fValue;
		VARCHANGECALLBACK   m_fnCB;
		void*               m_pvUser;
		CVarMetadata	    m_metadata;

}; // CPrefVar


