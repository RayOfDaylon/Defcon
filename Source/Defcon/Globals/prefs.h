// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once


/*
	Preference variables for Defcon game.

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
				baiter_spawn_frequency,
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
	(bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::controller_explicit_reverse].Value)


#define PLANET_TO_SCREEN_RATIO      gPrefs.m_pref[Defcon::CPrefs::Pref::planet_to_screen_ratio].Value

#define PLAYER_REBIRTH_DELAY        gPrefs.m_pref[Defcon::CPrefs::Pref::player_rebirth_delay].Value
#define PLAYER_BIRTHDURATION        gPrefs.m_pref[Defcon::CPrefs::Pref::player_birthduration].Value
#define PLAYER_BIRTHDEBRISDIST      gPrefs.m_pref[Defcon::CPrefs::Pref::player_birthdebrisdist].Value
#define PLAYER_REWARD_POINTS        (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::player_reward_points].Value
#define PLAYER_POSMARGIN            gPrefs.m_pref[Defcon::CPrefs::Pref::player_posmargin].Value
#define PLAYER_MARGINSETTLESPEED    gPrefs.m_pref[Defcon::CPrefs::Pref::player_marginsettlespeed].Value
#define PLAYER_MAXTHRUST            gPrefs.m_pref[Defcon::CPrefs::Pref::player_maxthrust].Value
#define PLAYER_MASS                 gPrefs.m_pref[Defcon::CPrefs::Pref::player_mass].Value
#define PLAYER_DRAG                 gPrefs.m_pref[Defcon::CPrefs::Pref::player_drag].Value

#define SMARTBOMB_INITIAL           (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_initial].Value
#define SMARTBOMB_VALUE             (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_value].Value
#define SMARTBOMB_RESUPPLY          (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_resupply].Value
#define SMARTBOMB_LIFESPAN          gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_lifespan].Value
#define SMARTBOMB_WAVEPUSH          gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_wavepush].Value//200.0f
#define SMARTBOMB_WAVEPUSHMIN       gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_wavepushmin].Value//100.0f
#define SMARTBOMB_MAX_FLASHSCREEN   (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::smartbomb_max_flashscreen].Value

#define HUMANS_COUNT                (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::humans_count].Value
#define HUMAN_WITHINRANGE           gPrefs.m_pref[Defcon::CPrefs::Pref::human_withinrange].Value
#define HUMAN_DISTRIBUTION          gPrefs.m_pref[Defcon::CPrefs::Pref::human_distribution].Value
#define HUMAN_TERMINALVELOCITY      gPrefs.m_pref[Defcon::CPrefs::Pref::human_terminalvelocity].Value
#define HUMAN_BONUS_VALUE           (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::human_bonus_value].Value


// When mission starts or enemies run low, 
// how much time before (more) enemies appear.
#define DELAY_BEFORE_ATTACK         gPrefs.m_pref[Defcon::CPrefs::Pref::delay_before_attack].Value

// When enemies plentiful, how much time 
// before more enemies appear.
#define DELAY_BETWEEN_REATTACK      gPrefs.m_pref[Defcon::CPrefs::Pref::delay_between_reattack].Value

#define LANDER_DESCENT_SPEED        gPrefs.m_pref[Defcon::CPrefs::Pref::lander_descent_speed].Value

/*
#define LANDERS_PER_ATTACK          (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::landers_per_attack].Value
#define BOMBERS_PER_ATTACK          (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::bombers_per_attack].Value
#define HUNTERS_PER_ATTACK          (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::hunters_per_attack].Value
#define BAITERS_PER_ATTACK          (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::baiters_per_attack].Value
#define FIREBOMBERS_PER_ATTACK      (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::firebombers_per_attack].Value
#define DYNAMOS_PER_ATTACK          (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::dynamos_per_attack].Value
#define PODS_PER_ATTACK             (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::pods_per_attack].Value
*/
#define ATTACK_INITIALDISTANCE      gPrefs.m_pref[Defcon::CPrefs::Pref::attack_initialdistance].Value

/*
#define LANDERS_IN_FIRST_WAVE       (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::landers_in_first_wave].Value
#define HUNTERS_IN_FIRST_WAVE       (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::hunters_in_first_wave].Value
#define BOMBERS_IN_FIRST_WAVE       (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::bombers_in_first_wave].Value
#define BAITERS_IN_FIRST_WAVE       (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::baiters_in_first_wave].Value
#define FIREBOMBERS_IN_FIRST_WAVE   (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::firebombers_in_first_wave].Value
#define DYNAMOS_IN_FIRST_WAVE       (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::dynamos_in_first_wave].Value
#define PODS_IN_FIRST_WAVE          (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::pods_in_first_wave].Value
*/


#define ARENA_BOGDOWN              (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::arena_bogdown].Value)
#define BAITER_PROB                gPrefs.m_pref[Defcon::CPrefs::Pref::baiter_prob].Value
#define BAITER_SPAWN_FREQUENCY     gPrefs.m_pref[Defcon::CPrefs::Pref::baiter_spawn_frequency].Value
#define BAITER_VALUE               (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::baiter_value].Value
#define BIGRED_VALUE               (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::bigred_value].Value
#define BOMBER_VALUE               (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::bomber_value].Value
#define BOUNCER_VALUE              (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::bouncer_value].Value
#define BULLET_DAMAGE              (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::bullet_damage].Value
#define BULLETS_HIT_TERRAIN        (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::bullets_hit_terrain].Value)
#define COLLISION_DAMAGE           (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::collision_damage].Value
#define DEBRIS_DRAWRADAR           (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::debris_drawradar].Value)
#define DEBRIS_DUAL_PROB           gPrefs.m_pref[Defcon::CPrefs::Pref::debris_dual_prob].Value
#define DEBRIS_SMOKE               (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::debris_smoke].Value)
#define DISPLAY_BLUR               (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::display_blur].Value
#define DISPLAY_BLUR_METHOD        ((int)gPrefs.m_pref[Defcon::CPrefs::Pref::display_blur_method].Value)
#define DISPLAY_FULLSCREEN         (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::display_fullscreen].Value)
#define DISPLAY_HEIGHT             (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::display_height].Value
#define DISPLAY_HIDEWINDOWFRAME    (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::display_hidewindowframe].Value)
#define DISPLAY_INTERLACE          (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::display_interlace].Value)
#define DISPLAY_WIDTH              (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::display_width].Value
#define DYNAMO_VALUE               (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::dynamo_value].Value
#define ENEMIES_CANTHURTEACHOTHER  (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::enemies_canthurteachother].Value)
#define ENEMIES_MINESDONTHURT      (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::enemies_minesdonthurt].Value)
#define ENEMY_BIRTHDEBRISDIST      gPrefs.m_pref[Defcon::CPrefs::Pref::enemy_birthdebrisdist].Value
#define ENEMY_BIRTHDURATION        gPrefs.m_pref[Defcon::CPrefs::Pref::enemy_birthduration].Value
#define FIREBALL_VALUE             (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::fireball_value].Value

#define FIREBOMBER_SPEED_MIN       gPrefs.m_pref[Defcon::CPrefs::Pref::firebomber_speed_min].Value
#define FIREBOMBER_SPEED_MAX       gPrefs.m_pref[Defcon::CPrefs::Pref::firebomber_speed_max].Value
#define FIREBOMBER_TRAVEL_TIME_MIN gPrefs.m_pref[Defcon::CPrefs::Pref::firebomber_travel_time_min].Value
#define FIREBOMBER_TRAVEL_TIME_MAX gPrefs.m_pref[Defcon::CPrefs::Pref::firebomber_travel_time_max].Value

#define FIREBOMBER_VALUE           (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::firebomber_value].Value


#define GHOST_PLAYER_DIST_MIN      (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::ghost_player_dist_min].Value 
#define GHOST_VALUE                (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::ghost_value].Value 
#define GUPPY_SPEEDMAX             gPrefs.m_pref[Defcon::CPrefs::Pref::guppy_speedmax].Value
#define GUPPY_SPEEDMIN             gPrefs.m_pref[Defcon::CPrefs::Pref::guppy_speedmin].Value
#define GUPPY_VALUE                (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::guppy_value].Value
#define HUMAN_VALUE_DEBARKED       (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::human_value_debarked].Value
#define HUMAN_VALUE_EMBARKED       (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::human_value_embarked].Value
#define HUMAN_VALUE_LIBERATED      (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::human_value_liberated].Value
#define HUNTER_SPEEDMAX            gPrefs.m_pref[Defcon::CPrefs::Pref::hunter_speedmax].Value
#define HUNTER_SPEEDMIN            gPrefs.m_pref[Defcon::CPrefs::Pref::hunter_speedmin].Value
#define HUNTER_VALUE               (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::hunter_value].Value
#define LANDER_ABDUCTODDS          gPrefs.m_pref[Defcon::CPrefs::Pref::lander_abductodds].Value
#define LANDER_ASCENTRATE          gPrefs.m_pref[Defcon::CPrefs::Pref::lander_ascentrate].Value
#define LANDER_MATURE              gPrefs.m_pref[Defcon::CPrefs::Pref::lander_mature].Value
#define LANDER_VALUE               (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::lander_value].Value
#define LASER_EXTRA_COUNT          (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::laser_extra_count].Value
#define LASER_MULTI_PROB           gPrefs.m_pref[Defcon::CPrefs::Pref::laser_multi_prob].Value
#define MEMCOPY_METHOD             (MemcopyMethod)((int)gPrefs.m_pref[Defcon::CPrefs::Pref::memcopy_method].Value)
#define MENUTEXT_UPPERCASE         (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::menutext_uppercase].Value)
#define MINE_DAMAGE                (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::mine_damage].Value
#define MINE_LIFESPAN              gPrefs.m_pref[Defcon::CPrefs::Pref::mine_lifespan].Value
#define MISSIONNAME_UPPERCASE      (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::missionname_uppercase].Value)
#define MUNCHIE_VALUE              (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::munchie_value].Value
#define OBJECT_DRAWBBOX            (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::object_drawbbox].Value)
#define PHRED_VALUE                (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::phred_value].Value
#define POD_VALUE                  (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::pod_value].Value
#define REFORMER_VALUE             (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::reformer_value].Value 
#define REFORMERPART_VALUE         (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::reformerpart_value].Value 
#define SCORETIPS_SHOWENEMYPOINTS  (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::scoretips_showenemypoints].Value)
#define SOUND_ACTIVE               (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::sound_active].Value)
#define SPACEHUM_VALUE             (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::spacehum_value].Value
#define STARS_COUNT                (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::stars_count].Value
#define SWARMER_VALUE              (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::swarmer_value].Value
#define SWARMERS_PER_POD           (int32)gPrefs.m_pref[Defcon::CPrefs::Pref::swarmers_per_pod].Value
#define TIME_PRECISE               (bool)(1.0f == gPrefs.m_pref[Defcon::CPrefs::Pref::time_precise].Value)

