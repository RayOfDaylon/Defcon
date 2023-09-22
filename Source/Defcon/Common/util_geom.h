// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

/* 
	util_geom.h
	Geometry-related utility routines
*/

#include "CoreMinimal.h"

#include <math.h>


#include "util_math.h"


#define FEPSILON	((float)1e-5)
#define DEPSILON	1e-12

#if 0
#ifndef PI
#define PI	3.1415926535897932384626433832795f
#endif

#ifndef TWO_PI
#define TWO_PI	6.283185307179586476925286766559f
#endif

#ifndef HALF_PI
#define HALF_PI	1.5707963267948966192313216916398f
#endif
#endif


#define M_PI_360	 0.00872664625997164788f
#define D2R	         0.017453292f
#define R2D         57.29578121996f

#define DEG2RAD(d)	((d) * D2R)
#define RAD2DEG(r)	((r) * R2D)
#define SQUARED(a)  ((a) * (a))

// Common 2D vector macros.
#define DDISTANCE(dx, dy)         (sqrt(SQUARED(dx) + SQUARED(dy)))
#define DISTANCE(x1, y1, x2, y2)  DDISTANCE((x1) - (x2), (y1) - (y2))
#define PTLEN(pt)                 DDISTANCE((pt).x, (pt).y)
#define PTSLEN(pt1, pt2)          DISTANCE((pt1).x, (pt1).y, (pt2).x, (pt2).y)


struct IntRect
{
	int32 Left, Right, Top, Bottom;

	int32 Width  () const { return Right  - Left; }
	int32 Height () const { return Bottom - Top;  }
};


class CFPoint
{
	public:
		//CFPoint () : x(0.0f), y(0.0f) {}
		CFPoint () : x(std::numeric_limits<float>::quiet_NaN()), y(std::numeric_limits<float>::quiet_NaN()) {}
		CFPoint (float x2, float y2) : x(x2), y(y2) {}
		CFPoint (int32 x2, int32 y2) : x((float)x2), y((float)y2) {}

		// This ctor causes FVector2D-only expressions to fail, don't know why
		//CFPoint (const FVector2D& V) : x(V.X), y(V.Y) {}

		bool IsValid() const { return !(isnan(x) || isnan(y)); }

		CFPoint& operator = (const FVector2D& V) { Set(V.X, V.Y); return *this; }
		
		void  Set  (float a, float b) { x = a; y = b; }
		//void  Set  (double a, double b) { x = (float)a; y = (float)b; }
		//void  Set  (int32 a, int32 b) { x = (float)a; y = (float)b; }

		void SetRandomVector() { const float T = FRAND * TWO_PI; x = cosf(T); y = sinf(T); }

		void  Add  (const CFPoint& pt) { x += pt.x; y += pt.y; }
		void  Sub  (const CFPoint& pt) { x -= pt.x; y -= pt.y; }
		void  Mul  (float f)           { x *= f; y *= f; }
		void  Div  (float f)           { x /= f; y /= f; }
		void  Mul  (const CFPoint& pt) { x *= pt.x; y *= pt.y; }
		void  Div  (const CFPoint& pt) { x /= pt.x; y /= pt.y; }

		CFPoint& operator -= (const CFPoint& pt) { Sub(pt); return *this; }
		CFPoint& operator += (const CFPoint& pt) { Add(pt); return *this; }
		CFPoint& operator *= (float f)           { Mul(f);  return *this; }
		CFPoint& operator /= (float f)           { Div(f);  return *this; }
		CFPoint& operator *= (const CFPoint& pt) { Mul(pt); return *this; }
		CFPoint& operator /= (const CFPoint& pt) { Div(pt); return *this; }
		bool     operator == (const CFPoint& pt) const { return (x == pt.x && y == pt.y); }
		bool     operator != (const CFPoint& pt) const { return (!(*this == pt)); }

		void   MulAdd      (const CFPoint& pt, float f)	            { x += pt.x * f; y += pt.y * f; }
		void   MulAdd      (const CFPoint& pt, const CFPoint& off)	{ x += pt.x * off.x; y += pt.y * off.y; }
		void   Lerp        (const CFPoint& pt, float t)             { x = LERP(x, pt.x, t); y = LERP(y, pt.y, t); }
		void   Average     (const CFPoint& pt)                      { x = AVG(x, pt.x); y = AVG(y, pt.y); }

		// angle() returns degrees, negative if ccw and positive if cw. X-axis = 0 deg.
		float  Angle       () const                                 { return (float)RAD2DEG(atan2(y, x)); }
		float  AngleDelta  (const CFPoint& pt) const                { return Angle() - pt.Angle(); }
		float  Length      () const                                 { return (float)PTLEN(*this); }
		float  Distance    (const CFPoint& pt) const                { return (float)PTSLEN(*this, pt); }
		//float  Dot         (const CFPoint& v) const                 { return (x * v.x + y * v.y); }
		//bool   IsZero      () const                                 { return (x == 0 && y == 0); }
		//float  Ordinal     (float w) const                          { return y * w + x; }

		void   Normalize     ()                                     { Div(Length()); check(IsValid()); }
		void   NormalizeSafe ()                                     { Div(Length()); if(!IsValid()) { Set(0.0f, 0.0f); } } 

		// Like angle(), but returns 0-360 values.
		//float  PosAngle    () const                                 { const float a = Angle(); return (a >= 0.0f ? a : a + 360.0f); }

#if 0
		// Like angledelta(), but it always returns 
		// the positive smallest angular difference.
		float AngleDelta2(const CFPoint& pt) const
			{
				float a = AngleDelta(pt);

				a = ABS(a);

				if(a > 180)
				{
					a = 360.0f - a;
				}
				return a;
			}

		void Clamp(const CFPoint& lower, const CFPoint& upper)
			{
				x = FMath::Max(x, lower.x);
				x = FMath::Min(x, upper.x);
				y = FMath::Max(y, lower.y);
				y = FMath::Min(y, upper.y);
			}
#endif

		void Rotate(float dAng)
		{
			CFPoint pt;
			const double a = DEG2RAD(dAng);
			pt.x = x * cosf(a) - y * sinf(a);
			pt.y = x * sinf(a) + y * cosf(a);

			*this = pt;
		}


		void Rotate(CFPoint& pt, float dAng)
		{
			dAng = fmod(dAng, 360.0);

			if((dAng / 90.0) == (int32)dAng / 90)
			{
				// Right-angled rotation.
				if(dAng < 0)
				{
					dAng = 360.0 + dAng;
				}

				switch((int32)dAng)
				{
					case 0: 
					case 360: return;

					case 90:  pt.x = -y; pt.y = x;  break;
					case 180: pt.x = -x; pt.y = -y; break;
					case 270: pt.x = y;  pt.y = -x; break;
			
					default: check(false); break;
				}
				return;
			}
			
			
			const float a = DEG2RAD(dAng);

			pt.x = x * cosf(a) - y * sinf(a);
			pt.y = x * sinf(a) + y * cosf(a);
		}

		float	x, y;
}; // CFPoint


inline CFPoint operator - (const CFPoint& pt1, const CFPoint& pt2) { CFPoint r(pt1); r -= pt2; return r; }
inline CFPoint operator + (const CFPoint& pt1, const CFPoint& pt2) { CFPoint r(pt1); r += pt2; return r; }
inline CFPoint operator * (const CFPoint& pt1, float f)            { CFPoint r(pt1); r.Mul(f); return r; }
inline CFPoint operator / (const CFPoint& pt1, float f)            { CFPoint r(pt1); r.Div(f); return r; }


// A bastardized 2D matrix to store 2D orientations.
struct Orient2D
{
	CFPoint	Fwd;
	CFPoint	Up;

	bool IsValid() const { return (Fwd.IsValid() && Up.IsValid()); }
};


class CFRect
{
	// A floating-point Cartesian rectangle.

	public:

		CFRect () {}
		CFRect (float llx, float lly, float urx, float ury)    { Set(llx, lly, urx, ury); }
		CFRect (const CFPoint& pt)                             { Set(pt); }
		CFRect (const CFPoint& _LL, const CFPoint& _UR)        { Set(_LL, _UR); }

		bool operator == (const CFRect& r) const  { return (LL == r.LL && UR == r.UR); }
		bool operator != (const CFRect& r) const  { return (!(*this == r)); }

		void            Set         (float x, float y)                            { LL.Set(x, y); UR.Set(x, y); }
		void            Set         (float llx, float lly, float urx, float ury)  { LL.Set(llx, lly); UR.Set(urx, ury); }
		void            Set         (const CFPoint& _LL, const CFPoint& _UR)      { LL = _LL; UR = _UR; }
		void            Set         (const CFPoint& pt)                           { LL = UR = pt; }

		void            Inflate     (const CFPoint& pt)        { LL -= pt; UR += pt; }
		void            Deflate     (const CFPoint& pt)        { LL += pt; UR -= pt; }
		const CFPoint&  LowerLeft   () const                   { return LL; }
		const CFPoint&  UpperRight  () const                   { return UR; }
		void            Add         (const CFPoint& pt)        { LL += pt; UR += pt; }
		void            Sub         (const CFPoint& pt)        { LL -= pt; UR -= pt; }
		void            Mul         (float f)                  { LL *= f; UR *= f; }
		//bool            IsZero      () const                   { return (LL.IsZero() && UR.IsZero()); }
		float           Diagonal    () const                   { return LL.Distance(UR); }
		bool            IsEmpty     () const                   { return (Diagonal() == 0); }
		void            GetCenter   (CFPoint& pt) const        { pt = LL; pt.Average(UR); }
		float           GetWidth    () const                   { return ABS(UR.x - LL.x); }
		float           GetHeight   () const                   { return ABS(UR.y - LL.y); }
		bool            PtInside    (const CFPoint& pt) const  { return (pt.x >= LL.x && pt.x <= UR.x && pt.y >= LL.y && pt.y <= UR.y); }
		bool            Contains    (const CFRect& r) const    { CFRect t; Union(r, t); return (t == *this); }
		void            Order       ()                         { float t; ORDER(LL.x, UR.x, t) ORDER(LL.y, UR.y, t) }


		void Classicize(int32& left, int32& top, int32& right, int32& bottom) const
			{
				left   = SROUND(LL.x);
				top    = SROUND(LL.y);
				right  = SROUND(UR.x);
				bottom = SROUND(UR.y);
			}


		bool Intersect(const CFRect& r, CFRect* pResult = nullptr) const
			{
				if(UR.x < r.LL.x || UR.y < r.LL.y || LL.x > r.UR.x || LL.y > r.UR.y)
				{
					return false;
				}
				
				if(pResult != nullptr)
				{
					pResult->LL.x = FMath::Min(r.UR.x, FMath::Max(LL.x, r.LL.x));
					pResult->LL.y = FMath::Min(r.UR.y, FMath::Max(LL.y, r.LL.y));
					pResult->UR.x = FMath::Max(r.LL.x, FMath::Min(UR.x, r.UR.x));
					pResult->UR.y = FMath::Max(r.LL.y, FMath::Min(UR.y, r.UR.y));
				}
				return true;
			}


		void Union(const CFRect& r, CFRect& result) const
			{
				result.LL.x = FMath::Min(LL.x, r.LL.x);
				result.LL.y = FMath::Min(LL.y, r.LL.y);
				result.UR.x = FMath::Max(UR.x, r.UR.x);
				result.UR.y = FMath::Max(UR.y, r.UR.y);
			}


		void Lerp(const CFRect& r, CFRect& result, float t)
			{
				result.LL = LL; result.LL.Lerp(r.LL, t);
				result.UR = UR; result.UR.Lerp(r.UR, t);
			}

		CFPoint		LL, UR;
}; // CFRect


class CBezierSpline2D
{
	public:
		CFPoint	Pts[4];	// p1, a, b, p2

		void CalcPt(float t, CFPoint& r) const
		{
			const float tsub1     = t - 1.0f;
			const float tcubed    = t * t * t;
			const float invt      = 1.0f - t;
			const float t3        = t * 3;
			const float invtcubed = invt * invt * invt;
			const float coeff1    = t3 * tsub1 * tsub1;
			const float coeff2    = t3 * t * invt;

			r.Set(
				Pts[0].x * invtcubed +
				Pts[1].x * coeff1 +
				Pts[2].x * coeff2 +
				Pts[3].x * tcubed,

				Pts[0].y * invtcubed +
				Pts[1].y * coeff1 +
				Pts[2].y * coeff2 +
				Pts[3].y * tcubed
				);
		}
};
