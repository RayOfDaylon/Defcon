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

			enum EPref 
			{
				arena_bogdown,
				attack_initialdistance,

				baiter_prob,
				baiter_spawn_frequency,
				baiter_value,

				bigred_value,

				bomber_value,
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

				dynamo_value,

				enemy_birthdebrisdist,
				enemy_birthduration,
				enemies_canthurteachother,
				enemies_minesdonthurt,

				fireballs_explode_on_ground,
				fireball_value,
				firebomber_value,
				FirebomberSpeed,
				FirebomberTravelTime,

				ghost_player_dist_min,
				ghost_value,
				guppy_value,
				GuppyCollisionForce,
				GuppySpeed,

				human_bonus_value,
				human_distribution,
				human_terminalvelocity,
				human_withinrange,
				humans_count,
				human_value_debarked,
				human_value_embarked,
				human_value_liberated,

				HunterCollisionForce,
				HunterSpeed,
				hunter_value,

				lander_abductodds,
				LanderAscentSpeed,
				LanderDescentSpeed,
				lander_mature,
				lander_value,

				laser_age_max,
				laser_extra_count,
				laser_multi_prob,
				LaserSpeed,

				menutext_uppercase,

				mine_damage,
				mine_lifespan,

				missionname_uppercase,
				MunchieCollisionForce,
				MunchieSpawnCountdown,
				munchie_value,

				object_drawbbox,

				phred_value,

				planet_to_screen_ratio,

				player_birthdebrisdist,
				player_birthduration,
				player_drag,
				player_marginsettlespeed,
				player_mass,
				player_maxthrust,
				player_posmargin,
				player_rebirth_delay,
				PodIntersectionWidth,
				PodSpeed,
				pod_value,

				RadarIsPlayerCentric,
				reformer_value,
				ReformerPartCollisionForce,
				reformerpart_value,

				scoretips_showenemypoints,

				smartbomb_initial,
				smartbomb_lifespan,
				smartbomb_max_flashscreen,
				SmartbombMaxNum,
				SmartbombRadiusFactor,
				smartbomb_resupply,
				smartbomb_value,	
				smartbomb_wavepush,
				smartbomb_wavepushmin,

				sound_active,
				SpacehumCollisionForce,
				stars_count,
				spacehum_value,
				swarmer_value,
				SwarmerCollisionForce,
				swarmers_per_pod,

				count
			};

			void      Init      (const FString& Str);
			FPrefVar& Translate (const FString& Str) const;

			FPrefVar  Pref[EPref::count];

		protected:
			void      Construct();
	};
}



extern Defcon::CPrefs	GPrefs;


#define PREFVAL(_key)                  GPrefs.Pref[Defcon::CPrefs::EPref::_key].Value
#define FPREFVAL(_key)                 PREFVAL(_key)
#define IPREFVAL(_key)                 (int32)PREFVAL(_key)
#define BPREFVAL(_key)                 (bool)(1.0f == PREFVAL(_key))
#define RPREFVAL(_key)                 GPrefs.Pref[Defcon::CPrefs::EPref::_key].Range

#define CONTROLLER_EXPLICIT_REVERSE    BPREFVAL(controller_explicit_reverse)

#define PLANET_TO_SCREEN_RATIO      FPREFVAL(planet_to_screen_ratio)

#define PLAYER_REBIRTH_DELAY        FPREFVAL(player_rebirth_delay)
#define PLAYER_BIRTHDURATION        FPREFVAL(player_birthduration)
#define PLAYER_BIRTHDEBRISDIST      FPREFVAL(player_birthdebrisdist)
#define PLAYER_REWARD_POINTS        IPREFVAL(player_reward_points)
#define PLAYER_POSMARGIN            FPREFVAL(player_posmargin)
#define PLAYER_MARGINSETTLESPEED    FPREFVAL(player_marginsettlespeed)
#define PLAYER_MAXTHRUST            FPREFVAL(player_maxthrust)
#define PLAYER_MASS                 FPREFVAL(player_mass)
#define PLAYER_DRAG                 FPREFVAL(player_drag)

#define RADAR_IS_PLAYER_CENTRIC     BPREFVAL(RadarIsPlayerCentric)

#define SMARTBOMB_INITIAL           IPREFVAL(smartbomb_initial)
#define SMARTBOMB_VALUE             IPREFVAL(smartbomb_value)
#define SMARTBOMB_RESUPPLY          IPREFVAL(smartbomb_resupply)
#define SMARTBOMB_LIFESPAN          FPREFVAL(smartbomb_lifespan)
#define SMARTBOMB_RADIUS_FACTOR     FPREFVAL(SmartbombRadiusFactor)
#define SMARTBOMB_WAVEPUSH          FPREFVAL(smartbomb_wavepush)//200.0f
#define SMARTBOMB_WAVEPUSHMIN       FPREFVAL(smartbomb_wavepushmin)//100.0f
#define SMARTBOMB_MAX_FLASHSCREEN   IPREFVAL(smartbomb_max_flashscreen)
#define SMARTBOMB_MAXNUM            IPREFVAL(SmartbombMaxNum)

#define HUMANS_COUNT                IPREFVAL(humans_count)
#define HUMAN_WITHINRANGE           FPREFVAL(human_withinrange)
#define HUMAN_DISTRIBUTION          FPREFVAL(human_distribution)
#define HUMAN_TERMINALVELOCITY      FPREFVAL(human_terminalvelocity)
#define HUMAN_BONUS_VALUE           IPREFVAL(human_bonus_value)


// When mission starts or enemies run low, 
// how much time before (more) enemies appear.
#define DELAY_BEFORE_ATTACK         FPREFVAL(delay_before_attack)

// When enemies plentiful, how much time 
// before more enemies appear.
#define DELAY_BETWEEN_REATTACK      FPREFVAL(delay_between_reattack)


#define ATTACK_INITIALDISTANCE      FPREFVAL(attack_initialdistance)

#define ARENA_BOGDOWN                BPREFVAL(arena_bogdown)
#define BAITER_PROB                  FPREFVAL(baiter_prob)
#define BAITER_SPAWN_FREQUENCY       FPREFVAL(baiter_spawn_frequency)
#define BAITER_VALUE                 IPREFVAL(baiter_value)
#define BIGRED_VALUE                 IPREFVAL(bigred_value)
#define BOMBER_VALUE                 IPREFVAL(bomber_value)
#define BOUNCER_VALUE                IPREFVAL(bouncer_value)
#define BULLET_DAMAGE                IPREFVAL(bullet_damage)
#define BULLETS_HIT_TERRAIN          BPREFVAL(bullets_hit_terrain)
#define COLLISION_DAMAGE             IPREFVAL(collision_damage)
#define DEBRIS_DRAWRADAR             BPREFVAL(debris_drawradar)
#define DEBRIS_DUAL_PROB             FPREFVAL(debris_dual_prob)
#define DEBRIS_SMOKE                 BPREFVAL(debris_smoke)
#define DYNAMO_VALUE                 IPREFVAL(dynamo_value)
#define ENEMIES_CANTHURTEACHOTHER    BPREFVAL(enemies_canthurteachother)
#define ENEMIES_MINESDONTHURT        BPREFVAL(enemies_minesdonthurt)
#define ENEMY_BIRTHDEBRISDIST        FPREFVAL(enemy_birthdebrisdist)
#define ENEMY_BIRTHDURATION          FPREFVAL(enemy_birthduration)
#define FIREBALLS_EXPLODE_ON_GROUND  BPREFVAL(fireballs_explode_on_ground)
#define FIREBALL_VALUE               IPREFVAL(fireball_value)
#define FIREBOMBER_SPEED             RPREFVAL(FirebomberSpeed)
#define FIREBOMBER_TRAVEL_TIME       RPREFVAL(FirebomberTravelTime)
#define FIREBOMBER_VALUE             IPREFVAL(firebomber_value)
#define GHOST_PLAYER_DIST_MIN        IPREFVAL(ghost_player_dist_min) 
#define GHOST_VALUE                  IPREFVAL(ghost_value) 
#define GUPPY_COLLISION_FORCE        FPREFVAL(GuppyCollisionForce)
#define GUPPY_SPEED                  RPREFVAL(GuppySpeed)
#define GUPPY_VALUE                  IPREFVAL(guppy_value)
#define HUMAN_VALUE_DEBARKED         IPREFVAL(human_value_debarked)
#define HUMAN_VALUE_EMBARKED         IPREFVAL(human_value_embarked)
#define HUMAN_VALUE_LIBERATED        IPREFVAL(human_value_liberated)
#define HUNTER_COLLISION_FORCE       FPREFVAL(HunterCollisionForce)
#define HUNTER_SPEED                 RPREFVAL(HunterSpeed)
#define HUNTER_VALUE                 IPREFVAL(hunter_value)
#define LANDER_ABDUCTODDS            FPREFVAL(lander_abductodds)
#define LANDER_ASCENT_SPEED          RPREFVAL(LanderAscentSpeed)
#define LANDER_DESCENT_SPEED         RPREFVAL(LanderDescentSpeed)
#define LANDER_MATURE                FPREFVAL(lander_mature)
#define LANDER_VALUE                 IPREFVAL(lander_value)
#define LASER_AGE_MAX                FPREFVAL(laser_age_max)
#define LASER_EXTRA_COUNT            IPREFVAL(laser_extra_count)
#define LASER_MULTI_PROB             FPREFVAL(laser_multi_prob)
#define LASER_SPEED                  RPREFVAL(LaserSpeed)
#define MENUTEXT_UPPERCASE           BPREFVAL(menutext_uppercase)
#define MINE_DAMAGE                  IPREFVAL(mine_damage)
#define MINE_LIFESPAN                FPREFVAL(mine_lifespan)
#define MISSIONNAME_UPPERCASE        BPREFVAL(missionname_uppercase)
#define MUNCHIE_COLLISION_FORCE      FPREFVAL(MunchieCollisionForce)
#define MUNCHIE_SPAWN_COUNTDOWN      RPREFVAL(MunchieSpawnCountdown)
#define MUNCHIE_VALUE                IPREFVAL(munchie_value)
#define OBJECT_DRAWBBOX              BPREFVAL(object_drawbbox)
#define PHRED_VALUE                  IPREFVAL(phred_value)
#define POD_INTERSECTION_WIDTH       RPREFVAL(PodIntersectionWidth)
#define POD_SPEED                    RPREFVAL(PodSpeed)
#define POD_VALUE                    IPREFVAL(pod_value)
#define REFORMER_VALUE               IPREFVAL(reformer_value) 
#define REFORMERPART_COLLISION_FORCE FPREFVAL(ReformerPartCollisionForce)
#define REFORMERPART_VALUE           IPREFVAL(reformerpart_value) 
#define SCORETIPS_SHOWENEMYPOINTS    BPREFVAL(scoretips_showenemypoints)
#define SOUND_ACTIVE                 BPREFVAL(sound_active)
#define SPACEHUM_VALUE               IPREFVAL(spacehum_value)
#define SPACEHUM_COLLISION_FORCE     FPREFVAL(SpacehumCollisionForce)
#define STARS_COUNT                  IPREFVAL(stars_count)
#define SWARMER_VALUE                IPREFVAL(swarmer_value)
#define SWARMER_COLLISION_FORCE      FPREFVAL(SwarmerCollisionForce)
#define SWARMERS_PER_POD             IPREFVAL(swarmers_per_pod)

