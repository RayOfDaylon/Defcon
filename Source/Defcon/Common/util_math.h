// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

/* 
	util_math.h
	Math-related utility routines
*/

#include "CoreMinimal.h"
#include "DaylonUtils.h"

#include <math.h>


#ifndef ABS
#define ABS(n)	((n) < 0 ? -(n) : (n))
#endif

#ifndef SGN
#define SGN(n)	((n) < 0 ? -1 : ((n) > 0 ? 1 : 0) )
#endif

#ifndef SGN2
#define SGN2(n) ((n) >= 0 ? 1 : -1)
#endif

//#define MIN3(a, b, c)	min(min(a, b), c)
#define MIN3(a, b, c)	( (a) < (b) && (a) < (c) ? (a) : FMath::Min(b, c) ) 
//#define MAX3(a, b, c)	max(max(a, b), c)
#define MAX3(a, b, c)	( (a) > (b) && (a) > (c) ? (a) : FMath::Max(b, c) ) 

#define LERP(a, b, t)		(((t) * ((b) - (a))) + (a))
#define NORM_(n, mn, mx)	((float)((n)-(mn)) / ((mx)-(mn)))
#define INVNORM(n, mn, mx)	((float)((mx)-(n)) / ((mx)-(mn)))
#define AVG(a, b)			(((a) + (b)) / 2)
// FMath probably has a clamp function, we should wrap that
#define CLAMP(n, a, b)		( FMath::Min((b), FMath::Max((a), (n))) )
#define WRAP(n, a, b)		{ if((n) < (a)) { (n) += (b)-(a); } else if((n) >= (b)) { (n) -= (b)-(a); } }


// MAP maps n which is between a1..b1 to a proportional
// value between a2..b2.
#define MAP(n, a1, b1, a2, b2)		LERP((a2), (b2), NORM_((n), (a1), (b1)))

// Oscillate smoothly between 0...1 instead of -1 ... +1.
#define PSIN(r)	(float)(sin(r) / 2 + 0.5)

// Rounding routines.
// ROUND is the classic fast rounder, but it 
// moves negative values incorrectly, e.g. -2.7 --> -2.0.
// SROUND takes the sign into account.
#define ROUND(_f)	(int)((_f) + 0.5f)
#define SROUND(_f)	(int)((_f) + (0.5f * SGN(_f)))

// Signed min/max work like their normal namesakes, 
// except that the absolute magnitude of the numbers 
// is compared and the winner's sign preserved.
#define SMIN(a, b)	( ABS(a) < ABS(b) ? (a) : (b) )
#define SMAX(a, b)	( ABS(a) > ABS(b) ? (a) : (b) )

// Magnitude min/max is like signed min/max except 
// that the result is always positive.
#define ABSMIN(a, b)	( ABS(a) < ABS(b) ? ABS(a) : ABS(b) )
#define ABSMAX(a, b)	( ABS(a) > ABS(b) ? ABS(a) : ABS(b) )

#ifndef SWAP
	#define SWAP(a, b, t)	{ t = a; a = b; b = t; }
#endif

#define ORDER(a, b, t)	if((a) > (b)) SWAP(a, b, t)

// Numeric and math routines.
#define roundcloser SROUND

// Get a random normalized value.
//#define FRAND	( (float)rand() / RAND_MAX )
#define FRAND ((float)Daylon::FRand())

#define FRANDRANGE(a, b) (float)Daylon::FRandRange((a), (b))

// Get a random value between -1 and +1.
//#define SFRAND	( FRAND * 2.0f - 1.0f )
#define SFRAND    FRANDRANGE(-1.0f, 1.0f)

#define BRAND (Daylon::RandBool())
//#define BRAND	( rand() & 0x01 )

#define IRAND(n)   (Daylon::RandRange(0, (n) - 1))
//#define IRAND(n)	( rand() % (n) )


#define SBRAND	( BRAND ? -1.0f : 1.0f )
