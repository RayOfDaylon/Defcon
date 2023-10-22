// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/GeometryCore/Public/BoxTypes.h"


namespace Daylon
{
	extern const double Epsilon;

	DAYLONGRAPHICSLIBRARY_API FORCEINLINE double Square (double x) { return x * x; }

	DAYLONGRAPHICSLIBRARY_API FVector2D     AngleToVector2D                   (float Angle);
	DAYLONGRAPHICSLIBRARY_API FVector2D     RandVector2D                      ();
	DAYLONGRAPHICSLIBRARY_API FVector2D     Rotate                            (const FVector2D& P, float Angle); 
	DAYLONGRAPHICSLIBRARY_API FVector2D     DeviateVector                     (const FVector2D& VectorOld, float MinDeviation, float MaxDeviation);
	DAYLONGRAPHICSLIBRARY_API float         Vector2DToAngle                   (const FVector2D& Vector);
	DAYLONGRAPHICSLIBRARY_API float         WrapAngle                         (float Angle);
	DAYLONGRAPHICSLIBRARY_API float         Normalize                         (float N, float Min, float Max);

	DAYLONGRAPHICSLIBRARY_API bool          DoesPointIntersectCircle          (const FVector2D& P, const FVector2D& CP, double R);
	DAYLONGRAPHICSLIBRARY_API bool          DoesLineSegmentIntersectCircle    (const FVector2D& P1, const FVector2D& P2, const FVector2D& CP, double R);
	DAYLONGRAPHICSLIBRARY_API bool          DoesLineSegmentIntersectTriangle  (const FVector2D& P1, const FVector2D& P2, const FVector2D Triangle[3]);
	DAYLONGRAPHICSLIBRARY_API bool          DoTrianglesIntersect              (const FVector2D TriA[3], const FVector2D TriB[3]);
	DAYLONGRAPHICSLIBRARY_API bool          DoCirclesIntersect                (const FVector2D& C1, float R1, const FVector2D& C2, float R2);
	DAYLONGRAPHICSLIBRARY_API FVector2D     RandomPtWithinBox                 (const FBox2d& Box);
	DAYLONGRAPHICSLIBRARY_API FVector2D     ComputeFiringSolution             (const FVector2D& LaunchP, float TorpedoSpeed, const FVector2D& TargetP, const FVector2D& TargetInertia);
	DAYLONGRAPHICSLIBRARY_API void          ComputeCollisionInertia           (float Mass1, float Mass2, float Restitution,	const FVector2D& P1, const FVector2D& P2, FVector2D& Inertia1, FVector2D& Inertia2);
}
