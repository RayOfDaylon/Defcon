// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

/* 
	util_core.h
	Core General-purpose utility header file.
*/


//float GameTime();

#ifdef __cplusplus
extern "C"
{
#endif

// Useful macros.

#define DELETE_AND_NULL(_p)  { delete (_p); (_p) = nullptr; }
#define SAFE_DELETE(_p)      { if((_p) != nullptr) { DELETE_AND_NULL(_p); }}


#define array_size(_a)	(sizeof(_a) / sizeof((_a)[0]))
//#define STRLOOKUP(n, s)	(strlookup((int32)(n), (s), array_size(s)))
#define ZEROFILL(obj)	memset(&obj, 0, sizeof(obj))

//char* strlookup(int32 n, char** ppsz, int32 tablelen);


// Some DLLs just can't link to MSVCRT's stricmp(), 
// so if this macro is defined, use our own version.
#ifdef __USE_LOCAL_STRICMP
	int our_stricmp(const char*, const char*);
#else
	#define our_stricmp(s1, s2)  _stricmp(s1, s2)
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

