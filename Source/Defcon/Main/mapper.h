#pragma once

#include "Common/util_geom.h"


class I2DCoordMapper
{
	public:
		virtual void To		(const CFPoint&, CFPoint&) const = 0;
		virtual void From	(const CFPoint&, CFPoint&) const = 0;

};


class CNull2DCoordMapper : public I2DCoordMapper
{
	public:
		virtual void To		(const CFPoint& from, CFPoint& to) const override { to = from; }
		virtual void From	(const CFPoint& from, CFPoint& to) const override { to = from; }
};



namespace Defcon
{
	class CPlayer;


	class CArenaCoordMapper : public I2DCoordMapper
	{
		public:
			CArenaCoordMapper() {}

			virtual void To    (const CFPoint&, CFPoint&) const override;
			virtual void From  (const CFPoint&, CFPoint&) const override;

			void Init          (int, int, int);
			void ScrollTo      (const CFPoint& pt) { m_offset = pt; }

			void SlideBy       (float);

			const CFPoint& GetOffset() const { return m_offset; }

			float	m_planetCircumference;

		private:
			CFPoint	m_offset;
			CFPoint	m_screensize;

	};


	class CRadarCoordMapper : public I2DCoordMapper
	{
		public:
			CRadarCoordMapper() {}

			virtual void To    (const CFPoint&, CFPoint&) const override;
			virtual void From  (const CFPoint&, CFPoint&) const override;

			void Init     (int, int, int);
			void ScrollTo (const CFPoint& pt) { m_offset = pt; }
			void SlideBy  (float);

			const CFPoint& GetOffset     () const { return m_offset; }
			const CFPoint& GetScreenSize () const { return m_screensize; }

			float	    m_planetCircumference;

			CFPoint     m_radarSize;
			CPlayer*    m_pPlayer;

		private:
			mutable CFPoint	m_offset;
			CFPoint	        m_screensize;
			CFPoint         m_scale;
	};
}
