// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	terrain.cpp
	Terrain for Defcon game.
*/


#include "terrain.h"

#include "Common/util_core.h"
#include "Common/Painter.h"
#include "Common/util_geom.h"
#include "Globals/GameColors.h"
#include "Globals/prefs.h"
#include "Main/mapper.h"
#include "DaylonUtils.h"
#include "DefconLogging.h"
#include "Runtime/SlateCore/Public/Rendering/DrawElements.h"



#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



#ifdef SHOW_STATS
	extern bool gbShowStats;
#endif

const float MinTerrainLineLength =  40.0f;
const float MaxTerrainLineLength = 120.0f;


Defcon::CTerrain::CTerrain()
{
	Age           = 0.0f;
	bMortal       = false;
	bCanBeInjured = false;
	ArenaWidth    = ArenaHeight = 0;
}

Defcon::CTerrain::~CTerrain()
{
}


#ifdef _DEBUG
const char* Defcon::CTerrain::GetClassname() const
{
	static char* psz = "Terrain";
	return psz;
}
#endif


void Defcon::CTerrain::InitTerrain(float w, float h)
{
	Vertices.Reserve(300);
	Vertices.Empty();

	ArenaWidth  = w;
	ArenaHeight = h;


	// Vertex generator

	const float MaxHeight = ArenaHeight * 0.45f;
	const float MinHeight = ArenaHeight * 0.05f;
	
	FVector2D P(0.0f, Daylon::FRandRange(MinHeight, MaxHeight)); // current point. Start at x = 0
	Vertices.Add(P);
	
	// Walk until current point is still inside arena.

	while(P.X < w) // Note: we need first and last vertex to join at a 0 or 45 degree angle.
	{
		FVector2D P2;

		int32 DirectionY;

		do
		{
			DirectionY = Daylon::RandRange(-1, 1);

			FVector2D V(1.0f, (float)DirectionY);
			V.Normalize();

			const float Length = Daylon::FRandRange(MinTerrainLineLength, MaxTerrainLineLength);
			P2 = P + V * Length;

			// Keep generating current point until it is within allowed vertical range
		}
		while(P2.Y < MinHeight || P2.Y > MaxHeight);

		P = P2;
		Vertices.Add(P);
	}

	// Collapse multiple segments into one where possible e.g. they have the same direction.
	// We can treat this as vertex removal where a vertex connects two lines having the same slope.

	const int32 OldVertexCount = Vertices.Num();

	int32 CurrentVertexIdx = 1;

	while(CurrentVertexIdx < Vertices.Num() - 1)
	{
		const int32 SlopeBehind = SGN(Vertices[CurrentVertexIdx - 1].Y - Vertices[CurrentVertexIdx].Y);
		const int32 SlopeAhead  = SGN(Vertices[CurrentVertexIdx].Y     - Vertices[CurrentVertexIdx + 1].Y);

		if(SlopeBehind == SlopeAhead)
		{
			Vertices.RemoveAt(CurrentVertexIdx);
			continue;
		}

		CurrentVertexIdx++;
	}

	UE_LOG(LogGame, Log, TEXT("%d terrain vertices optimized away, %.2f%% savings"), OldVertexCount - Vertices.Num(), 100 * (OldVertexCount - Vertices.Num()) / (float)OldVertexCount);


	// The last vertex will have an X >= w.
	// We need to replace it with the first vertex.
	// This won't give us the preferred 0 or 45 degree slope, but for now it's better than a rough join.
	Vertices.Last(0) = Vertices[0];

	// Render lines into heightfield for fast access during GetElev().
	// Note: line vertices are in Slate space, but our elevations are cartesian.

	const int32 numElevs = (int32)ArenaWidth + 1;
	Elevations.Reserve(numElevs);
	Elevations.SetNum(numElevs);

	for(int32 Index = 0; Index < Vertices.Num() - 1; Index++)
	{
		const FVector2D P1 = Vertices[Index];
		const FVector2D P2 = Vertices[Index + 1];

		// Iterate through line at pixel res.
		for(int32 X = (int32)P1.X; X <= (int32)P2.X; X++)
		{
			Elevations[X] = FMath::Lerp(P1.Y, P2.Y, Daylon::Normalize(X, P1.X, P2.X));
		}
	}
	// todo: we probably have gaps in Elevations, need to clean that up.
}


float Defcon::CTerrain::GetElev(float X) const
{
	// x is normalized.
	check(X >= 0.0f && X <= 1.0f);

	if(this == nullptr)
	{
		return 0.0f;
	}

	if(X >= 1.0f)
	{
		X -= 1.0f;
	}

	// 0 and 1 are the same arena point.
	// This is occupied by the first and last terrain vertex.

	X *= Elevations.Num() - 1;
	
	return Elevations[ROUND(X)];
}


void Defcon::CTerrain::Draw(FPainter& Painter, const I2DCoordMapper& Mapper)
{
	// In UE, we draw the terrain as a set of solid lines.

	const float PaintWidth = Painter.GetWidth();

	CFPoint Pt;

	const int32 NumVertices = Vertices.Num();

	int32 J = 0, K = 0;
	float Xmin = 1.0e+10f, Xmax = -1.0e+10f;

	// Find the visible part of the terrain.

	// Find the first visible terrain part on the left side.
	for(int32 I = 0; I < NumVertices; I++)
	{
		CFPoint PtTemp((float)Vertices[I].X, (float)Vertices[I].Y);
		Mapper.To(PtTemp, Pt);

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

		CFPoint PtTemp((float)Vertices[M].X, (float)Vertices[M].Y);

		Mapper.To(PtTemp, Pt);

		LinePts.Add(FVector2f(Pt.x, Pt.y));
	}

	FSlateDrawElement::MakeLines(*Painter.OutDrawElements, Painter.LayerId, *Painter.PaintGeometry, LinePts, ESlateDrawEffect::None, C_RED, true, 3.0f);
}


void Defcon::CTerrain::DrawSmall(FPainter& Painter, const I2DCoordMapper& Mapper, FSlateBrush&)
{
	// The entire terrain is visible, so don't search for start/end points.
	// Note that the player is always in the center, so the terrain shifts left/right.

	// note: Another way is to render once to texture, then draw at most two quads.
	// After all, the terrain only takes part of the radar widget and that widget 
	// takes only a small part of the screen.


	CFPoint Pt;

	const int32 NumVertices = Vertices.Num();

	TArray<FVector2f>  LinePts;

	LinePts.Reserve(Vertices.Num() + 1);

	// We can't just draw from the first to last vertex, because the first vertex could 
	// be somewhere to the right of the left edge of the widget, and a wraparound line will appear 
	// when we reach the visually rightmost vertex as it connects to the visually leftmost.

	// We need to start with the visually leftmost vertex, and increment with modulus.

	// Find the visually leftmost vertex.
	float MinX = 1.0e+10;

	int32 J = 0;

	for(int32 Index = 0; Index < NumVertices - 1; Index++)
	{
		const auto& Vertex = Vertices[Index];
		CFPoint PtTemp((float)Vertex.X, (float)Vertex.Y);
		Mapper.To(PtTemp, Pt);

		if(Pt.x > 0 && Pt.x < MinX)
		{
			J = Index;
			MinX = Pt.x;
		}
	}

	// Start with the vertex just outside the left edge.
	J--;
	if(J < 0)
	{
		J = NumVertices - 2;
	}

	// Now draw all the vertices starting at J and wrapping around when we hit the right edge.

	int32 I = 0;

	const float OurWidth = Painter.AllottedGeometry->GetLocalSize().X;

	for(int32 Index = J; Index < J + NumVertices; Index++, I++)
	{
		const auto& Vertex = Vertices[Index % (NumVertices)];
		CFPoint PtTemp((float)Vertex.X, (float)Vertex.Y);
		Mapper.To(PtTemp, Pt);

		// First drawn vertex will be near the right edge
		if(I == 0)
		{
			Pt.x -= OurWidth;
		}

		LinePts.Add(FVector2f(Pt.x, Pt.y));
	}

	// We still need 1-2 more lines to clip against the right edge.
	for(int32 Index = J + NumVertices; Index < J + NumVertices + 2; Index++)
	{
		const auto& Vertex = Vertices[Index % NumVertices];
		CFPoint PtTemp((float)Vertex.X, (float)Vertex.Y);
		Mapper.To(PtTemp, Pt);

		// If we wrapped back to the left edge, unwrap.
		if(Pt.x < OurWidth / 2)
		{
			Pt.x += OurWidth;
		}
		LinePts.Add(FVector2f(Pt.x, Pt.y));
	}

	FSlateDrawElement::MakeLines(*Painter.OutDrawElements, Painter.LayerId, *Painter.PaintGeometry, LinePts, ESlateDrawEffect::None, C_RED, true, 2.0f);
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
