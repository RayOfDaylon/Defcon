// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.


#include "materialization.h"
#include "Main/mapper.h"
#include "Globals/_sound.h"
#include "Arenas/DefconPlayViewBase.h"


Defcon::CMaterialization::CMaterialization()
{
	ParentType = Type;
	Type       = EObjType::MATERIALIZATION;

	bMortal    = true;
}


void Defcon::CMaterialization::InitMaterialization(const FMaterializationParams& InParams)
{
	check(InParams.Colors.Num() > 0);

	Params = InParams;

	Lifespan = Params.Lifetime;

	Particles.Reserve(Params.NumParticles);

	for(int32 Index = 0; Index < Params.NumParticles; Index++)
	{
		FMaterializationParticle Particle;

		Particle.Color = Params.Colors[IRAND(Params.Colors.Num())];
		const auto Vec = FVector2D(Params.P.x, Params.P.y)  + Daylon::AngleToVector2D(Daylon::FRandRange(0.0f, 360.0f)) * FVector2D(1.0f, 1.0f / Params.AspectRatio) * Daylon::FRandRange(Params.StartingRadiusMin, Params.StartingRadiusMax);
		Particle.SrcP.Set(Vec.X, Vec.Y);
		Particle.DstP = Params.P; // for now. Later on we might want to use some point on the bboxrad

		Particles.Add(Particle);
	}
}


void Defcon::CMaterialization::Tick(float DeltaTime)
{
	Age += DeltaTime;

	const float T = Age / Params.Lifetime;

	for(auto& Particle : Particles)
	{
		Particle.Update(T, Params);
	}
}


void Defcon::CMaterialization::Draw(FPainter& Painter, const I2DCoordMapper& Mapper)
{
	const auto LayerId = Painter.LayerId + 1;

	const auto Os = Painter.RenderOpacity;

	CFPoint pt;

	for(const auto& Particle : Particles)
	{
		// Particles use pure arena space, but we need to wrap X before using mapper.
		auto P = Particle.P;
		P.x = GArena->WrapX(P.x);
		Mapper.To(P, pt);

		// Don't draw if particle out of frame.
		if(pt.x < 0 || pt.x >= Painter.GetWidth())
		{
			//UE_LOG(LogGame, Warning, TEXT("Materialization particle out of frame at %d"), (int32)pt.x);
			continue;
		}

		//UE_LOG(LogGame, Warning, TEXT("Materialization particle visible at %d"), (int32)pt.x);
		const auto S = FVector2D(Particle.S);
		const FSlateLayoutTransform Translation(FVector2D(pt.x, pt.y) - S / 2);
		const auto Geometry = Painter.AllottedGeometry->MakeChild(S, Translation);

		FSlateDrawElement::MakeBox(
			*Painter.OutDrawElements,
			LayerId,
			Geometry.ToPaintGeometry(),
			GGameObjectResources.DebrisBrushRoundPtr,
			ESlateDrawEffect::None,
			Particle.Color * Os);
	}
}


void Defcon::CCreateMaterializationTask::Do()
{
	auto Materialization = new CMaterialization();

	Materialization->InitMaterialization(Params);

	GArena->AddDebris(Materialization);

	if(GArena->IsPointVisible(Params.P)) // todo: maybe check extents of materialization field i.e. if player gets even a partial glimpse
	{
		GAudio->OutputSound(EAudioTrack::Ship_materialize);
	}
}


