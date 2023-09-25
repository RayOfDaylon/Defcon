// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "DaylonRNG.h"
#include "util_math.h"
#include "util_color.h"
#include "Painter.h"
#include "Main/mapper.h"


namespace Defcon
{
	struct FParticle
	{
		FLinearColor Color;
		CFPoint      P;
		CFPoint      Inertia;
		float        S;
		float        Age = 0.0f;
		float        Lifetime = 0.0f;


		void Update(float DeltaTime)
		{
			Age += DeltaTime;
			P += Inertia * DeltaTime;
		}


		void Draw(FPainter& Painter, const Defcon::I2DCoordMapper& CoordMapper)
		{
			CFPoint pt;
			CoordMapper.To(P, pt);
			const float HalfS = S / 2;

			Color.A = 1.0f - CLAMP(NORM_(Age, 0.0f, Lifetime), 0.0f, 1.0f);

			Painter.FillRect(pt.x - HalfS, pt.y - HalfS, pt.x + HalfS, pt.y + HalfS, Color);
		}
	};


	struct FParticleGroup
	{
		FParticle Particles[8][2];
		CFPoint  P;


		FParticleGroup()
		{
			for(int32 y = 0; y < 2; y++)
			{
				for(int32 x = 0; x < 8; x++)
				{
					Particles[x][y].Color = BRAND ? C_WHITE : C_YELLOW;
					Particles[x][y].S = Daylon::FRandRange(3.0f, 9.0f);
				}
			}
		}


		void Init(const CFPoint& ObjP, const CFPoint& ObjHalfS, float ParticleSpeed, float MinParticleLifetime, float MaxParticleLifetime)
		{
			const CFPoint OriginShift((float)Daylon::FRandRange(-ObjHalfS.x * 1, ObjHalfS.x * 1), (float)Daylon::FRandRange(-ObjHalfS.y * 0.75, ObjHalfS.y * 0.75));
			const auto Origin = ObjP + OriginShift;

			for(int32 y = 0; y < 2; y++)
			{
				const float ty = y;

				for(int32 x = 0; x < 8; x++)
				{
					const float tx = x / 7.0f;

					Particles[x][y].Lifetime = Daylon::FRandRange(MinParticleLifetime, MaxParticleLifetime);
					Particles[x][y].P        = Origin + CFPoint(LERP(-ObjHalfS.x, ObjHalfS.x, tx), LERP(-ObjHalfS.y, ObjHalfS.y, ty));
					Particles[x][y].Inertia  = (Particles[x][y].P - P) * ParticleSpeed;
				}
			}
		}


		void Update(float DeltaTime)
		{
			for(int32 y = 0; y < 2; y++)
			{
				for(int32 x = 0; x < 8; x++)
				{
					Particles[x][y].Update(DeltaTime);
				}
			}
		}


		void Draw(FPainter& Painter, const Defcon::I2DCoordMapper& CoordMapper)
		{
			for(int32 y = 0; y < 2; y++)
			{
				for(int32 x = 0; x < 8; x++)
				{
					Particles[x][y].Draw(Painter, CoordMapper);
				}
			}
		}
	};
}