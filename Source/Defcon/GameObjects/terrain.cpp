/*
	terrain.cpp
	Terrain for Defcon game.
	Copyright 2003-2004 Daylon Graphics Ltd.
*/


#include "terrain.h"




#include "Common/util_core.h"
#include "Globals/GameColors.h"
#include "Globals/prefs.h"
#include "Main/mapper.h"

#include "DaylonUtils.h"
#include "DefconLogging.h"

#include "Runtime/SlateCore/Public/Rendering/DrawElements.h"


#pragma optimize("", off)

#ifdef SHOW_STATS
	extern bool gbShowStats;
#endif

const float MinTerrainLineLength =  40.0f;
const float MaxTerrainLineLength = 120.0f;


// ------------------------------------------------------


Defcon::CTerrain::CTerrain()
{
	m_fAge          = 0.0f;
	m_bMortal       = false;
	m_bCanBeInjured = false;
	m_width         = m_maxheight = 0;
	m_firsttime     = true;
	m_deltas        = nullptr;
}

Defcon::CTerrain::~CTerrain()
{
	if(m_deltas != nullptr)
	{
		delete [] m_deltas;
	}
}


#ifdef _DEBUG
const char* Defcon::CTerrain::GetClassname() const
{
	static char* psz = "Terrain";
	return psz;
}
#endif


void Defcon::CTerrain::InitTerrain(int seed, int w, int h, int diff)
{
	// Set up some funky mountainry.
	// The greater <diff> is, the more funky the terrain.
	// todo: we don't use <diff> at the moment, but should.

	Vertices.Reserve(300);
	Vertices.Empty();

	m_width     = w;
	m_maxheight = h;


	if(seed >= 0)
	{
		srand(seed);
		diff = (int)(FRAND * 100000 + 20000);
	}

	float t = NORM_((float)diff, 0, 200000);
	t = FMath::Min(t, 1.0f);
	diff = (int)(t * 50);


	// New vertex generator

	const float MaxHeight = h * 0.45f;
	const float MinHeight = h * 0.05f;
	
	FVector2D P(0.0f, Daylon::FRandRange(MinHeight, MaxHeight)); // current point. Start at x = 0
	Vertices.Add(P);
	
	// Walk until current point is still inside arena.

	while(P.X < w) // Note: we need first and last vertex to join at a 0 or 45 degree angle.
	{
		FVector2D P2;

		int DirectionY;

		do
		{
			DirectionY = Daylon::RandRange(-1, 1);
			// todo: we can boost performance by collapsing lines that extend current direction.

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
		const int SlopeBehind = SGN(Vertices[CurrentVertexIdx - 1].Y - Vertices[CurrentVertexIdx].Y);
		const int SlopeAhead  = SGN(Vertices[CurrentVertexIdx].Y     - Vertices[CurrentVertexIdx + 1].Y);

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

	const size_t numElevs = w + 1;
	m_elevs.reserve(numElevs);
	m_elevs.resize(numElevs);

	for(int32 Index = 0; Index < Vertices.Num() - 1; Index++)
	{
		FVector2D P1 = Vertices[Index];
		FVector2D P2 = Vertices[Index + 1];

		// Iterate through line at pixel res.
		for(int32 X = (int32)P1.X; X <= (int32)P2.X; X++)
		{
			m_elevs[X] = (int32)FMath::Lerp(P1.Y, P2.Y, Daylon::Normalize(X, P1.X, P2.X));
		}
	}
	// todo: we probably have gaps in m_elevs, need to clean that up.
}


float Defcon::CTerrain::GetElev(float x) const
{
	// x is normalized.
	check(x >= 0.0f && x <= 1.0f);

	if(this == nullptr)
	{
		return 0.0f;
	}

	if(x >= 1.0f)
	{
		x -= 1.0f;
	}

	// 0 and 1 are the same arena point.
	// This is occupied by the first and last terrain vertex.

	x *= m_elevs.size() - 1;
	//return (float)((m_maxheight - 1) - m_elevs[ROUND(x)]);
	return (float)m_elevs[ROUND(x)];
}


void Defcon::CTerrain::Move(float f)
{
	m_fAge += f;
}


void Defcon::CTerrain::Draw(FPaintArguments& framebuf, const I2DCoordMapper& mapper)
{
	// In UE, we draw the terrain as a set of solid lines.

	const float framebufWidth = framebuf.GetWidth();

	CFPoint pt;

	const int32 NumVertices = Vertices.Num();

	int32 i, j = 0, k = 0;
	float xmin = 1.0e+10f, xmax = -1.0e+10f;

	// Find the visible part of the terrain.

	// Find the first visible terrain part on the left side.
	for(i = 0; i < NumVertices; i++)
	{
		CFPoint ptTemp((float)Vertices[i].X, (float)Vertices[i].Y);
		mapper.To(ptTemp, pt);

		if(pt.x >= 0 && pt.x < framebufWidth)
		{
			if(pt.x < xmin)
			{
				xmin = pt.x;
				j = i;
			}

			if(pt.x > xmax)
			{
				xmax = pt.x;
				k = i;
			}
		}
	}

	// todo: cache j, k and perform above calc only when invalidated.
	j -= 2;
	k++;

	if(k < j)
	{
		k+= NumVertices;
	}

	TArray<FVector2f>  LinePts;

	LinePts.Reserve(k - j + 1);

	for(i = j; i <= k; i++)
	{
		const int32 m = (i + NumVertices) % NumVertices;

		CFPoint ptTemp((float)Vertices[m].X, (float)Vertices[m].Y);

		mapper.To(ptTemp, pt);

		LinePts.Add(FVector2f(pt.x, pt.y));
	}


	FSlateDrawElement::MakeLines(*framebuf.OutDrawElements, framebuf.LayerId, *framebuf.PaintGeometry, LinePts, ESlateDrawEffect::None, C_RED, true, 3.0f);
}


void Defcon::CTerrain::DrawSmall(FPaintArguments& framebuf, const I2DCoordMapper& mapper, FSlateBrush&)
{
	// The entire terrain is visible, so don't search for start/end points.
	// Note that the player is always in the center, so the terrain shifts left/right.


	CFPoint pt;

	const int32 NumVertices = Vertices.Num();

	TArray<FVector2f>  LinePts;

	LinePts.Reserve(Vertices.Num() + 1);

	// We can't just draw from the first to last vertex, because the first vertex could 
	// be somewhere to the right of the left edge of the widget, and a wraparound line will appear 
	// when we reach the visually rightmost vertex as it connects to the visually leftmost.

	// We need to start with the visually leftmost vertex, and increment with modulus.

	// Find the visually leftmost vertex.
	float minX = 1.0e+10;

	int32 j = 0;

	for(int32 Index = 0; Index < NumVertices - 1; Index++)
	{
		const auto& Vertex = Vertices[Index];
		CFPoint ptTemp((float)Vertex.X, (float)Vertex.Y);
		mapper.To(ptTemp, pt);

		if(pt.x > 0 && pt.x < minX)
		{
			j = Index;
			minX = pt.x;
		}
	}

	// Start with the vertex just outside the left edge.
	j--;
	if(j < 0)
	{
		j = NumVertices - 2;
	}

	// Now draw all the vertices starting at J and wrapping around when we hit the right edge.

	int32 i = 0;

	const float OurWidth = framebuf.AllottedGeometry->GetLocalSize().X;

	for(int32 Index = j; Index < j + NumVertices; Index++, i++)
	{
		const auto& Vertex = Vertices[Index % (NumVertices)];
		CFPoint ptTemp((float)Vertex.X, (float)Vertex.Y);
		mapper.To(ptTemp, pt);

		// First drawn vertex will be near the right edge
		if(i == 0)
		{
			pt.x -= OurWidth;
		}

		LinePts.Add(FVector2f(pt.x, pt.y));
	}

	// We still need 1-2 more lines to clip against the right edge.
	for(int32 Index = j + NumVertices; Index < j + NumVertices + 2; Index++)
	{
		const auto& Vertex = Vertices[Index % NumVertices];
		CFPoint ptTemp((float)Vertex.X, (float)Vertex.Y);
		mapper.To(ptTemp, pt);
		// If we wrapped back to the left edge, unwrap.
		if(pt.x < OurWidth / 2)
		{
			pt.x += OurWidth;
		}
		LinePts.Add(FVector2f(pt.x, pt.y));
	}

	FSlateDrawElement::MakeLines(*framebuf.OutDrawElements, framebuf.LayerId, *framebuf.PaintGeometry, LinePts, ESlateDrawEffect::None, C_RED, true, 2.0f);
}



#pragma optimize("", on)
