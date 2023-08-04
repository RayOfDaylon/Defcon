// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "prefs.h"

#include "Common/util_core.h"
#include "Common/util_str.h"

Defcon::CPrefs	GPrefs;


Defcon::CPrefs::CPrefs() {}


void Defcon::CPrefs::Construct()
{
	// Set default values for preferences.

	// Can't do this in ctor because we refer to 
	// objects, such as gpApp, that don't exist until 
	// later. Unless we want to make gPrefs a var which 
	// is instanced explicitly by the app.

	// Note: read "px" and "pixels" as being Slate units, not device pixels.

#define INITPREF(_key, _val, _units, _type, _min, _max, _desc)	m_pref[_key].Init(_val, _units, _type, _min, _max, #_key, _desc)
#define INITPREF2(_key, _val, _units, _type, _min, _max, _desc, _ex1, _ex2, _ex3)	m_pref[_key].Init(_val, _units, _type, _min, _max, #_key, _desc, _ex1, _ex2, _ex3)

    //        Key                          Value     Units           Type          Min          Max       Description
	//        ---------------------------  -----     -------------   ----------- ---------  ------------  --------------------------------------------------------------------------------------------

	INITPREF( planet_to_screen_ratio,       5.0f,    "%",            type_float,     2.0f,        6.0f,   "How wide the planet surface is relative to visible portion"                                );

	INITPREF( arena_bogdown,                0,       "",             type_bool,      0,           0,      "Slow game down when lots of objects and debris are visible"                                );
	INITPREF( attack_initialdistance,       1.0f,    "%",            type_float,     0.1f,        1.0f,   "Probability of enemy materializing far away from player"                                   );
	INITPREF( mine_lifespan,               10.0f,    "seconds",      type_float,     1.0f,       30.0f,   "How long a mine exists after being deployed"                                               );
	INITPREF( controller_explicit_reverse,  0.0f,    "",             type_bool,      0.0f,        0.0f,   "Controller uses explicit reverse button"                                                   );
	INITPREF( player_rebirth_delay,         2.0f,    "seconds",      type_float,     0.1f,        5.0f,   "Delay between player destruction and materialization"                                      );
	INITPREF( player_birthduration,         3.0f,    "seconds",      type_float,     0.1f,        5.0f,   "How long for player ship to materialize"                                                   );
	INITPREF( player_birthdebrisdist,    1250.0f,    "px",           type_float,     0.1f,        5.0f,   "Initial size of player ship's materialization field"                                       );
	INITPREF( player_reward_points,     10000.0f,    "",             type_int,    1000,      100000,      "Number of points needed to earn an extra ship"                                             );
	INITPREF( player_posmargin,           400.0f,    "px",           type_int,      50,         500,      "Minimum distance between player's ship and edge of screen"                                 );
	INITPREF( player_marginsettlespeed,  1400.0f,    "px/sec",       type_int,      50,        3000,      "How quickly the player ship settles to the edge of the screen after reversing direction"   );
	//INITPREF( player_maxthrust,             3.5f,    "",             type_float,     0.1f,        5.0f,   "Maximum thrust force applicable to player ship"                                            );
	INITPREF( player_maxthrust,             4.5f,    "",             type_float,     0.1f,        5.0f,   "Maximum thrust force applicable to player ship"                                            );
	INITPREF( player_mass,                  0.08f,   "",             type_float,     0.001f,      1.0f,   "Mass of player ship"                                                                       );
	INITPREF( player_drag,                  0.06f,   "",             type_float,     0.001f,      1.0f,   "Drag acting upon player ship"                                                              );
	INITPREF( smartbomb_initial,            3.0f,    "",             type_int,       0,          20,      "Initial number of smart bombs when starting a game"                                        );
	INITPREF( smartbomb_value,          10000.0f,    "",             type_int,    1000,      100000,      "Number of points needed to earn extra smart bombs"                                         );
	INITPREF( smartbomb_resupply,           1,       "",             type_int,       1,          10,      "Number of smart bombs given when earned"                                                   );
	INITPREF( smartbomb_lifespan,           0.75f,   "seconds",      type_float,     0.1f,        2.0f,   "How long a smartbomb shockwave persists"                                                   );
	INITPREF( smartbomb_wavepush,          30.0f,    "px/frame",     type_float,     5.0f,      100.0f,   "Debris near player is pushed at this speed by smartbomb shockwave"                         );
	INITPREF( smartbomb_wavepushmin,       15.0f,    "px/frame",     type_float,     5.0f,       50.0f,   "Debris is additionally pushed by smartbomb shockwave this fast regardless of location"     );
	INITPREF( smartbomb_max_flashscreen,   15,       "frames",       type_int,       1,          50,      "How long a smartbomb shockwave is visible"                                                 );
	INITPREF( humans_count,                15,       "",             type_int,       1,         100,      "Number of humanoids at start of game"                                                      );
	INITPREF( human_distribution,           0.5f,    "",             type_float,     0.5f,        1.0f,   "How far to distribute humans over planet surface"                                          );
	INITPREF( human_withinrange,           50,       "px",           type_int,      10,         500,      "Distance a humanoid must be to lander to be of interest"                                   );
	INITPREF( human_terminalvelocity,      90,       "px/sec",       type_int,       1,         500,      "Speed at which a falling humanoid dies if it has not yet reached the ground"               );
	INITPREF( human_bonus_value,          100,       "",             type_int,       1,         500,      "Bonus points for each humanoid alive at end of mission"                                    );
	INITPREF( human_value_debarked,       250,       "",             type_int,       0,        1000,      "Points earned when a humanoid is flown back to the ground"                                 );
	INITPREF( human_value_embarked,       500,       "",             type_int,       0,        1000,      "Points earned when a humanoid is picked up in the air"                                     );
	INITPREF( human_value_liberated,      250,       "",             type_int,       0,        1000,      "Points earned when a humanoid falls safely to the ground"                                  );
	INITPREF( delay_before_attack,          3.0f,    "seconds",      type_float,     0.1f,       10.0f,   "When mission starts or enemies run low, how much time before (more) enemies appear"        );
	INITPREF( delay_between_reattack,      10.0f,    "seconds",      type_float,     5.0,        60.0f,   "When enemies plentiful, how much time before reinforcements arrive"                        );
	INITPREF( lander_descent_speed,        45.0f,    "px/sec",       type_float,     5.0,       100.0f,   "How quickly landers descend to the ground"                                                 );
	INITPREF( scoretips_showenemypoints,    0,       "",             type_bool,      0,           0,      "Show floating score text when enemy destroyed"                                             );
	INITPREF( lander_ascentrate,           60.0f,    "px/sec",       type_float,    10.0f,      300.0f,   "How quickly a lander ascends after abducting a humanoid"                                   );
	INITPREF( lander_abductodds,            0.0001f, "%",            type_float,     0.00001f,    0.001f, "Probability per frame that a humanoid within range will be abducted"                       );
	INITPREF( lander_mature,               20.0f,    "seconds",      type_float,     1.0f,       60.0f,   "How old a lander has to be before it will definitely abduct a humanoid"                    );
	INITPREF( object_drawbbox,              0,       "",             type_bool,      0,           0,      "Show sprite bounding boxes (debugging aid)"                                                );
	INITPREF( debris_drawradar,             0,       "",             type_bool,      0,           0,      "Show debris particles on radar display"                                                    );
	INITPREF( debris_dual_prob,             0.75f,   "%",            type_float,     0.0f,        1.0f,   "Probablity that an explosion ejects double the normal debris"                              );
	INITPREF( debris_smoke,                 0,       "",             type_bool,      0,           0,      "Show smoke effects when an object explodes"                                                );
	INITPREF( sound_active,                 1,       "",             type_bool,      0,           0,      "Enable sound effects (disable, e.g., if you want to run a music program while playing)"    );
	INITPREF( display_fullscreen,           1,       "",             type_bool,      0,           0,      "Program window takes entire screen (requires restart)"                                     );
	INITPREF( display_hidewindowframe,      1,       "",             type_bool,      0,           0,      "Program window positions its menu and borders offscreen (requires restart)"                );
	INITPREF( display_blur,                 0,       "",             type_int,       0,           4,      "Set display blurring strength (depends on blur method)"                                    );
	INITPREF( display_interlace,            0,       "",             type_bool,      0,           0,      "Enable visible scanline display effect"                                                    );
	INITPREF( lander_value,               150,       "",             type_int,       0,        1000,      "Number of points earned for destroying a lander"                                           );
	INITPREF( laser_age_max,                0.5f,    "seconds",      type_float,     0.1f,        1.0f,   "Longest length of time a laser beam can exist"                                             );
	INITPREF( laser_extra_count,            1,       "",             type_int,       1,           4,      "Number of extra rounds per laser shot"                                                     );
	INITPREF( laser_multi_prob,             0.25f,   "%",            type_float,     0.0f,        1.0f,   "Probablity that extra laser rounds occur per shot"                                         );
	INITPREF( bomber_value,               250,       "",             type_int,       0,        1000,      "Number of points earned for destroying a bomber"                                           );

	INITPREF( firebomber_speed_min,       250,       "px/sec",       type_float,     0,        1000,      "Slowest speed a firebomber can travel"                                                     );
	INITPREF( firebomber_speed_max,       500,       "px/sec",       type_float,     0,        1000,      "Fastest speed a firebomber can travel"                                                     );
	INITPREF( firebomber_travel_time_min,   0.1f,    "seconds",      type_float,     0,        1000,      "Shortest time a firebomber can travel along current path"                                  );
	INITPREF( firebomber_travel_time_max,   0.4f,    "seconds",      type_float,     0,        1000,      "Longest time a firebomber can travel along current path"                                   );
	INITPREF( firebomber_value,           250,       "",             type_int,       0,        1000,      "Number of points earned for destroying a firebomber"                                       );

	INITPREF( bouncer_value,              200,       "",             type_int,       0,        1000,      "Number of points earned for destroying a bouncer"                                          );
	INITPREF( fireball_value,             100,       "",             type_int,       0,        1000,      "Number of points earned for destroying a fireball"                                         );
	INITPREF( fireballs_explode_on_ground,  1,       "",             type_bool,      0,           0,      "If fireballs should explode when hitting the ground"                                       );
	INITPREF( guppy_value,                200,       "",             type_int,       0,        1000,      "Number of points earned for destroying a space guppy"                                      );
	INITPREF( hunter_value,               250,       "",             type_int,       0,        1000,      "Number of points earned for destroying a hunter"                                           );
	INITPREF( baiter_prob,                  0.33f,   "%",            type_float,     0,           1,      "Probability of baiter appearing near end of mission"                                       );
	INITPREF( baiter_spawn_frequency,       2.0f,    "seconds",      type_float,     0.5f,       10.0f,   "Number of seconds before another baiter is spawned"                                        );
	INITPREF( baiter_value,               200,       "",             type_int,       0,        1000,      "Number of points earned for destroying a baiter"                                           );
	INITPREF( phred_value,                200,       "",             type_int,       0,        1000,      "Number of points earned for destroying a Phred"                                            );
	INITPREF( bigred_value,               200,       "",             type_int,       0,        1000,      "Number of points earned for destroying a Big Red"                                          );
	INITPREF( munchie_value,               50,       "",             type_int,       0,        1000,      "Number of points earned for destroying a munchie"                                          );
	INITPREF( pod_value,                 1000,       "",             type_int,       0,        1000,      "Number of points earned for destroying a pod"                                              );
	INITPREF( swarmer_value,              150,       "",             type_int,       0,        1000,      "Number of points earned for destroying a swarmer"                                          );
	INITPREF( swarmers_per_pod,             6,       "",             type_int,       0,          20,      "Number of swarmers a destroyed pod releases"                                               );
	INITPREF( dynamo_value,               150,       "",             type_int,       0,        1000,      "Number of points earned for destroying a dynamo"                                           );
	INITPREF( spacehum_value,             100,       "",             type_int,       0,        1000,      "Number of points earned for destroying a space hum"                                        );
	INITPREF( ghost_value,                250,       "",             type_int,       0,        1000,      "Number of points earned for destroying a ghost"                                            );
	INITPREF( ghost_player_dist_min,      400,       "px",           type_int,       0,        1000,      "Distance from player at which ghost will split"                                            ); 
	INITPREF( reformer_value,             250,       "",             type_int,       0,        1000,      "Number of points earned for destroying a reformer"                                         );
	INITPREF( reformerpart_value,          50,       "",             type_int,       0,        1000,      "Number of points earned for destroying a reformer part"                                    );
	INITPREF( enemy_birthduration,          0.7f,    "seconds",      type_float,     0.1f,        3.0f,   "Time an enemy takes to materialize"                                                        );
	INITPREF( enemy_birthdebrisdist,      500,       "px",           type_int,      50,        1200,      "Initial size of enemy materialization field"                                               );
	INITPREF( enemies_canthurteachother,    1,       "",             type_bool,      0,           0,      "Enemies do not experience friendly fire"                                                   );
	INITPREF( enemies_minesdonthurt,        1,       "",             type_bool,      0,           0,      "Enemies are not damaged by mines"                                                          );
	INITPREF( bullet_damage,               20,       "%",            type_int,       0,         100,      "Shield loss when object hit by a bullet"                                                   );
	INITPREF( collision_damage,            20,       "%",            type_int,       0,         100,      "Shield loss when objects collide"                                                          );
	INITPREF( mine_damage,                 30,       "%",            type_int,       0,         100,      "Shield loss when object hits a mine"                                                       );
	INITPREF( guppy_speedmin,             150.0f,    "px/sec",       type_float,    50,         500,      "Slowest speed at which a guppy will fly"                                                   );
	INITPREF( guppy_speedmax,             300.0f,    "px/sec",       type_float,    50,         800,      "Fastest speed at which a guppy will fly"                                                   );
	INITPREF( hunter_speedmin,            200.0f,    "px/sec",       type_float,    50,         500,      "Slowest speed at which a hunter will fly"                                                  );
	INITPREF( hunter_speedmax,            400.0f,    "px/sec",       type_float,    50,         800,      "Fastest speed at which a hunter will fly"                                                  );
	INITPREF( stars_count,                100,       "",             type_int,       0,        1000,      "Number of background stars per 100 x 100 pixel area"                                       );
	INITPREF( bullets_hit_terrain,          1,       "",             type_bool,      0,           0,      "Make enemy bullets unable to penetrate terrain"                                            );
	INITPREF( menutext_uppercase,           0,       "",             type_bool,      0,           0,      "Make menu text appear uppercase"                                                           );
	INITPREF( missionname_uppercase,        0,       "",             type_bool,      0,           0,      "Make mission name text appear uppercase"                                                   );
	INITPREF( time_precise,                 0,       "",             type_bool,      0,           0,      "Use high-precision timing APIs (may not work on some PCs)"                                 );


	static CChoiceNames memcpy_choices;
	memcpy_choices.Strings.Add("stdlib memcpy");
	memcpy_choices.Strings.Add("FP registers");
	memcpy_choices.Strings.Add("MMX/SSE");
	memcpy_choices.Strings.Add("AMD");
	INITPREF2(memcopy_method, 
		0,
		"", type_choice, 0, 0,
		"Memory copy method", &memcpy_choices, nullptr, nullptr);

	static CChoiceNames blur_choices;
	blur_choices.Strings.Add("Repeated two-sample");
	blur_choices.Strings.Add("One-time four-sample");
	blur_choices.Strings.Add("One-time eight-sample");
	INITPREF2(display_blur_method, 
		0,
		"", type_choice, 0, 0,
		"Display blur method (if blur strength greater than zero)", &blur_choices, nullptr, nullptr);

/*
	INITPREF2(display_width,             900,       "pixels",       type_int,     500,  (float)gpApp->m_devw, "Width of game display",  nullptr, OnDisplaySizeChanged, gpApp);
	INITPREF2(display_height,            475,       "pixels",       type_int,     350,  (float)gpApp->m_devh, "Height of game display", nullptr, OnDisplaySizeChanged, gpApp);
*/


/*	m_pref[player_initial_livesleft, 
		3.0f,
		"", type_int, 1, 10,
		"player_initial_livesleft",
		"Number of ships player given at start of game");
*/

/*	m_pref[landers_per_attack, 
		3,
		"landers_per_attack",
		"Number of landers in each attack wave",
		"", type_int, 0, 20);

	m_pref[bombers_per_attack, 
		2,
		"bombers_per_attack",
		"Number of bombers in each attack wave",
		"", type_int, 0, 20);

	m_pref[hunters_per_attack, 
		2,
		"hunters_per_attack",
		"Number of hunters in each attack wave",
		"", type_int, 0, 20);

	m_pref[baiters_per_attack, 
		0,
		"baiters_per_attack",
		"Number of baiters in each attack wave",
		"", type_int, 0, 20);

	m_pref[firebombers_per_attack, 
		1,
		"firebombers_per_attack",
		"Number of firebombers in each attack wave",
		"", type_int, 0, 20);

	m_pref[dynamos_per_attack, 
		2,
		"dynamos_per_attack",
		"Number of dynamos in each attack wave",
		"", type_int, 0, 20);

	m_pref[pods_per_attack, 
		1,
		"pods_per_attack",
		"Number of pods in each attack wave",
		"", type_int, 0, 20);
*/

/*
	m_pref[landers_in_first_wave, 
		9,
		"landers_in_first_wave",
		"Number of landers in first mission",
		"", type_int, 0, 20);

	m_pref[bombers_in_first_wave, 
		2,
		"bombers_in_first_wave",
		"Number of bombers in first mission",
		"", type_int, 0, 20);

	m_pref[hunters_in_first_wave, 
		3,
		"hunters_in_first_wave",
		"Number of hunters in first mission",
		"", type_int, 0, 20);

	m_pref[baiters_in_first_wave, 
		0,
		"baiters_in_first_wave",
		"Number of baiters in first mission",
		"", type_int, 0, 20);

	m_pref[firebombers_in_first_wave, 
		1,
		"firebombers_in_first_wave",
		"Number of firebombers in first mission",
		"", type_int, 0, 20);

	m_pref[dynamos_in_first_wave, 
		1,
		"dynamos_in_first_wave",
		"Number of dynamos in first mission",
		"", type_int, 0, 20);

	m_pref[pods_in_first_wave, 
		1,
		"pods_in_first_wave",
		"Number of pods in first mission",
		"", type_int, 0, 20);
*/

#undef INITPREF
#undef INITPREF2
}


void Defcon::CPrefs::Init(const FString& psz)
{
	Construct();
		// todo: use UE file read API
#if 0		
	CDiskFile f;
	f.Open(psz, "rb");

	char buf[MP_MAXLINELEN + 1];

	while(f.ReadLine(buf))
	{
		trimwhitespace(buf);
		int32 n = strlen(buf);
		if(n == 0)
			continue;

		if(n >= 2 && memcmp(buf, "//", 2) == 0)
			continue;

		char* token = MyStrtok(buf, " \t=\r\n");
		try
		{
			CPrefVar& var = Translate(token);
			token = MyStrtok(nullptr, " \t=\r\n");
			if(!isnum(token))
				throw 0;
			var.m_fValue = (float)atof(token);
		}
		catch(...) {}
	}
#endif
}


CPrefVar& Defcon::CPrefs::Translate(const FString& psz) const
{
	for(int32 i = 0; i < Pref::count; i++)
	{
		if(m_pref[i].Is(psz))
		{
			return (CPrefVar&)(m_pref[i]);
		}
	}
	check(false);
	throw 0;
}
