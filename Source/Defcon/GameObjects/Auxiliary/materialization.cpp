// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


#include "materialization.h"
#include "Main/mapper.h"
#include "Arenas/DefconPlayViewBase.h"


Defcon::CMaterialization::CMaterialization()
{
	m_parentType = m_type;
	m_type       = ObjType::MATERIALIZATION;

	m_bMortal    = true;
}


void Defcon::CMaterialization::InitMaterialization(const FMaterializationParams& InParams)
{
	check(InParams.Colors.Num() > 0);

	Params = InParams;

	m_fLifespan = Params.Lifetime;

	Particles.Reserve(Params.NumParticles);

	for(int32 Index = 0; Index < Params.NumParticles; Index++)
	{
		FMaterializationParticle Particle;

		Particle.Color = Params.Colors[IRAND(Params.Colors.Num())];
		const auto Vec = FVector2D(Params.P.x, Params.P.y)  + Daylon::AngleToVector2D(Daylon::FRandRange(0.0f, 360.0f)) * FVector2D(1.0f, 1.0f / Params.AspectRatio) * Daylon::FRandRange(Params.StartingRadiusMin, Params.StartingRadiusMax);
		Particle.SrcP.set(Vec.X, Vec.Y);
		Particle.DstP = Params.P; // for now. Later on we might want to use some point on the bboxrad

		Particles.Add(Particle);
	}
}


void Defcon::CMaterialization::Move(float DeltaTime)
{
	m_fAge += DeltaTime;

	const float T = m_fAge / Params.Lifetime;

	for(auto& Particle : Particles)
	{
		Particle.Update(T, Params);
	}
}


void Defcon::CMaterialization::Draw(FPaintArguments& PaintArgs, const I2DCoordMapper& Mapper)
{
	const auto LayerId = PaintArgs.LayerId + 1;

	const auto Os = PaintArgs.RenderOpacity;

	CFPoint pt;

	for(const auto& Particle : Particles)
	{
		// Particles use pure arena space, but we need to wrap X before using mapper.
		auto P = Particle.P;
		P.x = gpArena->WrapX(P.x);
		Mapper.To(P, pt);

		// Don't draw if particle out of frame.
		if(pt.x < 0 || pt.x >= PaintArgs.GetWidth())
		{
			//UE_LOG(LogGame, Warning, TEXT("Materialization particle out of frame at %d"), (int32)pt.x);
			continue;
		}

		//UE_LOG(LogGame, Warning, TEXT("Materialization particle visible at %d"), (int32)pt.x);
		const auto S = FVector2D(Particle.S);
		const FSlateLayoutTransform Translation(FVector2D(pt.x, pt.y) - S / 2);
		const auto Geometry = PaintArgs.AllottedGeometry->MakeChild(S, Translation);

		FSlateDrawElement::MakeBox(
			*PaintArgs.OutDrawElements,
			LayerId,
			Geometry.ToPaintGeometry(),
			GameObjectResources.DebrisBrushRoundPtr,
			ESlateDrawEffect::None,
			Particle.Color * Os);
	}
}



