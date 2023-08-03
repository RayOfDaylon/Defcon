// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once



#if 0

#include "Common/Painter.h"
#include "Common/util_geom.h"

#include "GameObjects/gameobj.h"
#include "GameObjects/gameobjlive.h"

#include "GameObjects/Auxiliary/slider.h"

#include "arena.h"
#include "Main/mapper.h"


namespace Defcon
{
	typedef CNull2DCoordMapper	COptionsArenaCoordMapper;


	class COptionsArena : public IArena
	{
		// This arena lets the user edit the 
		// preference variables.

		friend class options_arena_inputs;


		public:
#ifdef _DEBUG
			virtual const char* GetClassname() const
			{
				static const char* psz = "OptionsEditor";
				return psz;
			}
#endif

			virtual void OnDisplaySizeChanged(int32, int32);
			virtual void Init(const ArenaParams&);
			virtual void Destroy();
			virtual void Update(float);
			virtual void UpdateKybdState();
			virtual float GetWidth() const { return (float)m_virtualScreen.GetWidth(); }
			virtual float GetHeight() const { return (float)m_virtualScreen.GetHeight(); }
			void FastTextOut(const CFPoint& pt, const char* psz)
			{
				ArenaFastTextOut(pt, m_virtualScreen, psz);
			}

			void NormalTextOut(const CFPoint& pt, const char* psz, FLinearColor c)
			{
				ArenaTextOut(pt, m_virtualScreen, psz, c, false);
			}

		protected:

			enum State { viewing, editing };
			State	m_eState;


			void StartEditing();
			void StopEditing();
			void CancelEditing();

			void UpdateValueText(int32);

			void Scroll();

			//void OnKeyboardEvent(int32);

			//void navigate(EventType);


			FPaintArguments				m_virtualScreen;

			/*enum Keys 
			{
				moveup,
				movedown, 
				moveleft, 
				moveright,
				pageup,
				pagedown,
				home,
				end,
				choose,
				exit,
				count
			};*/


			COptionsArenaCoordMapper		m_coordMapper;

			//KybdKey					m_keys[Keys::count];

			int32					m_topItem;

			//CGameObjectCollection	m_objects;
#if 0
			CStaticTextDisplayer	m_title;
			CStaticTextDisplayer	m_desc;
			CStaticTextDisplayer	m_items[10];
			CStaticTextDisplayer	m_values[10];

			CStaticTextDisplayer	m_edittitle;
			CSlider					m_slider;
			IEditor*				m_pEditor;
#endif
			float					m_fadeInEditor;
			int						m_valueLeft;

			CFRect					m_box;

			ArenaParams*			m_pParams;

			bool					m_bLaunching;
			float					m_fLaunchAge;

			void FocusItem(int32);
	}; 
}

#endif

