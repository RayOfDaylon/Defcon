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


char* CPrefVar::GetValueText(char* psz) const
{
	switch(m_metadata.m_eVarType)
	{
		case type_bool:
			MyStrcpy(psz, (m_fValue != 0.0f) ? "true" : "false");
			break;

		case type_int:
			MySprintf(psz, "%d", (int)m_fValue);
			break;

		case type_float:
			if(strcmp_ci(m_metadata.m_pszUnits, "percent") == 0 || m_metadata.m_pszUnits[0] == '%')
				fnicesprintf(psz, m_fValue * 100, 6);
			else
				fnicesprintf(psz, m_fValue, 6);
			break;

		case type_choice:
		{
			size_t i = (size_t)m_fValue;
			check(i < m_metadata.m_pChoiceNames->m_strings.Num());
			MyStrcpy(psz, m_metadata.m_pChoiceNames->m_strings[i]);
		}
			break;

		default:
			check(false);
			break;
	}
	return psz;
}

