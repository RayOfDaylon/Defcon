// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


/*
	Preference variables for Defcon game.
	Copyright 2003-2004 Daylon Graphics Ltd.

	Most of these prefs are for tuning game
	behavior during development; most end users 
	would not be too interested in them. Others, 
	however, are essential (such as window size).
*/

#include "CoreMinimal.h"

#include "Common/variant.h"

namespace Defcon
{
	class CPrefs
	{
		public:
			CPrefs();
			virtual ~CPrefs() {}
			void Construct();

			enum Pref 
			{
				arena_bogdown,
				attack_initialdistance,

				baiter_prob,
				baiter_value,
				//baiters_in_first_wave,
				//baiters_per_attack,

				bigred_value,

				bomber_value,
				//bombers_in_first_wave,
				//bombers_per_attack,
				bouncer_value,

				bullet_damage,
				bullets_hit_terrain,

				collision_damage,
				controller_explicit_reverse,

				debris_drawradar,
				debris_dual_prob,
				debris_smoke,
				delay_before_attack,
				delay_between_reattack,

				display_blur,
				display_blur_method,
				display_fullscreen,
				display_hidewindowframe,
				display_height,
				display_interlace,
				display_width,

				dynamo_value,
				//dynamos_in_first_wave,
				//dynamos_per_attack,

				enemy_birthdebrisdist,
				enemy_birthduration,
				enemies_canthurteachother,
				enemies_minesdonthurt,

				fireball_value,
				firebomber_value,
				firebomber_speed_min,
				firebomber_speed_max,
				firebomber_travel_time_min,
				firebomber_travel_time_max,

				ghost_player_dist_min,
				ghost_value,
				guppy_value,
				guppy_speedmax,
				guppy_speedmin,

				human_bonus_value,
				human_distribution,
				human_terminalvelocity,
				human_withinrange,
				humans_count,
				human_value_debarked,
				human_value_embarked,
				human_value_liberated,

				hunter_speedmax,
				hunter_speedmin,
				hunter_value,
				//hunters_in_first_wave,
				//hunters_per_attack,

				lander_abductodds,
				lander_ascentrate,
				lander_descent_speed,
				lander_mature,
				lander_value,
				//landers_in_first_wave,
				//landers_per_attack,

				laser_extra_count,
				laser_multi_prob,

				memcopy_method,
				menutext_uppercase,

				mine_damage,
				mine_lifespan,

				missionname_uppercase,
				munchie_value,

				object_drawbbox,

				phred_value,

				planet_to_screen_ratio,

				player_birthdebrisdist,
				player_birthduration,
				player_drag,
				//player_initial_livesleft,
				player_marginsettlespeed,
				player_mass,
				player_maxthrust,
				player_posmargin,
				player_rebirth_delay,
				player_reward_points,
				pod_value,
				reformer_value,
				reformerpart_value,
				//pods_in_first_wave,
				//pods_per_attack,
				scoretips_showenemypoints,
				smartbomb_initial,
				smartbomb_lifespan,
				smartbomb_max_flashscreen,
				smartbomb_resupply,
				smartbomb_value,	
				smartbomb_wavepush,
				smartbomb_wavepushmin,
				sound_active,
				stars_count,
				spacehum_value,
				swarmer_value,
				swarmers_per_pod,

				time_precise,

				count
			};
			void Init(const FString& Str);
			CPrefVar& Translate(const FString& Str) const;


			CPrefVar	m_pref[Pref::count];
	};
}



extern Defcon::CPrefs	gPrefs;

#define CONTROLLER_EXPLICIT_REVERSE	\
	(bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::controller_explicit_reverse].m_fValue)


#define PLANET_TO_SCREEN_RATIO      gPrefs.m_pref[Defcon::CPrefs::Pref::planet_to_screen_ratio].m_fValue

#define PLAYER_REBIRTH_DELAY        gPrefs.m_pref[Defcon::CPrefs::Pref::player_rebirth_delay].m_fValue
#define PLAYER_BIRTHDURATION        gPrefs.m_pref[Defcon::CPrefs::Pref::player_birthduration].m_fValue
#define PLAYER_BIRTHDEBRISDIST      gPrefs.m_pref[Defcon::CPrefs::Pref::player_birthdebrisdist].m_fValue
#define PLAYER_REWARD_POINTS        (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::player_reward_points].m_fValue
#define PLAYER_POSMARGIN            gPrefs.m_pref[Defcon::CPrefs::Pref::player_posmargin].m_fValue
#define PLAYER_MARGINSETTLESPEED    gPrefs.m_pref[Defcon::CPrefs::Pref::player_marginsettlespeed].m_fValue
#define PLAYER_MAXTHRUST            gPrefs.m_pref[Defcon::CPrefs::Pref::player_maxthrust].m_fValue
#define PLAYER_MASS                 gPrefs.m_pref[Defcon::CPrefs::Pref::player_mass].m_fValue
#define PLAYER_DRAG                 gPrefs.m_pref[Defcon::CPrefs::Pref::player_drag].m_fValue

#define SMARTBOMB_INITIAL           (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_initial].m_fValue
#define SMARTBOMB_VALUE             (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_value].m_fValue
#define SMARTBOMB_RESUPPLY          (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_resupply].m_fValue
#define SMARTBOMB_LIFESPAN          gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_lifespan].m_fValue
#define SMARTBOMB_WAVEPUSH          gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_wavepush].m_fValue//200.0f
#define SMARTBOMB_WAVEPUSHMIN       gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_wavepushmin].m_fValue//100.0f
#define SMARTBOMB_MAX_FLASHSCREEN   (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_max_flashscreen].m_fValue

#define HUMANS_COUNT                (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::humans_count].m_fValue
#define HUMAN_WITHINRANGE           gPrefs.m_pref[Defcon::CPrefs::Pref::human_withinrange].m_fValue
#define HUMAN_DISTRIBUTION          gPrefs.m_pref[Defcon::CPrefs::Pref::human_distribution].m_fValue
#define HUMAN_TERMINALVELOCITY      gPrefs.m_pref[Defcon::CPrefs::Pref::human_terminalvelocity].m_fValue
#define HUMAN_BONUS_VALUE           (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::human_bonus_value].m_fValue


// When mission starts or enemies run low, 
// how much time before (more) enemies appear.
#define DELAY_BEFORE_ATTACK         gPrefs.m_pref[Defcon::CPrefs::Pref::delay_before_attack].m_fValue

// When enemies plentiful, how much time 
// before more enemies appear.
#define DELAY_BETWEEN_REATTACK      gPrefs.m_pref[Defcon::CPrefs::Pref::delay_between_reattack].m_fValue

#define LANDER_DESCENT_SPEED        gPrefs.m_pref[Defcon::CPrefs::Pref::lander_descent_speed].m_fValue

/*
#define LANDERS_PER_ATTACK          (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::landers_per_attack].m_fValue
#define BOMBERS_PER_ATTACK          (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::bombers_per_attack].m_fValue
#define HUNTERS_PER_ATTACK          (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::hunters_per_attack].m_fValue
#define BAITERS_PER_ATTACK          (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::baiters_per_attack].m_fValue
#define FIREBOMBERS_PER_ATTACK      (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::firebombers_per_attack].m_fValue
#define DYNAMOS_PER_ATTACK          (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::dynamos_per_attack].m_fValue
#define PODS_PER_ATTACK             (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::pods_per_attack].m_fValue
*/
#define ATTACK_INITIALDISTANCE      gPrefs.m_pref[Defcon::CPrefs::Pref::attack_initialdistance].m_fValue

/*
#define LANDERS_IN_FIRST_WAVE       (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::landers_in_first_wave].m_fValue
#define HUNTERS_IN_FIRST_WAVE       (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::hunters_in_first_wave].m_fValue
#define BOMBERS_IN_FIRST_WAVE       (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::bombers_in_first_wave].m_fValue
#define BAITERS_IN_FIRST_WAVE       (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::baiters_in_first_wave].m_fValue
#define FIREBOMBERS_IN_FIRST_WAVE   (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::firebombers_in_first_wave].m_fValue
#define DYNAMOS_IN_FIRST_WAVE       (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::dynamos_in_first_wave].m_fValue
#define PODS_IN_FIRST_WAVE          (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::pods_in_first_wave].m_fValue
*/


#define ARENA_BOGDOWN              (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::arena_bogdown].m_fValue)
#define BAITER_PROB                gPrefs.m_pref[Defcon::CPrefs::Pref::baiter_prob].m_fValue
#define BAITER_VALUE               (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::baiter_value].m_fValue
#define BIGRED_VALUE               (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::bigred_value].m_fValue
#define BOMBER_VALUE               (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::bomber_value].m_fValue
#define BOUNCER_VALUE              (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::bouncer_value].m_fValue
#define BULLET_DAMAGE              (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::bullet_damage].m_fValue
#define BULLETS_HIT_TERRAIN        (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::bullets_hit_terrain].m_fValue)
#define COLLISION_DAMAGE           (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::collision_damage].m_fValue
#define DEBRIS_DRAWRADAR           (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::debris_drawradar].m_fValue)
#define DEBRIS_DUAL_PROB           gPrefs.m_pref[Defcon::CPrefs::Pref::debris_dual_prob].m_fValue
#define DEBRIS_SMOKE               (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::debris_smoke].m_fValue)
#define DISPLAY_BLUR               (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::display_blur].m_fValue
#define DISPLAY_BLUR_METHOD        ((int)gPrefs.m_pref[Defcon::CPrefs::Pref::display_blur_method].m_fValue)
#define DISPLAY_FULLSCREEN         (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::display_fullscreen].m_fValue)
#define DISPLAY_HEIGHT             (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::display_height].m_fValue
#define DISPLAY_HIDEWINDOWFRAME    (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::display_hidewindowframe].m_fValue)
#define DISPLAY_INTERLACE          (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::display_interlace].m_fValue)
#define DISPLAY_WIDTH              (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::display_width].m_fValue
#define DYNAMO_VALUE               (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::dynamo_value].m_fValue
#define ENEMIES_CANTHURTEACHOTHER  (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::enemies_canthurteachother].m_fValue)
#define ENEMIES_MINESDONTHURT      (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::enemies_minesdonthurt].m_fValue)
#define ENEMY_BIRTHDEBRISDIST      gPrefs.m_pref[Defcon::CPrefs::Pref::enemy_birthdebrisdist].m_fValue
#define ENEMY_BIRTHDURATION        gPrefs.m_pref[Defcon::CPrefs::Pref::enemy_birthduration].m_fValue
#define FIREBALL_VALUE             (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::fireball_value].m_fValue

#define FIREBOMBER_SPEED_MIN       gPrefs.m_pref[Defcon::CPrefs::Pref::firebomber_speed_min].m_fValue
#define FIREBOMBER_SPEED_MAX       gPrefs.m_pref[Defcon::CPrefs::Pref::firebomber_speed_max].m_fValue
#define FIREBOMBER_TRAVEL_TIME_MIN gPrefs.m_pref[Defcon::CPrefs::Pref::firebomber_travel_time_min].m_fValue
#define FIREBOMBER_TRAVEL_TIME_MAX gPrefs.m_pref[Defcon::CPrefs::Pref::firebomber_travel_time_max].m_fValue

#define FIREBOMBER_VALUE           (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::firebomber_value].m_fValue


#define GHOST_PLAYER_DIST_MIN      (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::ghost_player_dist_min].m_fValue 
#define GHOST_VALUE                (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::ghost_value].m_fValue 
#define GUPPY_SPEEDMAX             gPrefs.m_pref[Defcon::CPrefs::Pref::guppy_speedmax].m_fValue
#define GUPPY_SPEEDMIN             gPrefs.m_pref[Defcon::CPrefs::Pref::guppy_speedmin].m_fValue
#define GUPPY_VALUE                (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::guppy_value].m_fValue
#define HUMAN_VALUE_DEBARKED       gPrefs.m_pref[Defcon::CPrefs::Pref::human_value_debarked].m_fValue
#define HUMAN_VALUE_EMBARKED       gPrefs.m_pref[Defcon::CPrefs::Pref::human_value_embarked].m_fValue
#define HUMAN_VALUE_LIBERATED      gPrefs.m_pref[Defcon::CPrefs::Pref::human_value_liberated].m_fValue
#define HUNTER_SPEEDMAX            gPrefs.m_pref[Defcon::CPrefs::Pref::hunter_speedmax].m_fValue
#define HUNTER_SPEEDMIN            gPrefs.m_pref[Defcon::CPrefs::Pref::hunter_speedmin].m_fValue
#define HUNTER_VALUE               (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::hunter_value].m_fValue
#define LANDER_ABDUCTODDS          gPrefs.m_pref[Defcon::CPrefs::Pref::lander_abductodds].m_fValue
#define LANDER_ASCENTRATE          gPrefs.m_pref[Defcon::CPrefs::Pref::lander_ascentrate].m_fValue
#define LANDER_MATURE              gPrefs.m_pref[Defcon::CPrefs::Pref::lander_mature].m_fValue
#define LANDER_VALUE               (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::lander_value].m_fValue
#define LASER_EXTRA_COUNT          (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::laser_extra_count].m_fValue
#define LASER_MULTI_PROB           gPrefs.m_pref[Defcon::CPrefs::Pref::laser_multi_prob].m_fValue
#define MEMCOPY_METHOD             (MemcopyMethod)((int)gPrefs.m_pref[Defcon::CPrefs::Pref::memcopy_method].m_fValue)
#define MENUTEXT_UPPERCASE         (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::menutext_uppercase].m_fValue)
#define MINE_DAMAGE                (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::mine_damage].m_fValue
#define MINE_LIFESPAN              gPrefs.m_pref[Defcon::CPrefs::Pref::mine_lifespan].m_fValue
#define MISSIONNAME_UPPERCASE      (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::missionname_uppercase].m_fValue)
#define MUNCHIE_VALUE              (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::munchie_value].m_fValue
#define OBJECT_DRAWBBOX            (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::object_drawbbox].m_fValue)
#define PHRED_VALUE                (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::phred_value].m_fValue
#define POD_VALUE                  (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::pod_value].m_fValue
#define REFORMER_VALUE             (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::reformer_value].m_fValue 
#define REFORMERPART_VALUE         (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::reformerpart_value].m_fValue 
#define SCORETIPS_SHOWENEMYPOINTS  (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::scoretips_showenemypoints].m_fValue)
#define SOUND_ACTIVE               (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::sound_active].m_fValue)
#define SPACEHUM_VALUE             (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::spacehum_value].m_fValue
#define STARS_COUNT                (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::stars_count].m_fValue
#define SWARMER_VALUE              (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::swarmer_value].m_fValue
#define SWARMERS_PER_POD           (size_t)gPrefs.m_pref[Defcon::CPrefs::Pref::swarmers_per_pod].m_fValue
#define TIME_PRECISE               (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::time_precise].m_fValue)

