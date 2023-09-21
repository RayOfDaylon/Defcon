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
#include "DaylonUtils.h"


namespace Daylon
{
	union FVariant 
	{
		bool   Boolean;
		int32  Integer;
		double Real;
	};

	template<typename E> struct FMetadata
	{
		void   Add        (const E& Key, FVariant Value) { Map.Add(Key, Value); }
		void   Add        (const E& Key, bool Value)   { FVariant Var; Var.Boolean = Value; Add(Key, Var); }
		void   Add        (const E& Key, int32 Value)  { FVariant Var; Var.Integer = Value; Add(Key, Var); }
		void   Add        (const E& Key, float Value)  { FVariant Var; Var.Real    = Value; Add(Key, Var); }
		void   Add        (const E& Key, double Value) { FVariant Var; Var.Real    = Value; Add(Key, Var); }

		int32  Num        () const { return Map.Num(); }
		bool   Has        (const E& Key) const { return (Map.Find(Key) != nullptr); }
		bool   GetBool    (const E& Key) const { return Map[Key].Boolean; }
		double GetReal    (const E& Key) const { return Map[Key].Real; }
		int32  GetInteger (const E& Key) const { return Map[Key].Integer; }


		protected:

			TMap<E, FVariant> Map;
	};
}


namespace Defcon
{
	enum class EMetadataKey : uint8
	{
		SpeedX,        // real
		OrientationX,  // real
		SmartbombID    // int
	};

	struct FMetadata : public Daylon::FMetadata<EMetadataKey>
	{
	};
}


enum class EVarType
{
	Boolean,
	Integer,
	Float,
	Choice,
	Range
};


class CChoiceNames
{
	public:
		TArray<char*>   Strings;
};


struct FPrefVar;

typedef void(*VARCHANGECALLBACK)(FPrefVar&, float, void*);


struct FVarMetadata
{
	// A variable metadata instance provides 
	// descriptions and type id'ing of a variable.

	void Init(
		char*         pszName, 
		char*         pszDesc, 
		char*         pszUnits, 
		EVarType      InVarType, 
		float         fmin, 
		float         fmax,
		CChoiceNames* pChoiceNames = nullptr)
	{
		Name        = pszName;
		Desc        = pszDesc;
		Units       = pszUnits;
		VarType     = InVarType;
		Min         = fmin;
		Max         = fmax;
		ChoiceNames = pChoiceNames;
	}

	FString         Name;
	char*           Desc;
	char*           Units;
	CChoiceNames*   ChoiceNames;
	EVarType        VarType;
	float           Min, Max;
};


struct FPrefVar
{
	void Init(
		float             InValue, 
		char*             pszUnits, 
		EVarType          VarType, 
		float             fmin, 
		float             fmax, 
		char*             pszName, 
		char*             pszDesc, 
		CChoiceNames*     pChoiceNames = nullptr, 
		const Daylon::FRange<float>* pRange = nullptr,
		VARCHANGECALLBACK fnCB = nullptr,
		void*             pvUser = nullptr)
	{
		Value = InValue;

		if(VarType == EVarType::Range)
		{
			check(pRange != nullptr);
			Range = *pRange;
		}

		m_fnCB   = fnCB;
		m_pvUser = pvUser;

		Metadata.Init(pszName, pszDesc, pszUnits, VarType, fmin, fmax, pChoiceNames);
	}


	void Init(
		const Daylon::FRange<float>& InRange,
		char*             pszUnits, 
		float             fmin, 
		float             fmax, 
		char*             pszName, 
		char*             pszDesc, 
		VARCHANGECALLBACK fnCB = nullptr,
		void*             pvUser = nullptr)
	{
		Range = InRange;

		m_fnCB   = fnCB;
		m_pvUser = pvUser;

		Metadata.Init(pszName, pszDesc, pszUnits, EVarType::Range, fmin, fmax);
	}


	float   GetValue        () const { return Value; }
	void    SetValue        (float InValue) { const float f = Value; Value = InValue; InvokeCallback(f); }
	char*   GetValueText    (char*) const;
	int32   GetChoiceCount  () const { return Metadata.ChoiceNames->Strings.Num(); }
	bool    Is              (const FString& Str) const  { return (0 == Str.Compare(Metadata.Name, ESearchCase::IgnoreCase)); }
	bool    HasCallback     () const { return (m_fnCB != nullptr); }
	void    InvokeCallback  (float f) { if(HasCallback()) { m_fnCB(*this, f, m_pvUser); } }

	FVarMetadata            Metadata;
	VARCHANGECALLBACK       m_fnCB;
	Daylon::FRange<float>   Range;
	void*                   m_pvUser;
	float                   Value;
};
