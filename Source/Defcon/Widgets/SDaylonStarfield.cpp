// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.

#include "SDaylonStarfield.h"
#include "Common/util_color.h"
#include "Common/util_geom.h"
#include "Common/util_core.h"
#include "DaylonUtils.h"


#pragma optimize("", off)

constexpr float kBackplaneMax_ = 10.0f;
constexpr float kBackplaneMin_ = 3.0f;
constexpr float kForeplane_	   = 0.1f;
constexpr float kPlanedist_	   = kBackplaneMax_ - kForeplane_;
constexpr float kSpeed_		   = 0.25f;
constexpr float kSpeedRot_	   = 3.0f;
constexpr float fRange_        = 7.0f; // normally a param




void SDaylonStarfield::Construct(const FArguments& InArgs)
{
	Size  = InArgs._Size.Get();
	Count = InArgs._Count.Get();
	Brush = InArgs._Brush.Get();

	Colors = 
	{
		C_WHITE, C_WHITE, C_WHITE, C_WHITE,	C_WHITE, C_WHITE, C_WHITE,
		C_LIGHTERBLUE, C_LIGHTYELLOW, C_LIGHTYELLOW, C_YELLOW,
		C_ORANGE, C_CYAN, C_RED, C_RED, C_ORANGE
	};

	Reset();
}


void SDaylonStarfield::Reset()
{
	//Rng.seed();

	Age = 0.0f;

	Stars.Empty();
	Stars.Reserve(Count);

	for(int32 Index = 0; Index < Count; Index++)
	{
		FDaylonStarParticle Star;

		Star.P.Set(Daylon::FRandRange(-4000.0f, 4000.0f), Daylon::FRandRange(-4000.0f, 4000.0f), Daylon::FRandRange(kBackplaneMin_, kBackplaneMax_));
		Star.ColorFar = Colors[Daylon::RandRange(0, Colors.Num() - 1)];
		
		/*if(m_bMorphColor)
		{
			m_stars[i].m_colorNear = nearColor;
		}*/

		Stars.Add(Star);
	}
}


void SDaylonStarfield::SetCount   (int32 InCount)
{
	Count = InCount;
}


void SDaylonStarfield::SetSize    (const FVector2D& InSize)
{
	Size = InSize;
}


void SDaylonStarfield::SetBrush   (const FSlateBrush& InBrush)
{
	Brush = InBrush;
}


void SDaylonStarfield::SetSpeed(float Speed)
{
	SpeedMul = Speed;
}


void SDaylonStarfield::SetRotationSpeed(float Speed)
{
	SpeedRotMul = Speed;
}

void SDaylonStarfield::SetAge            (float N) { Age = N; }
void SDaylonStarfield::SetFadeStartsAt   (float N) { FadeStartsAt = N; }
void SDaylonStarfield::SetFadeEndsAt     (float N) { FadeEndsAt = N; }
void SDaylonStarfield::SetOsStart        (float N) { OsStart = N; }
void SDaylonStarfield::SetOsEnd          (float N) { OsEnd = N; }
void SDaylonStarfield::SetColors         (const TArray<FLinearColor>& InColors) { Colors = InColors; Reset(); }


bool SDaylonStarfield::Update(float DeltaTime)
{
	Age += DeltaTime;

	// Bring the stars closer.

	for(auto& Star : Stars)
	{
		// Bring star closer to camera
		Star.P.Z -= (DeltaTime * kSpeed_ * SpeedMul);

		// If too close, wrap it back.
		if(Star.P.Z < kForeplane_)
		{
			Star.P.Set(Daylon::FRandRange(-3000.0f, 3000.0f), Daylon::FRandRange(-3000.0f, 3000.0f), Daylon::FRandRange(kBackplaneMin_, kBackplaneMax_));
		}
	}

	Angle += (DeltaTime * kSpeedRot_ * SpeedRotMul);
	Angle = (float)fmod(Angle, 360.0f);

	return true;
}


FVector2D SDaylonStarfield::ComputeDesiredSize(float) const
{
	return Size;
}


int32 SDaylonStarfield::OnPaint
(
	const FPaintArgs&          Args,
	const FGeometry&           AllottedGeometry,
	const FSlateRect&          MyCullingRect,
	FSlateWindowElementList&   OutDrawElements,
	int32                      LayerId,
	const FWidgetStyle&        InWidgetStyle,
	bool                       bParentEnabled
) const
{
	if(!IsValid(Brush.GetResourceObject()))
	{
		return LayerId;
	}


	CFRect rect(0, 0, Size.X, Size.Y);

	CFPoint scale(1.0f, (float)Size.Y / Size.X);

	CFPoint inc(1,1);

	float T = NORM_(Age, FadeStartsAt, FadeEndsAt);
	T = CLAMP(T, 0.0f, 1.0f);

	const float Os = LERP(OsStart, OsEnd, T);

	for(auto& Star : Stars)
	{
		// Project 3D star location to 2D.
		CFPoint starproj;

		starproj.set(Star.P.X / Star.P.Z, Star.P.Y / Star.P.Z);

		starproj.mul(scale);
		starproj.rotate(Angle);

		float c;

		if(rect.ptinside(starproj))
		{
			CFRect r(starproj);
			r.inflate(inc);
			c = 0.25f;//C_DARKER;


			if(Star.P.Z < 4.0f)
			{
				r.UR += inc;
				c = Daylon::RandBool() ? 0.5f/*C_DARK*/ : 0.75f/*C_LIGHT*/;
			}

			if(Star.P.Z < 2.0f)
			{
				r.UR += inc;
				c = Daylon::RandBool() ? 0.875f/*C_BRIGHT*/ : 1.0f/*C_WHITE*/;
			}

			if(bFat && Daylon::RandBool())
			{
				r.UR += inc;
			}
			
#if 0
			int x1, y1, x2, y2;
			r.order();
			r.classicize(x1, y1, x2, y2);
#endif

			//CMaskMap& mask = gBitmaps.GetMask(ROUND(r.LL.distance(r.UR)), 0);
			//FLinearColor cf = MakeBlendedColor(C_BLACK, m_stars[i].m_colorFar, c);

			/*if(m_bMorphColor && m_stars[i].m_pos.z <= 3.0f)
				cf = MakeBlendedColor(m_stars[i].m_colorNear, cf, m_stars[i].m_pos.z / 3.0f);*/

			//mask.ColorWith(cf, framebuf, ROUND(starproj.x), ROUND(starproj.y));
		}

		float TSize = NORM_(Star.P.Z, kBackplaneMax_, kForeplane_);
		TSize = CLAMP(TSize, 0.0f, 1.0f);

		FVector2D StarSize(LERP(2.0f, 9.0f, TSize));
		FVector2D StarProj(starproj.x, starproj.y);

		const FPaintGeometry PaintGeometry(
			AllottedGeometry.GetAbsolutePosition() + AllottedGeometry.GetAbsoluteSize() / 2  + (StarProj * AllottedGeometry.Scale) + 0.5f, 
			StarSize * AllottedGeometry.Scale,
			1.0f);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			PaintGeometry,
			&Brush,
			ESlateDrawEffect::None,
			Brush.TintColor.GetSpecifiedColor() * Star.ColorFar * Os * RenderOpacity * InWidgetStyle.GetColorAndOpacityTint().A);
	}

	return LayerId;
}

#pragma optimize("", on)
