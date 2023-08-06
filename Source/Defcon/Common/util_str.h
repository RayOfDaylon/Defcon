// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

/* 
	util_str.h
	String-releated routines. 
*/

#include "CoreMinimal.h"



// String routines.
//int iswhitespace(char*);
//char* str_snr(char*, unsigned char, unsigned char);
//#define str_equal(s1, s2)		(strcmp((s1), (s2)) == 0)
//#define mem_equal(p1, p2, n)	(memcmp((p1), (p2), (n)) == 0)

// todo: this stuff can go away if we don't read prefs from text file

int strcmp_ci(const char*, const char*);
char* str_deltrailingchar(char*);
char* str_delleadingchar(char*);
char* trimleadingwhitespace(char*);
char* trimtrailingwhitespace(char*);
char* trimwhitespace(char*);
bool isnum(const char*);
char* strtoupper(char*);
FString GetNumberSpelling(int32, int32);
char* fnicesprintf(char* dst, double d, int prec, char sep = 0);




