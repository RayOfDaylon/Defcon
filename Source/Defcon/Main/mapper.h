// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "Common/util_geom.h"

/*
	Mappers convert coordinates between world spaces and display spaces.
	In Defcon, we use the same unit magnitudes in each (Slate units) but
	the display space may be shifted (like in the main play arena) or 
	shrunk (like in the radar view).
*/

namespace Defcon
{
	class I2DCoordMapper
	{
		public:

			virtual void To		(const CFPoint&, CFPoint&) const = 0;
			virtual void From	(const CFPoint&, CFPoint&) const = 0;

	};


	class CNull2DCoordMapper : public I2DCoordMapper
	{
		public:

			virtual void To		(const CFPoint& From_, CFPoint& To_) const override { To_ = From_; }
			virtual void From	(const CFPoint& From_, CFPoint& To_) const override { To_ = From_; }
	};


	class IGameObject;


	class CArenaCoordMapper : public I2DCoordMapper
	{
		public:

			CArenaCoordMapper() {}

			virtual void    To         (const CFPoint&, CFPoint&) const override;
			virtual void    From       (const CFPoint&, CFPoint&) const override;

			void            Init       (float, float, float);
			void            ScrollTo   (const CFPoint& P) { Offset = P; }
			void            SlideBy    (float XAmount);
			const CFPoint&  GetOffset  () const { return Offset; }

			float	PlanetCircumference = 0.0f;


		private:

			CFPoint	Offset;
			CFPoint	ScreenSize;
			float   HalfCircumference = 0.0f;

	};


	class CRadarCoordMapper : public I2DCoordMapper
	{
		public:

			CRadarCoordMapper() {}

			virtual void    To            (const CFPoint&, CFPoint&) const override;
			virtual void    From          (const CFPoint&, CFPoint&) const override;

			void            Init          (float, float, float);
			void            ScrollTo      (const CFPoint& P) { Offset = P; }
			void            SlideBy       (float XAmount);
			const CFPoint&  GetOffset     () const { return Offset; }
			const CFPoint&  GetScreenSize () const { return ScreenSize; }

			float	        PlanetCircumference = 0.0f;

			CFPoint         RadarSize;
			IGameObject*    Player = nullptr;


		private:

			mutable CFPoint	Offset;
			CFPoint	        ScreenSize;
			CFPoint         Scale;
			float           HalfCircumference = 0.0f;
	};
}
