// Copyright 2023 Daylon Graphics Ltd. All rights reserved.


#include "DaylonUtils.h"

#include "Runtime/GeometryCore/Public/Intersection/IntrTriangle2Triangle2.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"
#include "UMG/Public/Components/TextBlock.h"
#include "UMG/Public/Blueprint/SlateBlueprintLibrary.h"
#include "Algo/Reverse.h"

//DECLARE_LOG_CATEGORY_EXTERN(LogDaylon, Log, All);

DEFINE_LOG_CATEGORY(LogDaylon);


const double Daylon::Epsilon = 1e-14;

UWidgetTree*  Daylon::WidgetTree = nullptr;
UCanvasPanel* Daylon::RootCanvas = nullptr;


void          Daylon::SetWidgetTree   (UWidgetTree* InWidgetTree) { WidgetTree = InWidgetTree; }
UWidgetTree*  Daylon::GetWidgetTree   () { return WidgetTree; }
void          Daylon::SetRootCanvas   (UCanvasPanel* InCanvas) { RootCanvas = InCanvas; }
UCanvasPanel* Daylon::GetRootCanvas   () { return RootCanvas; }


float Daylon::Normalize(float N, float Min, float Max)
{
	// Return N normalized given Min/Max.
	// Note that result can be outside the 0..1 range if N is not inside Min..Max.

	check(Min != Max);

	return (N - Min) / (Max - Min);
}


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


bool Daylon::DoesLineSegmentIntersectCircle(const FVector2D& P1, const FVector2D& P2, const FVector2D& CP, double R)
{
	// Test if a line intersects a circle.
	// p1 and p2 are the line endpoints.
	// cp is the circle center.
	// r is the circle radius.

	if(P1 == P2)
	{
		return (FVector2D::Distance(P1, CP) < R);
	}

	const auto x0 = CP.X;
	const auto y0 = CP.Y;
	const auto x1 = P1.X;
	const auto y1 = P1.Y;
	const auto x2 = P2.X;
	const auto y2 = P2.Y;
	const auto A = y2 - y1;
	const auto B = x1 - x2;
	const auto C = x2 * y1 - x1 * y2;
	const auto a = Square(A) + Square(B);

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
			if (within(x, y)) return true;
			x = (-b - d) / (2 * a);
			y = fx(x);
			res = (within(x, y));
		}
		else
		{
			y = (-b + d) / (2 * a);
			x = fy(y);
			if (within(x, y)) return true;
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


FVector2D Daylon::GetWidgetDirectionVector(const UWidget* Widget)
{
	if (Widget == nullptr)
	{
		return FVector2D(0.0f);
	}

	return Daylon::AngleToVector2D(Widget->GetRenderTransformAngle());
}


void Daylon::Show(UWidget* Widget, bool Visible)
{
	if(Widget == nullptr)
	{
		return;
	}

	Widget->SetVisibility(Visible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}


void Daylon::Hide(UWidget* Widget) 
{
	Show(Widget, false); 
}


void Daylon::Show(SWidget* Widget, bool Visible)
{
	if(Widget == nullptr)
	{
		return;
	}

	Widget->SetVisibility(Visible ? EVisibility::HitTestInvisible : EVisibility::Collapsed);
}


void Daylon::Hide(SWidget* Widget) 
{
	Show(Widget, false); 
}


void Daylon::UpdateRoundedReadout(UTextBlock* Readout, float Value, int32& OldValue)
{
	check(Readout);

	const int32 N = FMath::RoundToInt(Value);

	if(N != OldValue)
	{
		const auto Str = FString::Printf(TEXT("%d"), N);
		Readout->SetText(FText::FromString(Str));
		OldValue = N;
	}
}


FVector2D Daylon::GetWidgetPosition(const UWidget* Widget)
{
	// Return a UWidget's position in layout (not screen) space. E.g. an HD canvas on a 4K screen will return HD units
#if 1
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport(const_cast<UWidget*>(Widget), Widget->GetCachedGeometry(), FVector2D(0), PixelPosition, ViewportPosition);
	return ViewportPosition;
#else
	const auto& Geometry = Widget->GetPaintSpaceGeometry();

	return Geometry.GetAbsolutePosition() / Geometry.Scale;
#endif
}


FVector2D Daylon::GetWidgetSize(const UWidget* Widget)
{
	// Return a UWidget's size in layout (not screen) space. E.g. an HD canvas on a 4K screen will return HD units

	const auto& Geometry = Widget->GetPaintSpaceGeometry();

	return Geometry.GetAbsoluteSize() / Geometry.Scale;
}

// ------------------------------------------------------------------------------------------

Daylon::MTRand Daylon::Rng;

double Daylon::FRand      () { return Rng.rand(); }
double Daylon::FRandRange (double Min, double Max) { return (Min + Rng.rand(Max - Min)); }
int32  Daylon::RandRange  (int32  Min, int32  Max) { return (Min + Rng.randInt(Max - Min)); }
bool   Daylon::RandBool   () { return (Rng.rand() >= 0.5); }

// ------------------------------------------------------------------------------------------

void Daylon::FLoopedSound::Start(float InVolumeScale)
{
	VolumeScale2 = InVolumeScale;
	TimeRemaining = 0.0f;
	Tick(0.0f);
}


void Daylon::FLoopedSound::Tick(float DeltaTime)
{
	TimeRemaining -= DeltaTime;

	if(Sound == nullptr || WorldContextPtr == nullptr)
	{
		//UE_LOG(LogDaylon, Error, TEXT("Looped sound has no sound or context"));
		return;
	}

	if(TimeRemaining <= 0.0f)
	{
		TimeRemaining = Sound->GetDuration();

		UGameplayStatics::PlaySound2D(WorldContextPtr, Sound, VolumeScale * VolumeScale2);
	}
}


// ------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------

TSharedPtr<Daylon::ImagePlayObject2D> Daylon::SpawnImagePlayObject2D(const FSlateBrush& Brush, float Radius)
{
	auto PlayObj = SNew(ImagePlayObject2D);

	Daylon::Install<SImage>(PlayObj, Radius);

	PlayObj->SetBrush(Brush);

	return PlayObj;
}


TSharedPtr<Daylon::SpritePlayObject2D> Daylon::SpawnSpritePlayObject2D(const FDaylonSpriteAtlas& Atlas, const FVector2D& S, float Radius)
{
	auto PlayObj = SNew(SpritePlayObject2D);

	Daylon::Install<SDaylonSprite>(PlayObj, Radius);

	PlayObj->SetAtlas(Atlas);
	PlayObj->SetSize(S);
	PlayObj->UpdateWidgetSize();

	return PlayObj;
}


void Daylon::UninstallImpl(TSharedPtr<SWidget> Widget)
{
	Daylon::GetRootCanvas()->GetCanvasWidget()->RemoveSlot(Widget.ToSharedRef());
}

#define UNINSTALL_PLAYOBJECT(_Widget)	\
	if(!_Widget->IsValid())	\
	{	\
		UE_LOG(LogDaylon, Error, TEXT("Daylon::Destroy() tried to uninstall an invalid Slate widget!"));	\
		return;	\
	}	\
	_Widget->ClearSlot();	\
	UninstallImpl(StaticCastSharedPtr<SWidget>(Widget));


void Daylon::Uninstall(TSharedPtr<Daylon::ImagePlayObject2D> Widget)
{
	UNINSTALL_PLAYOBJECT(Widget);
}


void Daylon::Uninstall(TSharedPtr<SpritePlayObject2D> Widget)
{
	UNINSTALL_PLAYOBJECT(Widget);
}
