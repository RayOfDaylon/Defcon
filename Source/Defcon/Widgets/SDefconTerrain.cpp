// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2023 Daylon Graphics Ltd. All Rights Reserved.


#include "SDefconTerrain.h"
#include "Common/util_color.h"
#include "Common/util_geom.h"
#include "Common/util_core.h"
#include "Main/mapper.h"
#include "DaylonUtils.h"


#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



void SDefconTerrain::Construct(const FArguments& InArgs)
{
	Size     = InArgs._Size.Get();
	Vertices = nullptr;
	Mapper   = nullptr;
}


void SDefconTerrain::Init(const TArray<FVector2D>* InVertices, const Defcon::I2DCoordMapper* InMapper)
{
	check(InVertices != nullptr && InMapper != nullptr);

	Vertices = InVertices;
	Mapper   = InMapper;
}


void SDefconTerrain::SetSize(const FVector2D& InSize)
{
	Size = InSize;
}


FVector2D SDefconTerrain::ComputeDesiredSize(float) const
{
	return Size;
}


int32 SDefconTerrain::OnPaint
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
	if(Vertices == nullptr)
	{
		return LayerId;
	}

	const float PaintWidth = AllottedGeometry.GetAbsoluteSize().X / AllottedGeometry.Scale;

	CFPoint Pt;

	const int32 NumVertices = Vertices->Num();
	const auto& Verts = *Vertices;

	int32 J = 0, K = 0;
	float Xmin = 1.0e+10f, Xmax = -1.0e+10f;

	// Find the visible part of the terrain.

	// Find the first visible terrain part on the left side.
	for(int32 I = 0; I < NumVertices; I++)
	{
		CFPoint PtTemp((float)Verts[I].X, (float)Verts[I].Y);
		Mapper->To(PtTemp, Pt);

		if(Pt.x >= 0 && Pt.x < PaintWidth)
		{
			if(Pt.x < Xmin)
			{
				Xmin = Pt.x;
				J = I;
			}

			if(Pt.x > Xmax)
			{
				Xmax = Pt.x;
				K = I;
			}
		}
	}

	// todo: cache j, k and perform above calc only when invalidated.
	J -= 2;
	K++;

	if(K < J)
	{
		K += NumVertices;
	}

	TArray<FVector2f>  LinePts;

	LinePts.Reserve(K - J + 1);

	for(int32 I = J; I <= K; I++)
	{
		const int32 M = (I + NumVertices) % NumVertices;

		CFPoint PtTemp((float)Verts[M].X, (float)Verts[M].Y);

		Mapper->To(PtTemp, Pt);

		LinePts.Add(FVector2f(Pt.x, Pt.y));
	}

	FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), LinePts, ESlateDrawEffect::None, C_RED, true, 3.0f);

	return LayerId;
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
