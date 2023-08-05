// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

// materialization.h

/*
	A materialization is an implosion animation that draws a bunch of colored particles
	approaching a common position, varying the size and appearance of each particle
	along the way, depending on given parameters.
*/

#include "CoreMinimal.h"
#include "Common/Painter.h"
#include "Common/util_core.h"
#include "Common/util_geom.h"
#include "Common/util_color.h"
#include "GameObjects/gameobjlive.h"


namespace Defcon
{
	struct FMaterializationParams
	{
		TArray<FLinearColor> Colors;           // Sampled evenly; repeat a color to favor it
		CFPoint              P;
		CFPoint              TargetBoxRadius;
		int32                NumParticles;
		float                Lifetime;
		float                StartingRadiusMin;
		float                StartingRadiusMax;
		float                ParticleSizeMin;
		float                ParticleSizeMax;
		float                OsMin;
		float                OsMax;
		float                AspectRatio = 1.0f;
	};


	struct FMaterializationParticle
	{
		FLinearColor	Color;
		CFPoint         SrcP;
		CFPoint         DstP;

		CFPoint         P;
		float           S;

		void Update(float T, const FMaterializationParams& Params)
		{
			P = SrcP;
			P.Lerp(DstP, T);

			Color.A = LERP(Params.OsMin, Params.OsMax, T);
			S       = LERP(Params.ParticleSizeMin, Params.ParticleSizeMax, T);
		}
	};


	class CMaterialization : public IGameObject
	{
		public:
			CMaterialization();
			virtual ~CMaterialization() {}

			void InitMaterialization(const FMaterializationParams& InParams);

			virtual void Move		(float DeltaTime) override;
			virtual void Draw		(FPainter&, const Defcon::I2DCoordMapper&) override;
			virtual void DrawSmall	(FPainter&, const Defcon::I2DCoordMapper&, FSlateBrush&) override {}


		private:
			FMaterializationParams           Params;
			TArray<FMaterializationParticle> Particles;
	};
}


