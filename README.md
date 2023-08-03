# Defense Condition
Extended remake of Williams Electronics Stargate 1981 arcade game using Unreal Engine

"Defcon" is the internal project name.  
Stargate was also known as Defender II in some locales.

Example Videos
Version 1.0: https://youtu.be/WTqya41q9xM

The game differs from Stargate in these ways:
- Each level is its own campaign mission and not a repeat wave  
- After destroying all mission-critical targets, the stargate must be entered to jump to the next mission  
- Entering the stargate while carrying humans does not warp ahead any levels  
- Game can be made to start at any level  
- Sprite textures remastered for 4K displays  
- Player ship is shielded; shields slowly auto-regen after taking damage  
- Landers leave orbit after successfully abducting humans instead of becoming mutants  
- No score tracking/display; acquired points are used internally as XP  
- Extra enemy types: hunters, ghosts, bouncers, reformers, and weak firebombers  
- Enemy population tends to be greater  
- Smartbombs show explosion fireball, destroys enemies as shockwave reaches them  
- Planet is wider  
- Terrain is not as irregular  
- Display aspect ratio is wider  
- Messages/alerts show using Descent-style scrolling message log

The game was ported from my older Windows GDI version written in 2004.
It differs from that game in several ways as well -- there's no
settings screen, and settings are currently not read from disk.
The intro story and music are also left out. The mission "Swarm"
was removed (although its source code is included). Options like
double lasers and blur/interlace filters also are gone, but may
be included back later.

The DaylonGraphicsLibrary plugin provides generic types and functions
that can be used in other similar games, along with some widgets.
It's included directly instead of using a git submodule, so 
that if you clone or pull this repo, you'll have it without any fuss
ready to go. It should be the same as the DaylonGraphicsLibrary plugin
in the Stellar Mayhem (SpaceRox) repo.

Unreal Engine 5.1 or higher required.

Keyboard input supported at present.

A 1920 x 1080 (HD) viewport is used. Monitors of different sizes and/or display scales will scale. Textures are deliberately oversized to avoid blurring when upscaled.

Game sounds may be loud. Until a volume setting is added, it's recommended to manually lower the volume in your OS speaker/headphone settings.

All graphics are done in UMG/Slate, to push those subsystems even harder than in Stellar Mayhem.

No binaries available yet, sorry.
