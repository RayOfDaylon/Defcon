/*
	arena_options.cpp
	Defcon options editing arena.
	Copyright 2004 Daylon Graphics Ltd.

	rcg		jul 13/04	Created.
*/


#include "arena_options.h"






#include "Globals/_sound.h"

#include "Globals/prefs.h"


#include "Main/mapper.h"

#if 0

#undef C_UNSELECTED		
#undef C_SELECTED			
#undef C_SELECTEDDARK		
#undef C_UNSELECTEDVALUE	
#undef C_SELECTEDVALUE		

#define C_UNSELECTED		C_YELLOW
#define C_SELECTED			C_WHITE
#define C_SELECTEDDARK		C_LIGHT
#define C_UNSELECTEDVALUE	C_ORANGE
#define C_SELECTEDVALUE		C_SELECTED

#define LAUNCH_TIME		0.7f
#define SLIDER_LEN		230

// ------------------------------------------------------

static int32 sCurrentItem_arena_optons = 0;

// ------------------------------------------------------

namespace Defcon
{
	class options_arena_inputs : public Defcon::IControllerInputs
	{
		public:
			options_arena_inputs() : m_pArena(nullptr) 
			{
				m_size = 10;
				m_events[0].what = EventType::navigate_up;			m_events[0].bRepeats = true;
				m_events[1].what = EventType::navigate_down;		m_events[1].bRepeats = true;
				m_events[2].what = EventType::navigate_right;		m_events[2].bRepeats = true;
				m_events[3].what = EventType::navigate_left;		m_events[3].bRepeats = true;
				m_events[4].what = EventType::select;				m_events[4].bRepeats = false;
				m_events[5].what = EventType::cancel;				m_events[5].bRepeats = false;
				m_events[6].what = EventType::navigate_home;		m_events[6].bRepeats = false;
				m_events[7].what = EventType::navigate_end;			m_events[7].bRepeats = false;
				m_events[8].what = EventType::navigate_page_up;		m_events[8].bRepeats = false;
				m_events[9].what = EventType::navigate_page_down;	m_events[9].bRepeats = false;
			}
			virtual void process(const ControllerEvent&);
			Defcon::COptionsArena* m_pArena;
	};
}


void Defcon::options_arena_inputs::process(const ControllerEvent& evt)
{
	check(m_pArena != nullptr);

	switch(evt.what)
	{
		case Defcon::EventType::navigate_up:
			gpAudio->OutputSound(snd_select);
			if(m_pArena->m_eState == Defcon::COptionsArena::State::viewing)
			{
				if(sCurrentItem_arena_optons == 0)
					m_pArena->FocusItem(array_size(gPrefs.m_pref)-1);
				else
					m_pArena->FocusItem(sCurrentItem_arena_optons - 1);
			}
			break;


		case Defcon::EventType::navigate_down:
			gpAudio->OutputSound(snd_select);
			if(m_pArena->m_eState == Defcon::COptionsArena::State::viewing)
			{
				if(sCurrentItem_arena_optons == array_size(gPrefs.m_pref)-1)
					m_pArena->FocusItem(0);
				else
					m_pArena->FocusItem(sCurrentItem_arena_optons + 1);
			}
			break;


		case Defcon::EventType::select:
			switch(m_pArena->m_eState)
			{
				case Defcon::COptionsArena::State::viewing:
					m_pArena->StartEditing();
					break;
				case Defcon::COptionsArena::State::editing:
					m_pArena->StopEditing();
					break;
			}
			break;

		case Defcon::EventType::cancel:
			switch(m_pArena->m_eState)
			{
				case Defcon::COptionsArena::State::viewing:
					m_pArena->m_pNextArena = Defcon::CArenaFactory::Make(Defcon::ArenaKind::menu);
					break;
				case Defcon::COptionsArena::State::editing:
					m_pArena->CancelEditing();
					break;
			}
			break;

		case Defcon::EventType::navigate_left:
		case Defcon::EventType::navigate_right:
		case Defcon::EventType::navigate_home:
		case Defcon::EventType::navigate_end:
		case Defcon::EventType::navigate_page_up:
		case Defcon::EventType::navigate_page_down:
			switch(m_pArena->m_eState)
			{
				case Defcon::COptionsArena::State::viewing:
					m_pArena->navigate(evt.what);
					break;

				case Defcon::COptionsArena::State::editing:
					if(m_pArena->m_pEditor != nullptr)
					{
						m_pArena->m_pEditor->process(evt);
						gPrefs.m_pref[sCurrentItem_arena_optons].SetValue(m_pArena->m_pEditor->GetValue());
						//gPrefs.OnUpdate();
						m_pArena->UpdateValueText(sCurrentItem_arena_optons - m_pArena->m_topItem);
					}
					break;
			}
			break;
	}
}


static Defcon::options_arena_inputs s_event_handler_arena_options;

// -----------------------------------------------------------------


void Defcon::COptionsArena::Init
(
	const Defcon::ArenaParams& params
)
{
	m_pEditor = nullptr;
	m_eState = State::viewing;
	m_bLaunching = false;
	m_fLaunchAge = 0.0f;
	m_fAge = 0.0f;
	m_topItem = 0;

	m_pParams = &(Defcon::ArenaParams&)params;

	const IntRect& r = params.r;

	const int32 w = r.right - r.left;
	const int32 h = r.bottom - r.top;

	this->OnDisplaySizeChanged(w, h);

	s_event_handler_arena_options.m_pArena = this;
	m_pInputs = &s_event_handler_arena_options;
	m_pInputs->nullify();

	
	/*ZEROFILL(m_keys);

	m_keys[Keys::moveup].nKey = VK_UP;
	m_keys[Keys::moveup].bRepeats = true;

	m_keys[Keys::movedown].nKey = VK_DOWN;
	m_keys[Keys::movedown].bRepeats = true;

	m_keys[Keys::moveleft].nKey = VK_LEFT;
	m_keys[Keys::moveleft].bRepeats = true;

	m_keys[Keys::moveright].nKey = VK_RIGHT;
	m_keys[Keys::moveright].bRepeats = true;

	m_keys[Keys::pageup].nKey = VK_PRIOR;
	m_keys[Keys::pageup].bRepeats = true;

	m_keys[Keys::pagedown].nKey = VK_NEXT;
	m_keys[Keys::pagedown].bRepeats = true;

	m_keys[Keys::home].nKey = VK_HOME;
	m_keys[Keys::home].bRepeats = true;

	m_keys[Keys::end].nKey = VK_END;
	m_keys[Keys::end].bRepeats = true;

	m_keys[Keys::exit].nKey = VK_ESCAPE;
	m_keys[Keys::choose].nKey = VK_RETURN;*/


	int32 i;

	auto varfontid = EFont::heading;

	// Set up item text displayers.
	for(i = 0; i < array_size(m_items); i++)
	{
		m_items[i].SetFontID(varfontid);
		m_items[i].SetExternalOwnership(true);
		m_items[i].SetColor(C_UNSELECTED);
		m_objects.Add(&m_items[i]);

		m_values[i].SetFontID(varfontid);
		m_values[i].SetExternalOwnership(true);
		m_values[i].SetColor(C_UNSELECTEDVALUE);
		m_objects.Add(&m_values[i]);
	}

	m_title.SetColor(C_WHITE);
	m_title.SetFontID(EFont::body);
	m_title.SetAlignment(EAlignment::center);
	m_title.SetExternalOwnership(true);
	m_objects.Add(&m_title);

	m_desc.SetColor(C_WHITE);
	m_desc.SetFontID(EFont::body);
	m_desc.SetAlignment(EAlignment::center);
	m_desc.SetExternalOwnership(true);
	m_objects.Add(&m_desc);

	m_edittitle.SetText("Editing");
	m_edittitle.SetColor(C_WHITE);
	m_edittitle.SetFontID(m_items[0].GetFontID());
	m_edittitle.SetExternalOwnership(true);

	this->FocusItem(sCurrentItem_arena_optons);
}


void Defcon::COptionsArena::Scroll()
{
	if(sCurrentItem_arena_optons < m_topItem)
	{
		m_topItem = sCurrentItem_arena_optons;
	}
	else if(sCurrentItem_arena_optons >= m_topItem + array_size(m_items))
	{
		m_topItem = sCurrentItem_arena_optons - array_size(m_items) + 1;
		m_topItem = FMath::Min(m_topItem, 
			1 + array_size(gPrefs.m_pref) - array_size(m_items));
	}
}

void Defcon::COptionsArena::FocusItem(int32 id)
{
	// Scroll text area if necessary.
	this->Scroll();

	int32 itemid = sCurrentItem_arena_optons - m_topItem;
	m_items[itemid].SetColor(C_UNSELECTED);
	m_values[itemid].SetColor(C_UNSELECTEDVALUE);
	sCurrentItem_arena_optons = id;
	this->Scroll();

	itemid = sCurrentItem_arena_optons - m_topItem;
	m_items[itemid].SetColor(C_SELECTED);
	m_values[itemid].SetColor(C_SELECTEDVALUE);

	// Assign pref vars names to items.
	for(int32 i = 0; i < array_size(m_items); i++)
	{
		m_items[i].SetText(gPrefs.m_pref[m_topItem + i].m_metadata.m_pszName);

		this->UpdateValueText(i);
	}

	m_desc.SetText(gPrefs.m_pref[sCurrentItem_arena_optons].m_metadata.m_pszDesc);
}


void Defcon::COptionsArena::UpdateValueText(int32 i)
{
	int32 v = m_topItem + i;
	char sz[50], sz2[100];
	MySprintf(sz2, "%s %s",
		gPrefs.m_pref[v].GetValueText(sz),
		gPrefs.m_pref[v].m_metadata.m_pszUnits);
	m_values[i].SetText(sz2);
}


void Defcon::COptionsArena::OnDisplaySizeChanged(int32 w , int32 h)
{
	m_virtualScreen.Create(nullptr, w, h, 24, "options_arena");

	// Determine widest pref name.

	int32 i;

	auto varfontid = EFont::heading;
	CAppFont& varfont = gFonts.GetFont(varfontid);
	int32 widestItem = 0;
	for(i = 0; i < array_size(gPrefs.m_pref); i++)
	{
		int32 iw, ih;
		varfont.GetStringExtent(gPrefs.m_pref[i].m_metadata.m_pszName, iw, ih);
		widestItem = FMath::Max(widestItem, iw);
	}

	int32 textleft = (w - (1.75f * widestItem)) / 2;
	int32 leading = varfont.GetLeading();

	int32 texttop = 
		(h - (array_size(m_items) * leading)) / 2
		+ leading/2;

	m_valueLeft = textleft + widestItem + 30;

	// Set up item text displayers.
	for(i = 0; i < array_size(m_items); i++)
	{
		m_items[i].m_pos.set(textleft, texttop + i * leading);
		m_values[i].m_pos.set(m_valueLeft, m_items[i].m_pos.y);
	}

	m_box.set(w/2, h/2 - leading/4);
	m_box.inflate(
		CFPoint(0.5f*w - textleft+100, 
		leading + 0.5f * array_size(m_items) * leading));

	m_title.m_pos.set(w/2, m_items[0].m_pos.y - leading * 3);

	m_desc.m_pos.set(
		w/2, m_items[array_size(m_items)-1].m_pos.y + leading * 3);

	if(m_eState == State::editing)
	{
		i = sCurrentItem_arena_optons - m_topItem;
		CPrefVar& var = gPrefs.m_pref[sCurrentItem_arena_optons];

		switch(var.m_metadata.m_eVarType)
		{
			case type_int:
			case type_float:
				m_slider.m_pos.set(m_valueLeft + SLIDER_LEN/2, 
					m_items[i].m_pos.y - 
					(1.25f * m_items[i].GetLeading()));
				break;
		}
	}
}


void Defcon::COptionsArena::Destroy()
{
}


void Defcon::COptionsArena::navigate(EventType what)
{
	switch(what)
	{
		case EventType::navigate_home:
			this->FocusItem(0);
			break;

		case EventType::navigate_end:
			this->FocusItem(array_size(gPrefs.m_pref)-1);
			break;

		case EventType::navigate_page_up:
		{
			if(sCurrentItem_arena_optons < array_size(m_items))
				this->FocusItem(0);
			else
				this->FocusItem(sCurrentItem_arena_optons - array_size(m_items));
		}
			break;

		case EventType::navigate_page_down:
		{
			int32 n = 
				sCurrentItem_arena_optons + array_size(m_items);
			if(n < array_size(gPrefs.m_pref))
				this->FocusItem(n);
			else
				this->FocusItem(array_size(gPrefs.m_pref)-1);
		}
			break;
	}
}


void Defcon::COptionsArena::Update(float fElapsedTime)
{
	if(fElapsedTime == 0)
		return;

	m_fAge += fElapsedTime;


	// Move and draw our objects.

	m_virtualScreen.Clear(C_BLACK);


	//int32 i;

	// Make the selected item flash only if no 
	// keys are down.
	/*bool bkeydown = false;
	for(i = 0; i < array_size(m_keys); i++)
	{
		if(m_keys[i].bActive)
		{
			bkeydown = true;
			break;
		}
	}*/
	
	const bool bkeydown = m_pInputs->in_use();


	int32 itemid = sCurrentItem_arena_optons - m_topItem;
	if(bkeydown)
	{
		m_items[itemid].SetColor(C_SELECTED);
		m_values[itemid].SetColor(C_SELECTEDVALUE);
	}
	else
	{
		float t = fabs(sin(m_fAge * PI*2));
		FLinearColor c = MakeBlendedColor(C_SELECTEDDARK, C_SELECTED, t);
		m_items[itemid].SetColor(c);
		c = MakeBlendedColor(C_SELECTEDDARK, C_SELECTEDVALUE, t);
		m_values[itemid].SetColor(c);
	}


	switch(m_eState)
	{
		case State::viewing:
		{
			char sz[MAX_PATH_];
			MySprintf(sz, "Current settings. Press arrow keys to select, Enter to change");
			m_title.SetText(sz);
		}
			break;

		case State::editing:
			m_title.SetText("Use left/right arrows to change value. Press Enter when done, Escape to cancel.");
			break;
	}

	m_items[itemid].DrawWithHalo(m_virtualScreen, m_coordMapper);

	GameObjectProcessingParams gop;

	gop.fArenaWidth		= m_virtualScreen.GetWidth();
	gop.fElapsedTime	= fElapsedTime;
	gop.pDib			= &m_virtualScreen;
	gop.pMapper			= &m_coordMapper;

	m_objects.Process(gop);


	int x1, y1, x2, y2;
	m_box.classicize(x1, y1, x2, y2);
	for(int off = 0; off < 2; off++)
		m_virtualScreen.ColorRect(
			x1+off, y1+off, x2-off, y2-off, C_BLUE);

	if(m_eState == State::editing)
	{
		// Make a box around the current var.
		// Leave room above the var for some extra stuff.

		m_fadeInEditor += fElapsedTime;
		m_fadeInEditor = FMath::Min(m_fadeInEditor, 0.33f);
		m_virtualScreen.DarkenRect(x1, y1, x2, y2, 
			MAP(m_fadeInEditor, 0.0f, 0.33f, 1.0f, 0.33f) );

		CFRect r;
		int32 i = sCurrentItem_arena_optons - m_topItem;
		r.UR = r.LL = m_items[i].m_pos;
		r.UR += CFPoint(-20, -(int)m_items[i].GetLeading()*2);
		r.LL += CFPoint(540, 10);
		r.order();
		r.classicize(x1, y1, x2, y2);
		m_virtualScreen.FillRect(x1, y1, x2, y2, C_BLACK);
		for(int off = 0; off < 2; off++)
			m_virtualScreen.ColorRect(
				x1+off, y1+off, x2-off, y2-off, C_BLUE);
		m_items[i].SetColor(C_WHITE);
		m_items[i].Draw(m_virtualScreen, m_coordMapper);
		m_values[i].SetColor(C_WHITE);
		m_values[i].Draw(m_virtualScreen, m_coordMapper);

		m_edittitle.m_pos = m_items[i].m_pos;
		m_edittitle.m_pos.y -= m_items[i].GetLeading();
		m_edittitle.Draw(m_virtualScreen, m_coordMapper);

		if(m_pEditor != nullptr)
			m_pEditor->Draw(m_virtualScreen, m_coordMapper);
	}

	this->FadeIn(m_virtualScreen);
}


void Defcon::COptionsArena::UpdateKybdState()
{
	if(m_bLaunching)
		return;

	IArena::UpdateKybdState();
	//this->UpdateKeysState(m_keys, Keys::count);
}


#if 0
void Defcon::COptionsArena::OnKeyboardEvent(int32 key)
{
	m_keys[key].fTimeLastFired = gettime_secs();

	gpAudio->OutputSound(snd_select);

	switch(key)
	{
		case Keys::moveup:
			if(m_eState == State::viewing)
			{
				if(sCurrentItem_arena_optons == 0)
					this->FocusItem(array_size(gPrefs.m_pref)-1);
				else
					this->FocusItem(sCurrentItem_arena_optons - 1);
			}
			break;


		case Keys::movedown:
			if(m_eState == State::viewing)
			{
				if(sCurrentItem_arena_optons == array_size(gPrefs.m_pref)-1)
					this->FocusItem(0);
				else
					this->FocusItem(sCurrentItem_arena_optons + 1);
			}
			break;


		case Keys::moveleft:
		case Keys::moveright:
		case Keys::pageup:
		case Keys::pagedown:
		case Keys::home:
		case Keys::end:
			switch(m_eState)
			{
				case State::viewing:
					switch(key)
					{
						case Keys::home:
							this->FocusItem(0);
							break;

						case Keys::end:
							this->FocusItem(array_size(gPrefs.m_pref)-1);
							break;

						case Keys::pageup:
						{
							if(sCurrentItem_arena_optons < array_size(m_items))
								this->FocusItem(0);
							else
								this->FocusItem(sCurrentItem_arena_optons - array_size(m_items));
						}
							break;

						case Keys::pagedown:
						{
							int32 n = 
								sCurrentItem_arena_optons + array_size(m_items);
							if(n < array_size(gPrefs.m_pref))
								this->FocusItem(n);
							else
								this->FocusItem(array_size(gPrefs.m_pref)-1);
						}
							break;


					}
					break;

				case State::editing:
					if(m_pEditor != nullptr)
					{
						m_pEditor->OnKey(m_keys[key]);
						gPrefs.m_pref[sCurrentItem_arena_optons].SetValue(m_pEditor->GetValue());
						this->UpdateValueText(sCurrentItem_arena_optons - m_topItem);
					}
					break;
			}
			break;


		case Keys::choose:
			switch(m_eState)
			{
				case State::viewing:
					this->StartEditing();
					break;
				case State::editing:
					this->StopEditing();
					break;
			}
			break;


		case Keys::exit:
			switch(m_eState)
			{
				case State::viewing:
					m_pNextArena = Defcon::CArenaFactory::Make(Defcon::ArenaKind::menu);
					break;
				case State::editing:
					this->CancelEditing();
					break;
			}
			break;

	}
}
#endif // 0


void Defcon::COptionsArena::StartEditing()
{
	int32 i = sCurrentItem_arena_optons - m_topItem;
	CPrefVar& var = gPrefs.m_pref[sCurrentItem_arena_optons];

	switch(var.m_metadata.m_eVarType)
	{
		case type_bool:
			// Just toggle the value immediately.
			var.SetValue(1.0f - var.GetValue());
			this->UpdateValueText(i);
			break;

		case type_choice:
			// Switch to next choice w/ wraparound.
			var.SetValue(var.GetValue() + 1.0f);
			if(var.GetValue() >= var.GetChoiceCount())
				var.SetValue(0.0f);
			this->UpdateValueText(i);
			break;
				

		case type_int:
		case type_float:
		{
			m_slider.SetValue(var.GetValue());

			float fInc = 1.0f;
			if(var.m_metadata.m_eVarType == type_float)
				 fInc = (var.m_metadata.m_fMax - 
					var.m_metadata.m_fMin) / SLIDER_LEN;

			float fPage = (var.m_metadata.m_fMax - 
					var.m_metadata.m_fMin) / 8;
			
			m_slider.Init(SLIDER_LEN,
				var.m_fValue, 
				var.m_metadata.m_fMin,
				var.m_metadata.m_fMax, fInc, fPage);
			m_slider.m_pos.set(m_valueLeft + SLIDER_LEN/2, 
						m_items[i].m_pos.y - (1.25f * m_items[i].GetLeading()));

			m_pEditor = &m_slider;
		}
			break;

		default:
			m_pEditor = nullptr;
			break;
	}
	if(m_pEditor != nullptr)
	{
		m_eState = State::editing;
		m_fadeInEditor = 0.0f;
	}
}


void Defcon::COptionsArena::StopEditing()
{
	//gPrefs.m_pref[sCurrentItem_arena_optons].m_fValue = m_pEditor->GetValue();
	m_eState = State::viewing;
	m_pEditor = nullptr;
}


void Defcon::COptionsArena::CancelEditing()
{
	gPrefs.m_pref[sCurrentItem_arena_optons].m_fValue = m_pEditor->GetInitialValue();
	this->UpdateValueText(sCurrentItem_arena_optons - m_topItem);
	m_eState = State::viewing;
}

#endif // 0
