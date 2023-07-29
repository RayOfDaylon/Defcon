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

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif


#define FEPSILON	((float)1e-5)
#define DEPSILON	1e-12


#ifndef PI
#define PI	3.1415926535897932384626433832795
#endif

#ifndef TWO_PI
#define TWO_PI	6.283185307179586476925286766559
#endif

#ifndef HALF_PI
#define HALF_PI	1.5707963267948966192313216916398
#endif

#define M_PI_360	0.00872664625997164788

#define D2R	0.017453292
#define R2D 57.29578121996
#define DEG2RAD(d)	((d) * D2R)
#define RAD2DEG(r)	((r) * R2D)
#define SQUARED(a) ((a)*(a))

// Common 2D vector macros.
#define DDISTANCE(dx, dy)	\
	(sqrt(SQUARED(dx) + SQUARED(dy)))

#define DISTANCE(x1, y1, x2, y2)	\
	DDISTANCE((x1)-(x2), (y1)-(y2))

#define PTLEN(pt)	\
	DDISTANCE((pt).x, (pt).y)

#define PTSLEN(pt1, pt2)	\
	DISTANCE((pt1).x, (pt1).y, (pt2).x, (pt2).y)

struct IntRect
{
	int32 left, right, top, bottom;

	int32 width  () const { return right - left; }
	int32 height () const { return bottom - top; }
};


class CFPoint
{
	public:
		CFPoint() : x(0.0f), y(0.0f) {}
		CFPoint(float x2, float y2) : x(x2), y(y2) {}
		CFPoint(int32 x2, int32 y2) : x((float)x2), y((float)y2) {}
		
		void  set  (float a, float b) { x = a; y = b; }

		void  add  (const CFPoint& pt) { x += pt.x; y += pt.y; }
		void  sub  (const CFPoint& pt) { x -= pt.x; y -= pt.y; }
		void  mul  (float f)           { x *= f; y *= f; }
		void  div  (float f)           { x /= f; y /= f; }
		void  mul  (const CFPoint& pt) { x *= pt.x; y *= pt.y; }
		void  div  (const CFPoint& pt) { x /= pt.x; y /= pt.y; }

		CFPoint& operator -= (const CFPoint& pt) { this->sub(pt); return *this; }
		CFPoint& operator += (const CFPoint& pt) { this->add(pt); return *this; }
		CFPoint& operator *= (float f)           { this->mul(f);  return *this; }
		CFPoint& operator /= (float f)           { this->div(f);  return *this; }
		CFPoint& operator *= (const CFPoint& pt) { this->mul(pt); return *this; }
		CFPoint& operator /= (const CFPoint& pt) { this->div(pt); return *this; }
		bool     operator == (const CFPoint& pt) const { return (x == pt.x && y == pt.y); }
		bool     operator != (const CFPoint& pt) const { return (!(*this == pt)); }

		void   muladd      (const CFPoint& pt, float f)	            { x += pt.x * f; y += pt.y * f; }
		void   muladd      (const CFPoint& pt, const CFPoint& off)	{ x += pt.x * off.x; y += pt.y * off.y; }
		float  dot         (const CFPoint& v)                       { return (x * v.x + y * v.y); }
		float  angledelta  (const CFPoint& pt)                      { return this->angle() - pt.angle(); }
		float  length      () const                                 { return (float)PTLEN(*this); }
		void   normalize   ()                                       { this->div(this->length()); }
		float  distance    (const CFPoint& pt) const                { return (float)PTSLEN(*this, pt); }
		void   lerp        (const CFPoint& pt, float t)             { x = LERP(x, pt.x, t); y = LERP(y, pt.y, t); }
		void   avg         (const CFPoint& pt)                      { x = AVG(x, pt.x); y = AVG(y, pt.y); }
		bool   iszero      () const                                 { return (x == 0 && y == 0); }
		float  ordinal     (float w) const                          { return y * w + x; }

		// angle() returns degrees, negative if ccw 
		// and positive if cw. X-axis = 0 deg.
		float  angle       () const                                 { return (float)RAD2DEG(atan2(y, x)); }

		// Like angle(), but returns 0-360 values.
		float posangle() const
			{
				float a = this->angle();
				if(a < 0)
					a += 360.0f;
				return a;
			}

		// Like angledelta(), but it always returns 
		// the positive smallest angular difference.
		float angledelta2(const CFPoint& pt)
			{
				float a = this->angledelta(pt);
				a = ABS(a);
				if(a > 180)
					a = 360.0f - a; 
				return a;
			}

		void clamp(const CFPoint& lower, const CFPoint& upper)
			{
				x = FMath::Max(x, lower.x);
				x = FMath::Min(x, upper.x);
				y = FMath::Max(y, lower.y);
				y = FMath::Min(y, upper.y);
			}

		void rotate(double dAng)
		{
			CFPoint pt;
			double a = DEG2RAD(dAng);
			pt.x = x * (float)cos(a) - y * (float)sin(a);
			pt.y = x * (float)sin(a) + y * (float)cos(a);
			*this = pt;
		}


		void rotate(CFPoint& pt, double dAng)
		{
			dAng = fmod(dAng, 360.0);

			if((dAng / 90.0) == (int)dAng/90)
			{
				// Right-angled rotation.
				if(dAng < 0)
				{
					dAng = 360.0 + dAng;
				}

				switch((int)dAng)
				{
					case 0: 
					case 360: return;

					case 90:  pt.x = -y; pt.y = x; break;
					case 180: pt.x = -x; pt.y = -y; break;
					case 270: pt.x = y; pt.y = -x; break;
					default: check(false); break;
				}
			}
			else
			{
				double a = DEG2RAD(dAng);
				pt.x = x * (float)cos(a) - y * (float)sin(a);
				pt.y = x * (float)sin(a) + y * (float)cos(a);
			}
		}

		float	x, y;
}; // CFPoint


inline CFPoint operator - (const CFPoint& pt1, const CFPoint& pt2) { CFPoint r(pt1); r -= pt2; return r; }
inline CFPoint operator + (const CFPoint& pt1, const CFPoint& pt2) { CFPoint r(pt1); r += pt2; return r; }
inline CFPoint operator * (const CFPoint& pt1, float f)            { CFPoint r(pt1); r.mul(f); return r; }
inline CFPoint operator / (const CFPoint& pt1, float f)            { CFPoint r(pt1); r.div(f); return r; }


class CFRect
{
	// A floating-point Cartesian rectangle.

	public:
		CFRect() {}

		CFRect(float llx, float lly, float urx, float ury)	  { this->set(llx, lly, urx, ury); }
		CFRect(const CFPoint& pt)			                  { this->set(pt); }
		CFRect(const CFPoint& _LL, const CFPoint& _UR)		  { this->set(_LL, _UR); }

		void set(float x, float y)			                  { LL.set(x, y); UR.set(x, y); }
		void set(float llx, float lly, float urx, float ury)  { LL.set(llx, lly); UR.set(urx, ury); }
		void set(const CFPoint& _LL, const CFPoint& _UR)	  { LL = _LL; UR = _UR; }
		void set(const CFPoint& pt)			                  { LL = UR = pt; }
		void inflate(const CFPoint& pt)			              { LL -= pt; UR += pt; }
		void deflate(const CFPoint& pt)			              { LL += pt; UR -= pt; }

		bool ptinside(const CFPoint& pt) const
			{
				return (
					pt.x >= LL.x && 
					pt.x <= UR.x &&
					pt.y >= LL.y && 
					pt.y <= UR.y);
			}

		const CFPoint& lowerleft() const	{ return LL; }
		const CFPoint& upperright() const	{ return UR; }
		void add(const CFPoint& pt)			{ LL += pt; UR += pt; }
		void sub(const CFPoint& pt)			{ LL -= pt; UR -= pt; }
		void mul(float f)					{ LL *= f; UR *= f; }

		bool intersect(const CFRect& r, CFRect* pResult = nullptr) const
			{
				if(UR.x < r.LL.x || UR.y < r.LL.y || LL.x > r.UR.x || LL.y > r.UR.y)
					return false;
				
				if(pResult != nullptr)
				{
					pResult->LL.x = FMath::Min(r.UR.x, FMath::Max(LL.x, r.LL.x));
					pResult->LL.y = FMath::Min(r.UR.y, FMath::Max(LL.y, r.LL.y));
					pResult->UR.x = FMath::Max(r.LL.x, FMath::Min(UR.x, r.UR.x));
					pResult->UR.y = FMath::Max(r.LL.y, FMath::Min(UR.y, r.UR.y));
				}
				return true;
			}

		void _union(const CFRect& r, CFRect& result) const
			{
				result.LL.x = FMath::Min(LL.x, r.LL.x);
				result.LL.y = FMath::Min(LL.y, r.LL.y);
				result.UR.x = FMath::Max(UR.x, r.UR.x);
				result.UR.y = FMath::Max(UR.y, r.UR.y);
			}

		bool contains(const CFRect& r) const
			{
				CFRect t;
				this->_union(r, t);
				return (t == *this);
			}

		bool operator==(const CFRect& r) const	{ return (LL == r.LL && UR == r.UR); }
		bool operator!=(const CFRect& r) const	{ return (!(*this == r)); }

		bool iszero() const 					{ return (LL.iszero() && UR.iszero()); }
		float diagonal() const					{ return LL.distance(UR); }
		bool isempty() const					{ return (this->diagonal() == 0); }
		void getcenter(CFPoint& pt) const		{ pt = LL; pt.avg(UR); }
		float getwidth() const					{ return ABS(UR.x - LL.x); }
		float getheight() const					{ return ABS(UR.y - LL.y); }

		void lerp(const CFRect& r, CFRect& result, float t)
			{
				result.LL = LL; result.LL.lerp(r.LL, t);
				result.UR = UR; result.UR.lerp(r.UR, t);
			}

		void order()
			{
				float t;
				ORDER(LL.x, UR.x, t)
				ORDER(LL.y, UR.y, t)
			}

		void classicize(int& left, int& top, int& right, int& bottom) const
			{
				left = SROUND(LL.x);
				top = SROUND(LL.y);
				right = SROUND(UR.x);
				bottom = SROUND(UR.y);
			}

		CFPoint		LL, UR;
}; // CFRect



typedef struct
{
	float x, y, z;
} FVector_;


// Floating-point vector class. Should be a template, ideally.
class CFVector : public FVector_
{
public:
	CFVector() { x = y = z = 0; }
	CFVector(const CFVector& v) { *this = v; }
	CFVector(const FVector_& v) { x = v.x; y = v.y; z = v.z; }
	CFVector(const FVector_* pv) { x = pv->x; y = pv->y; z = pv->z; }
	CFVector(float a, float b, float c)	{ x = a; y = b; z = c; }
	virtual ~CFVector() {}

	operator FVector_*() { return this; }
	operator const FVector_*() const { return this; }

	void mul(const float f) { x *= f; y *= f; z *= f; }
	void set(float tx, float ty, float tz) { x = tx; y = ty; z = tz; }

}; // CFVector


// A bastardized 2D matrix to store 2D orientations.
typedef struct
{
	CFPoint	fwd;
	CFPoint	up;
} Orient2D;


class CBezierSpline2D
{
	public:
		CFPoint	m_pt[4];	// p1, a, b, p2

		void CalcPt(float t, CFPoint& r) const
		{
			const float tsub1     = t - 1.0f;
			const float tcubed    = t * t * t;
			const float invt      = 1.0f - t;
			const float t3        = t * 3;
			const float invtcubed = invt * invt * invt;
			const float coeff1    = t3 * tsub1 * tsub1;
			const float coeff2    = t3 * t * invt;

			r.set(
				m_pt[0].x * invtcubed +
				m_pt[1].x * coeff1 +
				m_pt[2].x * coeff2 +
				m_pt[3].x * tcubed,

				m_pt[0].y * invtcubed +
				m_pt[1].y * coeff1 +
				m_pt[2].y * coeff2 +
				m_pt[3].y * tcubed
				);
		}
}; // CBezierSpline2D


