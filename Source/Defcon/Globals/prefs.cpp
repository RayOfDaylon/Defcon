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

	// Note: read "px" and "pixels" as being Slate units, not device pixels.

#define INITPREF(_key, _val, _units, _type, _min, _max, _desc)                     Pref[_key].Init(_val, _units, EVarType::_type, _min, _max, #_key, _desc)
//#define INITPREF2(_key, _val, _units, _type, _min, _max, _desc, _ex1, _ex2, _ex3)  Pref[_key].Init(_val, _units, EVarType::_type, _min, _max, #_key, _desc, _ex1, _ex2, _ex3)

    //        Key                          Value     Units           Type          Min          Max       Description
	//        ---------------------------  -----     -------------   ----------- ---------  ------------  --------------------------------------------------------------------------------------------

	INITPREF ( arena_bogdown,                1,       "",             Boolean,      0,           0,      "Slow game down when lots of objects and debris are visible"                                );
	INITPREF ( attack_initialdistance,       1.0f,    "%",            Float,        0.1f,        1.0f,   "Probability of enemy materializing far away from player"                                   );
	INITPREF ( baiter_prob,                  0.33f,   "%",            Float,        0,           1,      "Probability of baiter appearing near end of mission"                                       );
	INITPREF ( baiter_spawn_frequency,       2.0f,    "seconds",      Float,        0.5f,       10.0f,   "Number of seconds before another baiter is spawned"                                        );
	INITPREF ( baiter_value,               200,       "",             Integer,      0,        1000,      "Number of points earned for destroying a baiter"                                           );
	INITPREF ( bigred_value,               200,       "",             Integer,      0,        1000,      "Number of points earned for destroying a Big Red"                                          );
	INITPREF ( bomber_value,               250,       "",             Integer,      0,        1000,      "Number of points earned for destroying a bomber"                                           );
	INITPREF ( bouncer_value,              200,       "",             Integer,      0,        1000,      "Number of points earned for destroying a bouncer"                                          );
	INITPREF ( bullet_damage,               20,       "%",            Integer,      0,         100,      "Shield loss when object hit by a bullet"                                                   );
	INITPREF ( bullets_hit_terrain,          1,       "",             Boolean,      0,           0,      "Make enemy bullets unable to penetrate terrain"                                            );
	INITPREF ( collision_damage,            20,       "%",            Integer,      0,         100,      "Default shield loss when objects collide"                                                  );
	INITPREF ( controller_explicit_reverse,  0.0f,    "",             Boolean,      0.0f,        0.0f,   "Controller uses explicit reverse button"                                                   );
	INITPREF ( debris_drawradar,             0,       "",             Boolean,      0,           0,      "Show debris particles on radar display"                                                    );
	INITPREF ( debris_dual_prob,             0.75f,   "%",            Float,        0.0f,        1.0f,   "Probablity that an explosion ejects double the normal debris"                              );
	INITPREF ( debris_smoke,                 0,       "",             Boolean,      0,           0,      "Show smoke effects when an object explodes"                                                );
	INITPREF ( delay_before_attack,          3.0f,    "seconds",      Float,        0.1f,       10.0f,   "When mission starts or enemies run low, how much time before (more) enemies appear"        );
	INITPREF ( delay_between_reattack,      10.0f,    "seconds",      Float,        5.0,        60.0f,   "When enemies plentiful, how much time before reinforcements arrive"                        );

	INITPREF ( DestroyedPlayerFlashDuration,       1.5f,   "seconds",  Float,       0.0f,        1.0f,   "How long to flash the player ship before it explodes"                                      );
	INITPREF ( DestroyedPlayerExplosionDuration,   2.0f,   "seconds",  Float,       0.2f,        3.0f,   "How long the player ship explosion takes"                                                  );
	INITPREF ( DestroyedPlayerBlankDuration,       0.033f, "seconds",  Float,       0.0f,        0.2f,   "How long to blank the entire screen when the player ship explodes"                         );
	INITPREF ( DestroyedPlayerParticleSpeed,      30.0f,   "px/sec",   Float,      10.0f,      100.0f,   "Speed of particles in player ship explosion"                                               );
	INITPREF ( DestroyedPlayerParticleMinLifetime, 1.0f,   "seconds",  Float,       0.25f,       2.0f,   "Shortest lifetime possible for a player ship explosion particle"                           );     

	INITPREF ( dynamo_value,               150,       "",             Integer,      0,        1000,      "Number of points earned for destroying a dynamo"                                           );
	INITPREF ( enemies_canthurteachother,    1,       "",             Boolean,      0,           0,      "Enemies do not experience friendly fire"                                                   );
	INITPREF ( enemies_minesdonthurt,        1,       "",             Boolean,      0,           0,      "Enemies are not damaged by mines"                                                          );
	INITPREF ( enemy_birthdebrisdist,      500,       "px",           Integer,     50,        1200,      "Initial size of enemy materialization field"                                               );
	INITPREF ( enemy_birthduration,          0.7f,    "seconds",      Float,        0.1f,        3.0f,   "Time an enemy takes to materialize"                                                        );
	INITPREF ( fireball_value,             100,       "",             Integer,      0,        1000,      "Number of points earned for destroying a fireball"                                         );
	INITPREF ( fireballs_explode_on_ground,  1,       "",             Boolean,      0,           0,      "If fireballs should explode when hitting the ground"                                       );
	INITPREF ( firebomber_value,           250,       "",             Integer,      0,        1000,      "Number of points earned for destroying a firebomber"                                       );
	INITPREF ( ghost_value,                250,       "",             Integer,      0,        1000,      "Number of points earned for destroying a ghost"                                            );
	INITPREF ( ghost_player_dist_min,      400,       "px",           Integer,      0,        1000,      "Distance from player at which ghost will split"                                            ); 
	INITPREF ( GuppyCollisionForce,        17,       "%",            Integer,      0,         100,      "Shield loss when colliding with Yllabian space guppy"                                       );
	INITPREF ( guppy_value,                200,       "",             Integer,      0,        1000,      "Number of points earned for destroying a space guppy"                                      );
	INITPREF ( humans_count,                15,       "",             Integer,      1,         100,      "Number of humanoids at start of game"                                                      );
	INITPREF ( human_distribution,           0.5f,    "",             Float,        0.5f,        1.0f,   "How far to distribute humans over planet surface"                                          );
	INITPREF ( human_withinrange,           50,       "px",           Integer,     10,         500,      "Distance a humanoid must be to lander to be of interest"                                   );
	INITPREF ( human_terminalvelocity,      90,       "px/sec",       Integer,      1,         500,      "Speed at which a falling humanoid dies if it has not yet reached the ground"               );
	INITPREF ( human_bonus_value,          100,       "",             Integer,      1,         500,      "Bonus points for each humanoid alive at end of mission"                                    );
	INITPREF ( human_value_debarked,       250,       "",             Integer,      0,        1000,      "Points earned when a humanoid is flown back to the ground"                                 );
	INITPREF ( human_value_embarked,       500,       "",             Integer,      0,        1000,      "Points earned when a humanoid is picked up in the air"                                     );
	INITPREF ( human_value_liberated,      250,       "",             Integer,      0,        1000,      "Points earned when a humanoid falls safely to the ground"                                  );
	INITPREF ( HunterCollisionForce,       30,       "%",            Integer,      0,         100,      "Shield loss when colliding with hunter"                                                     );
	INITPREF ( hunter_value,               250,       "",             Integer,      0,        1000,      "Number of points earned for destroying a hunter"                                           );
	INITPREF ( lander_abductodds,            0.0001f, "%",            Float,        0.00001f,    0.001f, "Probability per frame that a humanoid within range will be abducted"                       );
	INITPREF ( lander_mature,               20.0f,    "seconds",      Float,        1.0f,       60.0f,   "How old a lander has to be before it will definitely abduct a humanoid"                    );
	INITPREF ( lander_value,               150,       "",             Integer,      0,        1000,      "Number of points earned for destroying a lander"                                           );
	INITPREF ( laser_age_max,                0.5f,    "seconds",      Float,        0.1f,        1.0f,   "Longest length of time a laser beam can exist"                                             );
	INITPREF ( laser_extra_count,            1,       "",             Integer,      1,           4,      "Number of extra rounds per laser shot"                                                     );
	INITPREF ( laser_multi_prob,             0.25f,   "%",            Float,        0.0f,        1.0f,   "Probablity that extra laser rounds occur per shot"                                         );
	INITPREF ( menutext_uppercase,           0,       "",             Boolean,      0,           0,      "Make menu text appear uppercase"                                                           );
	INITPREF ( MessageDuration,              1.5f,    "seconds",      Float,        0.5f,        6.0f,   "How long a normal message stays visible in the message log"                                );
	INITPREF ( MessageDurationImportant,     4.0f,    "seconds",      Float,        0.5f,        6.0f,   "How long an important message stays visible in the message log"                            );
	INITPREF ( mine_damage,                 30,       "%",            Integer,      0,         100,      "Shield loss when object hits a mine"                                                       );
	INITPREF ( mine_lifespan,               10.0f,    "seconds",      Float,        1.0f,       30.0f,   "How long a mine exists after being deployed"                                               );
	INITPREF ( missionname_uppercase,        0,       "",             Boolean,      0,           0,      "Make mission name text appear uppercase"                                                   );
	INITPREF ( MunchieCollisionForce,      15,       "%",            Integer,      0,         100,      "Shield loss when colliding with munchie"                                                    );
	INITPREF ( munchie_value,               50,       "",             Integer,      0,        1000,      "Number of points earned for destroying a munchie"                                          );
	INITPREF ( object_drawbbox,              0,       "",             Boolean,      0,           0,      "Show sprite bounding boxes (debugging aid)"                                                );
	INITPREF ( phred_value,                200,       "",             Integer,      0,        1000,      "Number of points earned for destroying a Phred"                                            );
	INITPREF ( planet_to_screen_ratio,       5.0f,    "%",            Float,     2.0f,        6.0f,      "How wide the planet surface is relative to visible portion"                                );
	INITPREF ( player_birthdebrisdist,    1250.0f,    "px",           Float,        0.1f,        5.0f,   "Initial size of player ship's materialization field"                                       );
	INITPREF ( player_birthduration,         3.0f,    "seconds",      Float,        0.1f,        5.0f,   "How long for player ship to materialize"                                                   );
	INITPREF ( player_drag,                  0.06f,   "",             Float,        0.001f,      1.0f,   "Drag acting upon player ship"                                                              );
	INITPREF ( player_marginsettlespeed,  1400.0f,    "px/sec",       Integer,     50,        3000,      "How quickly the player ship settles to the edge of the screen after reversing direction"   );
	INITPREF ( player_mass,                  0.08f,   "",             Float,        0.001f,      1.0f,   "Mass of player ship"                                                                       );
	INITPREF ( player_maxthrust,             4.5f,    "",             Float,        0.1f,        5.0f,   "Maximum thrust force applicable to player ship"                                            );
	INITPREF ( player_posmargin,           450.0f,    "px",           Integer,     50,         700,      "Minimum distance between player's ship and edge of screen"                                 );
	INITPREF ( player_rebirth_delay,         2.0f,    "seconds",      Float,        0.1f,        5.0f,   "Delay between player destruction and materialization"                                      );
	INITPREF ( PlayerDoubleGunsLoss,         0.02f,   "%/sec",        Float,        0.01f,       0.2f,   "Rate at which player ship double gun availability diminishes"                              );
	INITPREF ( PlayerInvincibilityLoss,      0.02f,   "%/sec",        Float,        0.01f,       0.2f,   "Rate at which player ship invincibility diminishes"                                        );
	INITPREF ( PlayerShipImpactScale,        0.5f,    "%",            Float,        0.1f,        2.0f,   "How damage to player ship should be scaled up or down"                                     );
	INITPREF ( pod_value,                 1000,       "",             Integer,      0,        1000,      "Number of points earned for destroying a pod"                                              );
	INITPREF ( RadarIsPlayerCentric,         0,       "",             Boolean,      0,           0,      "Radar keeps player ship fixed, otherwise screen edges are fixed (arcade match)"            );
	INITPREF ( reformer_value,             250,       "",             Integer,      0,        1000,      "Number of points earned for destroying a reformer"                                         );
	INITPREF ( ReformerPartCollisionForce,  15,       "%",            Integer,      0,         100,      "Shield loss when colliding with reformer part"                                             );
	INITPREF ( reformerpart_value,          50,       "",             Integer,      0,        1000,      "Number of points earned for destroying a reformer part"                                    );
	INITPREF ( scoretips_showenemypoints,    0,       "",             Boolean,      0,           0,      "Show floating score text when enemy destroyed"                                             );
	INITPREF ( smartbomb_initial,            3.0f,    "",             Integer,      0,          20,      "Initial number of smart bombs when starting a game"                                        );
	INITPREF ( smartbomb_value,          20000.0f,    "",             Integer,   1000,      100000,      "Number of points needed to earn extra smart bombs"                                         );
	INITPREF ( smartbomb_resupply,           1,       "",             Integer,      1,          10,      "Number of smart bombs given when earned"                                                   );
	INITPREF ( smartbomb_lifespan,           0.75f,   "seconds",      Float,        0.1f,        2.0f,   "How long a smartbomb shockwave persists"                                                   );
	INITPREF ( SmartbombRadiusFactor,        1.0f,    "%",            Float,        0.5f,      1.0f,     "Percentage of screen width to use for smartbomb shockwave radius"                          );
	INITPREF ( smartbomb_wavepush,          30.0f,    "px/frame",     Float,        5.0f,      100.0f,   "Debris near player is pushed at this speed by smartbomb shockwave"                         );
	INITPREF ( smartbomb_wavepushmin,       15.0f,    "px/frame",     Float,        5.0f,       50.0f,   "Debris is additionally pushed by smartbomb shockwave this fast regardless of location"     );
	INITPREF ( smartbomb_max_flashscreen,   15,       "frames",       Integer,      1,          50,      "How long a smartbomb shockwave is visible"                                                 );
	INITPREF ( SmartbombMaxNum,             15,       "",             Integer,      1,          15,      "Maximum number of smart bombs player ship can carry"                                       );
	INITPREF ( sound_active,                 1,       "",             Boolean,      0,           0,      "Enable sound effects (disable, e.g., if you want to run a music program while playing)"    );
	INITPREF ( SpacehumCollisionForce,       5,       "%",            Integer,      0,         100,      "Shield loss when colliding with spacehum"                                                  );
	INITPREF ( spacehum_value,             100,       "",             Integer,      0,        1000,      "Number of points earned for destroying a space hum"                                        );
	INITPREF ( stars_count,                100,       "",             Integer,      0,        1000,      "Number of background stars per 100 x 100 pixel area"                                       );
	INITPREF ( SwarmerCollisionForce,       10,       "%",            Integer,      0,         100,      "Shield loss when colliding with swarmer"                                                   );
	INITPREF ( swarmer_value,              150,       "",             Integer,      0,        1000,      "Number of points earned for destroying a swarmer"                                          );
	INITPREF ( swarmers_per_pod,             6,       "",             Integer,      0,          20,      "Number of swarmers a destroyed pod releases"                                               );

#undef INITPREF
//#undef INITPREF2


	// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Ranged preferences.

#define INITPREFR(_key, _lo, _hi, _units, _desc)  Pref[_key].Init(Daylon::FRange<float>(_lo, _hi), _units, 0.0f, 0.0f, #_key, _desc)

    //        Key                         Min        Max         Units     Description
	//        ------------------------  -------    -------     ---------   ---------------------------------------------------------------

	INITPREFR ( FirebomberSpeed,          250,        500,     "px/sec",   "Firebomber travel speed"                                      );
	INITPREFR ( FirebomberTravelTime,       0.1f,       0.4f,  "seconds",  "Firebomber travel time along current path"                    );
	INITPREFR ( GuppySpeed,               150,        300,     "px/sec",   "Guppy flight speed"                                           );
	INITPREFR ( HunterSpeed,              200,        400,     "px/sec",   "Hunter flight speed"                                          );
	INITPREFR ( LanderAscentSpeed,         60,        180,     "px/sec",   "Lander ascent rate"                                           );
	INITPREFR ( LanderDescentSpeed,        45,        135,     "px/sec",   "Lander descent rate"                                          );
	INITPREFR ( LaserSpeed,              2000,       5000,     "px/sec",   "Speed of trailing and leading ends of laser beams"            );
	INITPREFR ( MunchieSpawnCountdown,      2.0f,       5.0f,  "sec",      "Time for a phred to spawn a munchie"                          );
	INITPREFR ( PodIntersectionWidth,       0.2f,       0.8f,  "%",        "Pod intersection width (percentage of visible screen width)"  );
	INITPREFR ( PodSpeed,                  50,        400,     "px/sec",   "Pod travel speed");
	INITPREFR ( SpacehumSpeed,             50,        700,     "px/sec",   "Spacehum travel speed");

#undef INITPREFR
}


void Defcon::CPrefs::Init(const FString& Filename)
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
			FPrefVar& var = Translate(token);
			token = MyStrtok(nullptr, " \t=\r\n");
			if(!isnum(token))
				throw 0;
			var.m_fValue = (float)atof(token);
		}
		catch(...) {}
	}
#endif
}


FPrefVar& Defcon::CPrefs::Translate(const FString& Key) const
{
	for(int32 Index = 0; Index < EPref::count; Index++)
	{
		if(Pref[Index].Is(Key))
		{
			return (FPrefVar&)(Pref[Index]);
		}
	}
	check(false);
	throw 0;
}
