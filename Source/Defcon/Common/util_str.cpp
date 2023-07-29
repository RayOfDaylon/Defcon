// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/* 
	util_str.cpp
	String-releated routines. 
*/


#include "util_str.h"

#include <string.h>
#include <cctype>

#include "util_core.h"
#include "compat.h"


#ifndef FALSE
	#define FALSE	0
#endif

#ifndef TRUE
	#define TRUE	1
#endif

#ifndef MAX
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#endif


#if 0
int iswhitespace(char* psz)
{
	while(*psz != 0) { if(*psz++ > ' ') { return FALSE; } }
	return TRUE;
}
#endif


char* str_deltrailingchar(char* psz)
{
	if(*psz != 0)
		psz[strlen(psz)-1] = 0;
	return psz;
}

char* str_delleadingchar(char* psz)
{
	int n = strlen(psz);
	if(n > 0)
		memmove(psz, psz+1, n);
	return psz;
}


char* trimleadingwhitespace(char* psz)
{
	while(*psz != 0 && *psz <= ' ')
		str_delleadingchar(psz);

	return psz;
}


char* trimtrailingwhitespace(char* psz)
{
	while(*psz != 0 && psz[strlen(psz)-1] <= ' ') 
		str_deltrailingchar(psz);

	return psz;
}


char* trimwhitespace(char* psz)
{
	(void) trimleadingwhitespace(psz);
	(void) trimtrailingwhitespace(psz);
	return psz;
}


int isnum(const char* psz)
{
	if(psz == nullptr)
		return FALSE;
	double d = atof(psz);
	if(d == 0)
	{
		for(unsigned int i = 0; i < strlen(psz); i++)
		{
			if((psz[i] < '0' || psz[i] > '9') && psz[i] != '.' && psz[i] != '-')
				return FALSE;
		}
	}

	return TRUE;
}


// Case-insenstive version of strcmp.
// The MSVC stdlib has stricmp in a legacy lib, 
// so to keep things simple we just provide our own function.
int strcmp_ci(const char* s1, const char* s2)
{
	// If both strings are null, treat them as equal.
	if(*s1 == 0 && *s2 == 0)
		return 0;

	return our_stricmp(s1, s2);
}


#if 0
// Search and replace occurances of cOld with cNew in psz.
char* str_snr(char* psz, unsigned char cOld, unsigned char cNew)
{
	check(psz != nullptr);

	unsigned char* p = (unsigned char*)psz;
	while(*p != 0) { if(*p == cOld) { *p = cNew; } p++; }
	return psz;
}
#endif


char* strtoupper(char* psz)
{
	for(char* p = psz; p != nullptr && *p != 0; p++)
		*p = toupper(*p);
	return psz;
}


FString GetNumberSpelling(int32 n, int eCap)
{
	char sz[20];

	static const char* psz[] =
	{
		"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", 
		"eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", 
		"nineteen", "twenty"
	};


	if(n >= array_size(psz))
	{
		return FString::Printf(TEXT("%d"), (int32)n);
	}

	
	strncpy_s(sz, psz[n], strlen(psz[n]));
	//MyStrcpy(sz, psz[n]);
	if(eCap == 1)
	{
		sz[0] = toupper(sz[0]);
	}
	else if(eCap == 2)
	{
		strtoupper(sz);
	}
	return sz;
}


char* fnicesprintf(char* dst, double d, int prec, char sep /* = 0 */)
{
	// Print a nice floating-point value w/o trailing zeros.

	char	fmt[20];
	int		j;

	MySprintf(fmt, "%%.%df", prec);
	MySprintf(dst, fmt, d);
	if(strstr(dst, ".") != nullptr)
	{
		for(j = strlen(dst)-1; dst[j] == '0' && j >= 0; j--)
			dst[j] = 0;
		if(dst[strlen(dst)-1] == '.') // strip trailing decimal point too, if we can.
			dst[strlen(dst)-1] = 0;
		if(strcmp(dst, "-0") == 0)
			MyStrcpy(dst, "0");
	}
	if(sep != 0)
	{
		// Apply thousands separator.
		char* pDec = strstr(dst, ".");
		// Point to digit in the one's column.
		if(pDec == nullptr)
			pDec = dst + strlen(dst)-1;
		else 
			pDec--;
		// Work backwards and insert seps.
		size_t tt = pDec - dst + 1;
		int t = (int)tt;
		int charsToShift = strlen(pDec+1)+1; // incl. terminator!
		while(t > 0)
		{
			t -= 3;
			// If we're out of string, or the char we're at is 
			// a leading minus sign, stop.
			if(t > 0 && *(pDec-(tt-t)) != '-')
			{
				// Insert sep.
				charsToShift += 3;
				memmove(pDec-(tt-t)+2, pDec-(tt-t)+1, charsToShift);
				*(pDec-(tt-t)+1) = sep;
				charsToShift++;
			}
		}
	}
	return dst;
}
