// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#include "DaylonGeometry.h"
#include "DaylonRNG.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/GeometryCore/Public/Intersection/IntrTriangle2Triangle2.h"
#include "Runtime/GeometryCore/Public/MathUtil.h"


// Set to 1 to enable debugging
#define DEBUG_MODULE                0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif


const double Daylon::Epsilon = 1e-14;


FVector2D Daylon::AngleToVector2D(float Angle)
{
	// We place zero degrees pointing up and increasing clockwise.

	Angle = UKismetMathLibrary::DegreesToRadians(Angle - 90.0f);

	return FVector2D(UKismetMathLibrary::Cos(Angle), UKismetMathLibrary::Sin(Angle));
}


FVector2D Daylon::RandVector2D()
{
	FVector2D Result;
	FVector::FReal L;

	do
	{
		// Check random vectors in the unit sphere so result is statistically uniform.
		Result.X = Daylon::FRand() * 2.f - 1.f;
		Result.Y = Daylon::FRand() * 2.f - 1.f;
		L = Result.SizeSquared();
	} while (L > 1.0f || L < UE_KINDA_SMALL_NUMBER);

	return Result * (1.0f / FMath::Sqrt(L));
}


FVector2D Daylon::Rotate(const FVector2D& P, float Angle)
{
	// Rotate P around the origin for Angle degrees.

	const auto Theta = FMath::DegreesToRadians(Angle);

	return FVector2D(P.X * cos(Theta) - P.Y * sin(Theta), P.Y * cos(Theta) + P.X * sin(Theta));
}


float Daylon::Vector2DToAngle(const FVector2D& Vector)
{
	// We place zero degrees pointing up and increasing clockwise.

	return FMath::Atan2(Vector.Y, Vector.X) * 180 / PI + 90;
}


float Daylon::WrapAngle(float Angle)
{
	return (float)UKismetMathLibrary::GenericPercent_FloatFloat(Angle, 360.0);
}


FVector2D Daylon::DeviateVector(const FVector2D& VectorOld, float MinDeviation, float MaxDeviation)
{
	return Daylon::Rotate(VectorOld, Daylon::FRandRange(MinDeviation, MaxDeviation));
}


bool Daylon::DoCirclesIntersect(const FVector2D& C1, float R1, const FVector2D& C2, float R2)
{
	return (FVector2D::Distance(C1, C2) < (R1 + R2));
}


static UE::Geometry::TIntrTriangle2Triangle2<double> TriTriIntersector;


bool Daylon::DoesLineSegmentIntersectTriangle(const FVector2D& P1, const FVector2D& P2, const FVector2D Triangle[3])
{
	// Use a degenerate triangle to mimic the line segment.

	UE::Geometry::FTriangle2d Tri0(P1, P2, P2);
	TriTriIntersector.SetTriangle0(Tri0);

	UE::Geometry::FTriangle2d Tri1(Triangle);
	TriTriIntersector.SetTriangle1(Tri1);

	return TriTriIntersector.Test();
}


bool Daylon::DoTrianglesIntersect(const FVector2D TriA[3], const FVector2D TriB[3])
{
	UE::Geometry::FTriangle2d Tri0(TriA);
	TriTriIntersector.SetTriangle0(Tri0);

	UE::Geometry::FTriangle2d Tri1(TriB);
	TriTriIntersector.SetTriangle1(Tri1);

	return TriTriIntersector.Test();
}


bool Daylon::DoesPointIntersectCircle(const FVector2D& P, const FVector2D& CP, double R)
{
	return (FVector2D::Distance(P, CP) < R);
}


bool Daylon::DoesLineSegmentIntersectCircle(const FVector2D& P1, const FVector2D& P2, const FVector2D& CP, double R)
{
	// Test if a line intersects a circle.
	// Intersection is considered false only if both line endpoints are outside circle and 
	// no point on the line is inside the circle.
 
	// P1, P2 --> Line segment endpoints.
	// CP     --> Circle center.
	// R      --> Circle radius.

	if(DoesPointIntersectCircle(P1, CP, R) || DoesPointIntersectCircle(P2, CP, R))
	{
		return true;
	}

	// This code checks when the line segment endpoints lie outside the circle.

	if(P1 == P2)
	{
		return false;
	}

	const auto x0 = CP.X;
	const auto y0 = CP.Y;
	const auto x1 = P1.X;
	const auto y1 = P1.Y;
	const auto x2 = P2.X;
	const auto y2 = P2.Y;
	const auto A  = y2 - y1;
	const auto B  = x1 - x2;
	const auto C  = x2 * y1 - x1 * y2;
	const auto a  = Square(A) + Square(B);

	double b, c;

	bool bnz = true;

	if (abs(B) >= Epsilon)
	{
		b = 2 * (A * C + A * B * y0 - Square(B) * x0);
		c = Square(C) + 2 * B * C * y0 - Square(B) * (Square(R) - Square(x0) - Square(y0));
	}
	else
	{
		b = 2 * (B * C + A * B * x0 - Square(A) * y0);
		c = Square(C) + 2 * A * C * x0 - Square(A) * (Square(R) - Square(x0) - Square(y0));
		bnz = false;
	}

	auto d = Square(b) - 4 * a * c; // discriminant

	if (d < 0)
	{
		return false;
	}

	// Checks whether a point is within a segment
	auto within = [x1, y1, x2, y2](double x, double y)
	{
		auto d1 = sqrt(Square(x2 - x1) + Square(y2 - y1));  // distance between end-points
		auto d2 = sqrt(Square(x - x1) + Square(y - y1));    // distance from point to one end
		auto d3 = sqrt(Square(x2 - x) + Square(y2 - y));    // distance from point to other end
		auto delta = d1 - d2 - d3;

		return abs(delta) < Epsilon;                // true if delta is less than a small tolerance
	};

	auto fx = [A, B, C](double x) { return -(A * x + C) / B; };
	auto fy = [A, B, C](double y) { return -(B * y + C) / A; };

	bool res = false;

	double x, y;

	if (d == 0.0)
	{
		// Line is tangent to circle, so just one intersect at most

		if (bnz)
		{
			x = -b / (2 * a);
			y = fx(x);
			res = (within(x, y));
		}
		else
		{
			y = -b / (2 * a);
			x = fy(y);
			res = (within(x, y));
		}
	}
	else
	{
		// Two intersects at most

		d = sqrt(d);

		if (bnz)
		{
			x = (-b + d) / (2 * a);
			y = fx(x);
			if (within(x, y)) 
			{
				return true;
			}
			x = (-b - d) / (2 * a);
			y = fx(x);
			res = (within(x, y));
		}
		else
		{
			y = (-b + d) / (2 * a);
			x = fy(y);
			if (within(x, y)) 
			{
				return true;
			}
			y = (-b - d) / (2 * a);
			x = fy(y);
			res = (within(x, y));
		}
	}

	return res;
}


FVector2D Daylon::ComputeFiringSolution(const FVector2D& LaunchP, float TorpedoSpeed, const FVector2D& TargetP, const FVector2D& TargetInertia)
{
	// Given launch and target positions, torpedo speed, and target inertia, 
	// return a firing direction.

	const auto DeltaPos = TargetP - LaunchP;
	const auto DeltaVee = TargetInertia;// - ShooterInertia

	const auto A = DeltaVee.Dot(DeltaVee) - Square(TorpedoSpeed);
	const auto B = 2 * DeltaVee.Dot(DeltaPos);
	const auto C = DeltaPos.Dot(DeltaPos);

	const auto Desc = B * B - 4 * A * C;

	if(Desc <= 0)
	{
		return Daylon::RandVector2D();
	}

	const auto TimeToTarget = 2 * C / (FMath::Sqrt(Desc) - B);

	const auto TrueAimPoint = TargetP + TargetInertia * TimeToTarget;
	auto RelativeAimPoint = TrueAimPoint - LaunchP;

	RelativeAimPoint.Normalize();
	return RelativeAimPoint;

	//auto OffsetAimPoint = RelativeAimPoint - TimeToTarget * ShooterInertia;
	//return OffsetAimPoint.Normalize();
}


FVector2D Daylon::RandomPtWithinBox(const FBox2d& Box)
{
	return FVector2D(Daylon::FRandRange(Box.Min.X, Box.Max.X), Daylon::FRandRange(Box.Min.Y, Box.Max.Y));
}


float Daylon::Normalize(float N, float Min, float Max)
{
	// Return N normalized given Min/Max.
	// Note that result can be outside the 0..1 range if N is not inside Min..Max.

	check(Min != Max);

	return (N - Min) / (Max - Min);
}


void Daylon::ComputeCollisionInertia
(
	float            Mass1, 
	float            Mass2, 
	float            Restitution,
	const FVector2D& P1,
	const FVector2D& P2,
	FVector2D&       Inertia1,
	FVector2D&       Inertia2
)
{
	// Based on collision code courtesy of Thomas Smid from https://www.plasmaphysics.org.uk/programs/coll2d_cpp.htm

	check(Mass1 > 0.0f && Mass2 > 0.0f);
	check(Restitution >= 0.0f && Restitution <= 1.0f);
	
	auto            MassRatio    = Mass2 / Mass1;
	FVector2D       DeltaPos     = P2 - P1;
	const FVector2D DeltaInertia = Inertia2 - Inertia1; 


	// Return old inertias if masses are not approaching

	if((DeltaInertia.X * DeltaPos.X + DeltaInertia.Y * DeltaPos.Y) >= 0) 
	{
		return;
	}


	// Following statements avoid a zero divide
  
	const float F = TMathUtilConstants<float>::Epsilon * fabs(DeltaPos.Y);                            

	if ( fabs(DeltaPos.X) < F ) 
	{  
		DeltaPos.X = F;

		if(DeltaPos.X < 0)
		{
			DeltaPos.X *= -1;
		}
	} 

	// Update inertias

	const auto RatioDeltaAxes = DeltaPos.Y / DeltaPos.X;
	const auto Dvx2           = -2 * (DeltaInertia.X + RatioDeltaAxes * DeltaInertia.Y) / ((1 + RatioDeltaAxes * RatioDeltaAxes) * (1 + MassRatio));

	Inertia2 += FVector2D(Dvx2, Dvx2 * RatioDeltaAxes);
	Inertia1 -= FVector2D(Dvx2 * MassRatio, Dvx2 * RatioDeltaAxes * MassRatio);

	// Velocity correction for inelastic collisions
	const auto TotalMass = Mass1 + Mass2;
	const FVector2D InertiaOfTotalMass = (Inertia1 * Mass1 + Inertia2 * Mass2) / TotalMass;
	
	Inertia1 = (Inertia1 - InertiaOfTotalMass) * Restitution + InertiaOfTotalMass;
	Inertia2 = (Inertia2 - InertiaOfTotalMass) * Restitution + InertiaOfTotalMass;
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
