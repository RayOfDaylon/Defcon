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


#define PREFVAL(_key)                  gPrefs.m_pref[Defcon::CPrefs::Pref::_key].Value
#define FPREFVAL(_key)                 PREFVAL(_key)
#define IPREFVAL(_key)                 (int32)PREFVAL(_key)
#define BPREFVAL(_key)                 (bool)(1.0f == PREFVAL(_key))

#define CONTROLLER_EXPLICIT_REVERSE    BPREFVAL(controller_explicit_reverse)

#define PLANET_TO_SCREEN_RATIO      PREFVAL(planet_to_screen_ratio)

#define PLAYER_REBIRTH_DELAY        PREFVAL(player_rebirth_delay)
#define PLAYER_BIRTHDURATION        PREFVAL(player_birthduration)
#define PLAYER_BIRTHDEBRISDIST      PREFVAL(player_birthdebrisdist)
#define PLAYER_REWARD_POINTS        IPREFVAL(player_reward_points)
#define PLAYER_POSMARGIN            PREFVAL(player_posmargin)
#define PLAYER_MARGINSETTLESPEED    PREFVAL(player_marginsettlespeed)
#define PLAYER_MAXTHRUST            PREFVAL(player_maxthrust)
#define PLAYER_MASS                 PREFVAL(player_mass)
#define PLAYER_DRAG                 PREFVAL(player_drag)

#define SMARTBOMB_INITIAL           IPREFVAL(smartbomb_initial)
#define SMARTBOMB_VALUE             IPREFVAL(smartbomb_value)
#define SMARTBOMB_RESUPPLY          IPREFVAL(smartbomb_resupply)
#define SMARTBOMB_LIFESPAN          PREFVAL(smartbomb_lifespan)
#define SMARTBOMB_WAVEPUSH          PREFVAL(smartbomb_wavepush)//200.0f
#define SMARTBOMB_WAVEPUSHMIN       PREFVAL(smartbomb_wavepushmin)//100.0f
#define SMARTBOMB_MAX_FLASHSCREEN   IPREFVAL(smartbomb_max_flashscreen)

#define HUMANS_COUNT                IPREFVAL(humans_count)
#define HUMAN_WITHINRANGE           PREFVAL(human_withinrange)
#define HUMAN_DISTRIBUTION          PREFVAL(human_distribution)
#define HUMAN_TERMINALVELOCITY      PREFVAL(human_terminalvelocity)
#define HUMAN_BONUS_VALUE           IPREFVAL(human_bonus_value)


// When mission starts or enemies run low, 
// how much time before (more) enemies appear.
#define DELAY_BEFORE_ATTACK         PREFVAL(delay_before_attack)

// When enemies plentiful, how much time 
// before more enemies appear.
#define DELAY_BETWEEN_REATTACK      PREFVAL(delay_between_reattack)

#define LANDER_DESCENT_SPEED        PREFVAL(lander_descent_speed)

/*
#define LANDERS_PER_ATTACK          IPREFVAL(landers_per_attack)
#define BOMBERS_PER_ATTACK          IPREFVAL(bombers_per_attack)
#define HUNTERS_PER_ATTACK          IPREFVAL(hunters_per_attack)
#define BAITERS_PER_ATTACK          IPREFVAL(baiters_per_attack)
#define FIREBOMBERS_PER_ATTACK      IPREFVAL(firebombers_per_attack)
#define DYNAMOS_PER_ATTACK          IPREFVAL(dynamos_per_attack)
#define PODS_PER_ATTACK             IPREFVAL(pods_per_attack)
*/
#define ATTACK_INITIALDISTANCE      PREFVAL(attack_initialdistance)

/*
#define LANDERS_IN_FIRST_WAVE       IPREFVAL(landers_in_first_wave)
#define HUNTERS_IN_FIRST_WAVE       IPREFVAL(hunters_in_first_wave)
#define BOMBERS_IN_FIRST_WAVE       IPREFVAL(bombers_in_first_wave)
#define BAITERS_IN_FIRST_WAVE       IPREFVAL(baiters_in_first_wave)
#define FIREBOMBERS_IN_FIRST_WAVE   IPREFVAL(firebombers_in_first_wave)
#define DYNAMOS_IN_FIRST_WAVE       IPREFVAL(dynamos_in_first_wave)
#define PODS_IN_FIRST_WAVE          IPREFVAL(pods_in_first_wave)
*/


#define ARENA_BOGDOWN              BPREFVAL(arena_bogdown)
#define BAITER_PROB                PREFVAL(baiter_prob)
#define BAITER_SPAWN_FREQUENCY     PREFVAL(baiter_spawn_frequency)
#define BAITER_VALUE               IPREFVAL(baiter_value)
#define BIGRED_VALUE               IPREFVAL(bigred_value)
#define BOMBER_VALUE               IPREFVAL(bomber_value)
#define BOUNCER_VALUE              IPREFVAL(bouncer_value)
#define BULLET_DAMAGE              IPREFVAL(bullet_damage)
#define BULLETS_HIT_TERRAIN        BPREFVAL(bullets_hit_terrain)
#define COLLISION_DAMAGE           IPREFVAL(collision_damage)
#define DEBRIS_DRAWRADAR           BPREFVAL(debris_drawradar)
#define DEBRIS_DUAL_PROB           PREFVAL(debris_dual_prob)
#define DEBRIS_SMOKE               BPREFVAL(debris_smoke)
#define DISPLAY_BLUR               IPREFVAL(display_blur)
#define DISPLAY_BLUR_METHOD        IPREFVAL(display_blur_method)
#define DISPLAY_FULLSCREEN         BPREFVAL(display_fullscreen)
#define DISPLAY_HEIGHT             IPREFVAL(display_height)
#define DISPLAY_HIDEWINDOWFRAME    BPREFVAL(display_hidewindowframe)
#define DISPLAY_INTERLACE          BPREFVAL(display_interlace)
#define DISPLAY_WIDTH              IPREFVAL(display_width)
#define DYNAMO_VALUE               IPREFVAL(dynamo_value)
#define ENEMIES_CANTHURTEACHOTHER  BPREFVAL(enemies_canthurteachother)
#define ENEMIES_MINESDONTHURT      BPREFVAL(enemies_minesdonthurt)
#define ENEMY_BIRTHDEBRISDIST      PREFVAL(enemy_birthdebrisdist)
#define ENEMY_BIRTHDURATION        PREFVAL(enemy_birthduration)
#define FIREBALL_VALUE             IPREFVAL(fireball_value)

#define FIREBOMBER_SPEED_MIN       PREFVAL(firebomber_speed_min)
#define FIREBOMBER_SPEED_MAX       PREFVAL(firebomber_speed_max)
#define FIREBOMBER_TRAVEL_TIME_MIN PREFVAL(firebomber_travel_time_min)
#define FIREBOMBER_TRAVEL_TIME_MAX PREFVAL(firebomber_travel_time_max)

#define FIREBOMBER_VALUE           IPREFVAL(firebomber_value)


#define GHOST_PLAYER_DIST_MIN      IPREFVAL(ghost_player_dist_min) 
#define GHOST_VALUE                IPREFVAL(ghost_value) 
#define GUPPY_SPEEDMAX             PREFVAL(guppy_speedmax)
#define GUPPY_SPEEDMIN             PREFVAL(guppy_speedmin)
#define GUPPY_VALUE                IPREFVAL(guppy_value)
#define HUMAN_VALUE_DEBARKED       IPREFVAL(human_value_debarked)
#define HUMAN_VALUE_EMBARKED       IPREFVAL(human_value_embarked)
#define HUMAN_VALUE_LIBERATED      IPREFVAL(human_value_liberated)
#define HUNTER_SPEEDMAX            PREFVAL(hunter_speedmax)
#define HUNTER_SPEEDMIN            PREFVAL(hunter_speedmin)
#define HUNTER_VALUE               IPREFVAL(hunter_value)
#define LANDER_ABDUCTODDS          PREFVAL(lander_abductodds)
#define LANDER_ASCENTRATE          PREFVAL(lander_ascentrate)
#define LANDER_MATURE              PREFVAL(lander_mature)
#define LANDER_VALUE               IPREFVAL(lander_value)
#define LASER_EXTRA_COUNT          IPREFVAL(laser_extra_count)
#define LASER_MULTI_PROB           PREFVAL(laser_multi_prob)
#define MEMCOPY_METHOD             (MemcopyMethod)((int)PREFVAL(memcopy_method))
#define MENUTEXT_UPPERCASE         BPREFVAL(menutext_uppercase)
#define MINE_DAMAGE                IPREFVAL(mine_damage)
#define MINE_LIFESPAN              PREFVAL(mine_lifespan)
#define MISSIONNAME_UPPERCASE      BPREFVAL(missionname_uppercase)
#define MUNCHIE_VALUE              IPREFVAL(munchie_value)
#define OBJECT_DRAWBBOX            BPREFVAL(object_drawbbox)
#define PHRED_VALUE                IPREFVAL(phred_value)
#define POD_VALUE                  IPREFVAL(pod_value)
#define REFORMER_VALUE             IPREFVAL(reformer_value) 
#define REFORMERPART_VALUE         IPREFVAL(reformerpart_value) 
#define SCORETIPS_SHOWENEMYPOINTS  BPREFVAL(scoretips_showenemypoints)
#define SOUND_ACTIVE               BPREFVAL(sound_active)
#define SPACEHUM_VALUE             IPREFVAL(spacehum_value)
#define STARS_COUNT                IPREFVAL(stars_count)
#define SWARMER_VALUE              IPREFVAL(swarmer_value)
#define SWARMERS_PER_POD           IPREFVAL(swarmers_per_pod)
#define TIME_PRECISE               BPREFVAL(time_precise)

