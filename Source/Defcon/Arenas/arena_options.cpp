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
			options_arena_inputs() : gpArena(nullptr) 
			{
				m_size = 10;
				ScheduledTasks[0].what = EventType::navigate_up;			ScheduledTasks[0].bRepeats = true;
				ScheduledTasks[1].what = EventType::navigate_down;		ScheduledTasks[1].bRepeats = true;
				ScheduledTasks[2].what = EventType::navigate_right;		ScheduledTasks[2].bRepeats = true;
				ScheduledTasks[3].what = EventType::navigate_left;		ScheduledTasks[3].bRepeats = true;
				ScheduledTasks[4].what = EventType::select;				ScheduledTasks[4].bRepeats = false;
				ScheduledTasks[5].what = EventType::cancel;				ScheduledTasks[5].bRepeats = false;
				ScheduledTasks[6].what = EventType::navigate_home;		ScheduledTasks[6].bRepeats = false;
				ScheduledTasks[7].what = EventType::navigate_end;			ScheduledTasks[7].bRepeats = false;
				ScheduledTasks[8].what = EventType::navigate_page_up;		ScheduledTasks[8].bRepeats = false;
				ScheduledTasks[9].what = EventType::navigate_page_down;	ScheduledTasks[9].bRepeats = false;
			}
			virtual void process(const ControllerEvent&);
			Defcon::COptionsArena* gpArena;
	};
}


void Defcon::options_arena_inputs::process(const ControllerEvent& evt)
{
	check(gpArena != nullptr);

	switch(evt.what)
	{
		case Defcon::EventType::navigate_up:
			GMessageMediator.PlaySound(select);
			if(gpArena->State == Defcon::COptionsArena::State::viewing)
			{
				if(sCurrentItem_arena_optons == 0)
					gpArena->FocusItem(array_size(gPrefs.Pref)-1);
				else
					gpArena->FocusItem(sCurrentItem_arena_optons - 1);
			}
			break;


		case Defcon::EventType::navigate_down:
			GMessageMediator.PlaySound(select);
			if(gpArena->State == Defcon::COptionsArena::State::viewing)
			{
				if(sCurrentItem_arena_optons == array_size(gPrefs.Pref)-1)
					gpArena->FocusItem(0);
				else
					gpArena->FocusItem(sCurrentItem_arena_optons + 1);
			}
			break;


		case Defcon::EventType::select:
			switch(gpArena->State)
			{
				case Defcon::COptionsArena::State::viewing:
					gpArena->StartEditing();
					break;
				case Defcon::COptionsArena::State::editing:
					gpArena->StopEditing();
					break;
			}
			break;

		case Defcon::EventType::cancel:
			switch(gpArena->State)
			{
				case Defcon::COptionsArena::State::viewing:
					gpArena->m_pNextArena = Defcon::CArenaFactory::Make(Defcon::ArenaKind::menu);
					break;
				case Defcon::COptionsArena::State::editing:
					gpArena->CancelEditing();
					break;
			}
			break;

		case Defcon::EventType::navigate_left:
		case Defcon::EventType::navigate_right:
		case Defcon::EventType::navigate_home:
		case Defcon::EventType::navigate_end:
		case Defcon::EventType::navigate_page_up:
		case Defcon::EventType::navigate_page_down:
			switch(gpArena->State)
			{
				case Defcon::COptionsArena::State::viewing:
					gpArena->navigate(evt.what);
					break;

				case Defcon::COptionsArena::State::editing:
					if(gpArena->m_pEditor != nullptr)
					{
						gpArena->m_pEditor->process(evt);
						gPrefs.Pref[sCurrentItem_arena_optons].SetValue(gpArena->m_pEditor->GetValue());
						//gPrefs.OnUpdate();
						gpArena->UpdateValueText(sCurrentItem_arena_optons - gpArena->m_topItem);
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
	State = State::viewing;
	m_bLaunching = false;
	m_fLaunchAge = 0.0f;
	Age = 0.0f;
	m_topItem = 0;

	m_pParams = &(Defcon::ArenaParams&)params;

	const IntRect& r = params.r;

	const int32 w = r.right - r.left;
	const int32 h = r.bottom - r.top;

	OnDisplaySizeChanged(w, h);

	s_event_handler_arena_options.gpArena = this;
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
		Objects.Add(&m_items[i]);

		m_values[i].SetFontID(varfontid);
		m_values[i].SetExternalOwnership(true);
		m_values[i].SetColor(C_UNSELECTEDVALUE);
		Objects.Add(&m_values[i]);
	}

	m_title.SetColor(C_WHITE);
	m_title.SetFontID(EFont::body);
	m_title.SetAlignment(EAlignment::center);
	m_title.SetExternalOwnership(true);
	Objects.Add(&m_title);

	m_desc.SetColor(C_WHITE);
	m_desc.SetFontID(EFont::body);
	m_desc.SetAlignment(EAlignment::center);
	m_desc.SetExternalOwnership(true);
	Objects.Add(&m_desc);

	m_edittitle.SetText("Editing");
	m_edittitle.SetColor(C_WHITE);
	m_edittitle.SetFontID(m_items[0].GetFontID());
	m_edittitle.SetExternalOwnership(true);

	FocusItem(sCurrentItem_arena_optons);
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
			1 + array_size(gPrefs.Pref) - array_size(m_items));
	}
}

void Defcon::COptionsArena::FocusItem(int32 id)
{
	// Scroll text area if necessary.
	Scroll();

	int32 itemid = sCurrentItem_arena_optons - m_topItem;
	m_items[itemid].SetColor(C_UNSELECTED);
	m_values[itemid].SetColor(C_UNSELECTEDVALUE);
	sCurrentItem_arena_optons = id;
	Scroll();

	itemid = sCurrentItem_arena_optons - m_topItem;
	m_items[itemid].SetColor(C_SELECTED);
	m_values[itemid].SetColor(C_SELECTEDVALUE);

	// Assign pref vars names to items.
	for(int32 i = 0; i < array_size(m_items); i++)
	{
		m_items[i].SetText(gPrefs.Pref[m_topItem + i].Metadata.m_pszName);

		UpdateValueText(i);
	}

	m_desc.SetText(gPrefs.Pref[sCurrentItem_arena_optons].Metadata.Desc);
}


void Defcon::COptionsArena::UpdateValueText(int32 i)
{
	int32 v = m_topItem + i;
	char sz[50], sz2[100];
	MySprintf(sz2, "%s %s",
		gPrefs.Pref[v].GetValueText(sz),
		gPrefs.Pref[v].Metadata.Units);
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
	for(i = 0; i < array_size(gPrefs.Pref); i++)
	{
		int32 iw, ih;
		varfont.GetStringExtent(gPrefs.Pref[i].Metadata.m_pszName, iw, ih);
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
		m_items[i].Position.Set(textleft, texttop + i * leading);
		m_values[i].Position.Set(m_valueLeft, m_items[i].Position.y);
	}

	m_box.Set(w/2, h/2 - leading/4);
	m_box.Inflate(
		CFPoint(0.5f*w - textleft+100, 
		leading + 0.5f * array_size(m_items) * leading));

	m_title.Position.set(w/2, m_items[0].Position.y - leading * 3);

	m_desc.Position.Set(
		w/2, m_items[array_size(m_items)-1].Position.y + leading * 3);

	if(State == State::editing)
	{
		i = sCurrentItem_arena_optons - m_topItem;
		FPrefVar& var = gPrefs.Pref[sCurrentItem_arena_optons];

		switch(var.Metadata.m_eVarType)
		{
			case type_int:
			case type_float:
				m_slider.Position.Set(m_valueLeft + SLIDER_LEN/2, 
					m_items[i].Position.y - 
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
			FocusItem(0);
			break;

		case EventType::navigate_end:
			FocusItem(array_size(gPrefs.Pref)-1);
			break;

		case EventType::navigate_page_up:
		{
			if(sCurrentItem_arena_optons < array_size(m_items))
				FocusItem(0);
			else
				FocusItem(sCurrentItem_arena_optons - array_size(m_items));
		}
			break;

		case EventType::navigate_page_down:
		{
			int32 n = 
				sCurrentItem_arena_optons + array_size(m_items);
			if(n < array_size(gPrefs.Pref))
				FocusItem(n);
			else
				FocusItem(array_size(gPrefs.Pref)-1);
		}
			break;
	}
}


void Defcon::COptionsArena::Update(float DeltaTime)
{
	if(DeltaTime == 0)
		return;

	Age += DeltaTime;


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
		float t = fabs(sin(Age * PI*2));
		FLinearColor c = MakeBlendedColor(C_SELECTEDDARK, C_SELECTED, t);
		m_items[itemid].SetColor(c);
		c = MakeBlendedColor(C_SELECTEDDARK, C_SELECTEDVALUE, t);
		m_values[itemid].SetColor(c);
	}


	switch(State)
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

	gop.ArenaWidth		= m_virtualScreen.GetWidth();
	gop.DeltaTime	= DeltaTime;
	gop.pDib			= &m_virtualScreen;
	gop.MapperPtr			= &m_coordMapper;

	Objects.Process(gop);


	int32 x1, y1, x2, y2;
	m_box.Classicize(x1, y1, x2, y2);
	for(int32 off = 0; off < 2; off++)
		m_virtualScreen.ColorRect(
			x1+off, y1+off, x2-off, y2-off, C_BLUE);

	if(State == State::editing)
	{
		// Make a box around the current var.
		// Leave room above the var for some extra stuff.

		m_fadeInEditor += DeltaTime;
		m_fadeInEditor = FMath::Min(m_fadeInEditor, 0.33f);
		m_virtualScreen.DarkenRect(x1, y1, x2, y2, 
			MAP(m_fadeInEditor, 0.0f, 0.33f, 1.0f, 0.33f) );

		CFRect r;
		int32 i = sCurrentItem_arena_optons - m_topItem;
		r.UR = r.LL = m_items[i].Position;
		r.UR += CFPoint(-20, -(int32)m_items[i].GetLeading()*2);
		r.LL += CFPoint(540, 10);
		r.order();
		r.Classicize(x1, y1, x2, y2);
		m_virtualScreen.FillRect(x1, y1, x2, y2, C_BLACK);
		for(int32 off = 0; off < 2; off++)
			m_virtualScreen.ColorRect(
				x1+off, y1+off, x2-off, y2-off, C_BLUE);
		m_items[i].SetColor(C_WHITE);
		m_items[i].Draw(m_virtualScreen, m_coordMapper);
		m_values[i].SetColor(C_WHITE);
		m_values[i].Draw(m_virtualScreen, m_coordMapper);

		m_edittitle.Position = m_items[i].Position;
		m_edittitle.Position.y -= m_items[i].GetLeading();
		m_edittitle.Draw(m_virtualScreen, m_coordMapper);

		if(m_pEditor != nullptr)
			m_pEditor->Draw(m_virtualScreen, m_coordMapper);
	}

	FadeIn(m_virtualScreen);
}


void Defcon::COptionsArena::UpdateKybdState()
{
	if(m_bLaunching)
		return;

	IArena::UpdateKybdState();
	//UpdateKeysState(m_keys, Keys::count);
}


#if 0
void Defcon::COptionsArena::OnKeyboardEvent(int32 key)
{
	m_keys[key].fTimeLastFired = gettime_secs();

	GMessageMediator.PlaySound(select);

	switch(key)
	{
		case Keys::moveup:
			if(State == State::viewing)
			{
				if(sCurrentItem_arena_optons == 0)
					FocusItem(array_size(gPrefs.Pref)-1);
				else
					FocusItem(sCurrentItem_arena_optons - 1);
			}
			break;


		case Keys::movedown:
			if(State == State::viewing)
			{
				if(sCurrentItem_arena_optons == array_size(gPrefs.Pref)-1)
					FocusItem(0);
				else
					FocusItem(sCurrentItem_arena_optons + 1);
			}
			break;


		case Keys::moveleft:
		case Keys::moveright:
		case Keys::pageup:
		case Keys::pagedown:
		case Keys::home:
		case Keys::end:
			switch(State)
			{
				case State::viewing:
					switch(key)
					{
						case Keys::home:
							FocusItem(0);
							break;

						case Keys::end:
							FocusItem(array_size(gPrefs.Pref)-1);
							break;

						case Keys::pageup:
						{
							if(sCurrentItem_arena_optons < array_size(m_items))
								FocusItem(0);
							else
								FocusItem(sCurrentItem_arena_optons - array_size(m_items));
						}
							break;

						case Keys::pagedown:
						{
							int32 n = 
								sCurrentItem_arena_optons + array_size(m_items);
							if(n < array_size(gPrefs.Pref))
								FocusItem(n);
							else
								FocusItem(array_size(gPrefs.Pref)-1);
						}
							break;


					}
					break;

				case State::editing:
					if(m_pEditor != nullptr)
					{
						m_pEditor->OnKey(m_keys[key]);
						gPrefs.Pref[sCurrentItem_arena_optons].SetValue(m_pEditor->GetValue());
						UpdateValueText(sCurrentItem_arena_optons - m_topItem);
					}
					break;
			}
			break;


		case Keys::choose:
			switch(State)
			{
				case State::viewing:
					StartEditing();
					break;
				case State::editing:
					StopEditing();
					break;
			}
			break;


		case Keys::exit:
			switch(State)
			{
				case State::viewing:
					m_pNextArena = Defcon::CArenaFactory::Make(Defcon::ArenaKind::menu);
					break;
				case State::editing:
					CancelEditing();
					break;
			}
			break;

	}
}
#endif // 0


void Defcon::COptionsArena::StartEditing()
{
	int32 i = sCurrentItem_arena_optons - m_topItem;
	FPrefVar& var = gPrefs.Pref[sCurrentItem_arena_optons];

	switch(var.Metadata.m_eVarType)
	{
		case type_bool:
			// Just toggle the value immediately.
			var.SetValue(1.0f - var.GetValue());
			UpdateValueText(i);
			break;

		case type_choice:
			// Switch to next choice w/ wraparound.
			var.SetValue(var.GetValue() + 1.0f);
			if(var.GetValue() >= var.GetChoiceCount())
				var.SetValue(0.0f);
			UpdateValueText(i);
			break;
				

		case type_int:
		case type_float:
		{
			m_slider.SetValue(var.GetValue());

			float fInc = 1.0f;
			if(var.Metadata.m_eVarType == type_float)
				 fInc = (var.Metadata.Max - 
					var.Metadata.Min) / SLIDER_LEN;

			float fPage = (var.Metadata.Max - 
					var.Metadata.Min) / 8;
			
			m_slider.Init(SLIDER_LEN,
				var.m_fValue, 
				var.Metadata.Min,
				var.Metadata.Max, fInc, fPage);
			m_slider.Position.Set(m_valueLeft + SLIDER_LEN/2, 
						m_items[i].Position.y - (1.25f * m_items[i].GetLeading()));

			m_pEditor = &m_slider;
		}
			break;

		default:
			m_pEditor = nullptr;
			break;
	}
	if(m_pEditor != nullptr)
	{
		State = State::editing;
		m_fadeInEditor = 0.0f;
	}
}


void Defcon::COptionsArena::StopEditing()
{
	//gPrefs.Pref[sCurrentItem_arena_optons].m_fValue = m_pEditor->GetValue();
	State = State::viewing;
	m_pEditor = nullptr;
}


void Defcon::COptionsArena::CancelEditing()
{
	gPrefs.Pref[sCurrentItem_arena_optons].m_fValue = m_pEditor->GetInitialValue();
	UpdateValueText(sCurrentItem_arena_optons - m_topItem);
	State = State::viewing;
}

#endif // 0
