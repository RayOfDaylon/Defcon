// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DaylonRange.h"


namespace Daylon
{
	// Mersenne Twister random number generator -- a C++ class MTRand
	// Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
	// Richard J. Wagner  v1.0  15 May 2003  rjwagner@writeme.com

	// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
	// Copyright (C) 2000 - 2003, Richard J. Wagner
	// All rights reserved.
	
	// Modified slightly (e.g. args passed by const scalar reference are passed by value,
	// unneeded functions ommitted, no 'register' keyword, etc.)

	class MTRand
	{
		// Data
		public:
	
			enum { N = 624 };       // length of state vector
			enum { SAVE = N + 1 };  // length of array for save()


		protected:

			enum { M = 397 };  // period parameter
	
			uint32  state[N];   // internal state
			uint32* pNext;      // next value to get from state
			int     left;       // number of values left before reload needed


		public:

			MTRand(const uint32& oneSeed);  // initialize with a simple uint32
			MTRand();  // auto-initialize with /dev/urandom or time() and clock()
	
			// Do NOT use for CRYPTOGRAPHY without securely hashing several returned
			// values together, otherwise the generator state can be learned after
			// reading 624 consecutive values.
	
			// Access to 32-bit random numbers
			double rand         ();              // real number in [0,1]
			double rand         (double n);      // real number in [0,n]
			uint32 randInt      ();              // integer in [0,2^32-1]
			uint32 randInt      (uint32 n);      // integer in [0,n] for n < 2^32
			double operator()   () { return this->rand(); }  // same as rand()
	
			// Access to 53-bit random numbers (capacity of IEEE double precision)
			double rand53       ();              // real number in [0,1)
	
			// Re-seeding functions with same behavior as initializers
			void seed           ();
			void seed           (uint32);


		protected:
			void            initialize (uint32 oneSeed);
			void            reload     ();
			uint32          hiBit      (uint32 u) const { return u & 0x80000000UL; }
			uint32          loBit      (uint32 u) const { return u & 0x00000001UL; }
			uint32          loBits     (uint32 u) const { return u & 0x7fffffffUL; }
			uint32          mixBits    (uint32 u, uint32 v) const { return hiBit(u) | loBits(v); }
			uint32          twist      (uint32 m, uint32 s0, uint32 s1) const { return m ^ (mixBits(s0,s1)>>1) ^ (negate(loBit(s1)) & 0x9908b0dfUL); }
			static uint32   hash       (time_t t, clock_t c);
			uint32          flipHiBit  (uint32 u) const { return (hiBit(u ^ 0x80000000UL)) | loBits(u); }
			uint32          negate     (uint32 u) const { int n = -(const int&)u; return *(uint32*)&n; }
	};


	inline        MTRand::MTRand  ()                       { this->seed(); }
	inline        MTRand::MTRand  (const uint32& oneSeed)  { this->seed(oneSeed); }
	inline double MTRand::rand    ()                       { return double(randInt()) * (1.0/4294967295.0); }
	inline double MTRand::rand    (double n)               { return this->rand() * n; }

	inline double MTRand::rand53()
	{
		uint32 a = randInt() >> 5, b = randInt() >> 6;
		return ( a * 67108864.0 + b ) * (1.0 / 9007199254740992.0);  // by Isaku Wada
	}


	inline uint32 MTRand::randInt()
	{
		// Pull a 32-bit integer from the generator state
		// Every other access function simply transforms the numbers extracted here
	
		if( left == 0 ) 
		{
			reload();
		}

		--left;
		
		uint32 s1 = *pNext++;

		s1 ^= (s1 >> 11);
		s1 ^= (s1 <<  7) & 0x9d2c5680UL;
		s1 ^= (s1 << 15) & 0xefc60000UL;
		
		return ( s1 ^ (s1 >> 18) );
	}


	inline uint32 MTRand::randInt(uint32 n)
	{
		// Find which bits are used in n
		// Optimized by Magnus Jonsson (magnus@smartelectronix.com)
		uint32 used = n;

		used |= used >> 1;
		used |= used >> 2;
		used |= used >> 4;
		used |= used >> 8;
		used |= used >> 16;
	
		// Draw numbers until one is found in [0,n]
		uint32 i;
		do
		{
			i = randInt() & used;  // toss unused bits to shorten search
		}
		while( i > n );

		return i;
	}


	inline void MTRand::seed(uint32 oneSeed)
	{
		// Seed the generator with a simple uint32
		this->initialize(oneSeed);
		this->reload();
	}


	inline void MTRand::seed()
	{
		// Seed the generator with an array from /dev/urandom if available
		// Otherwise use a hash of time() and clock() values
	
#if 0
		// First try getting an array from /dev/urandom
		FILE* urandom = fopen( "/dev/urandom", "rb" );
		if( urandom )
		{
			uint32 bigSeed[N];
			uint32 *s = bigSeed;
			int i = N;
			bool success = true;
			while( success && (0 != i--) )
				success = (0 != fread( s++, sizeof(uint32), 1, urandom ));
			fclose(urandom);
			if( success ) { this->seed( bigSeed, N );  return; }
		}
#endif	
		// Was not successful, so use time() and clock() instead
		this->seed(hash(time(NULL), ::clock()));
	}


	inline void MTRand::initialize(uint32 Seed)
	{
		// Initialize generator state with seed
		// See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
		// In previous versions, most significant bits (MSBs) of the seed affect
		// only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto.

		uint32* s = state;
		uint32* r = state;

		int i = 1;
		*s++ = Seed & 0xffffffffUL;

		for( ; i < N; ++i )
		{
			*s++ = ( 1812433253UL * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffUL;
			r++;
		}
	}


	inline void MTRand::reload()
	{
		// Generate N new values in state
		// Made clearer and faster by Matthew Bellew (matthew.bellew@home.com)

		uint32 *p = state;
		int i;

		for( i = N - M; i--; ++p )
			*p = twist( p[M], p[0], p[1] );

		for( i = M; --i; ++p )
			*p = twist( p[M-N], p[0], p[1] );

		*p = twist( p[M-N], p[0], state[0] );

		left = N, pNext = state;
	}


	inline uint32 MTRand::hash(time_t t, clock_t c)
	{
		// Get a uint32 from t and c
		// Better than uint32(x) in case x is floating point in [0,1]
		// Based on code by Lawrence Kirby (fred@genesis.demon.co.uk)

		static uint32 differ = 0;  // guarantee time-based seeds will change

		uint32 h1 = 0;

		uint8* p = (uint8*) &t;

		for(size_t i = 0; i < sizeof(t); ++i)
		{
			h1 *= UCHAR_MAX + 2U;
			h1 += p[i];
		}

		uint32 h2 = 0;
		p = (uint8*) &c;

		for(size_t j = 0; j < sizeof(c); ++j)
		{
			h2 *= UCHAR_MAX + 2U;
			h2 += p[j];
		}

		return ( h1 + differ++ ) ^ h2;
	}


	// Mimic UE's FMath RNG APIs.

	extern MTRand Rng;

	DAYLONGRAPHICSLIBRARY_API double FRand      ();
	DAYLONGRAPHICSLIBRARY_API double FRandRange (double Min, double Max);
	DAYLONGRAPHICSLIBRARY_API int32  RandRange  (int32 Min, int32 Max);
	DAYLONGRAPHICSLIBRARY_API bool   RandBool   ();

	inline float FRandRange(const FRange<float>& Range)
	{
		return (float)FRandRange(Range.Low(), Range.High());
	}
}
