// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	Variants are like their VB namesake: they are objects 
	that can be of any scalar class depending on their typeid. 
	They are used for defining preference variables.
*/


#include "variant.h"
#include "util_str.h"
#include "compat.h"


char* FPrefVar::GetValueText(char* psz) const
{
	switch(Metadata.VarType)
	{
		case EVarType::Boolean:
			MyStrcpy(psz, (Value != 0.0f) ? "true" : "false");
			break;

		case EVarType::Integer:
			MySprintf(psz, "%d", (int32)Value);
			break;

		case EVarType::Float:
			if(strcmp_ci(Metadata.Units, "percent") == 0 || Metadata.Units[0] == '%')
				fnicesprintf(psz, Value * 100, 6);
			else
				fnicesprintf(psz, Value, 6);
			break;

		case EVarType::Choice:
		{
			int32 I = (int32)Value;
			check(I < Metadata.ChoiceNames->Strings.Num());
			MyStrcpy(psz, Metadata.ChoiceNames->Strings[I]);
		}
			break;

		default:
			check(false);
			break;
	}
	return psz;
}

