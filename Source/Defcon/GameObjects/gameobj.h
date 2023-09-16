// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DaylonUtils.h"
#include "GameObjects/obj_types.h"
#include "Globals/prefs.h"
#include "Common/util_geom.h"
#include "Common/util_color.h"
#include "Globals/GameColors.h"
#include "Globals/GameObjectResources.h"


struct FPainter;


namespace Defcon
{
	class I2DCoordMapper;

	enum class EObjectCreationFlags : uint8
	{
		NoMaterialization = 0x0,
		NotMissionTarget  = 0x0,
		Materializes      = 0x1,
		IsMissionTarget   = 0x2,

		EnemyPart     = NoMaterialization | NotMissionTarget,
		StandardEnemy = Materializes | IsMissionTarget,
		CleanerEnemy  = Materializes | NotMissionTarget
	};


	// todo: could be a template function supporting all mask-oriented enum classes
	inline bool HasFlag(EObjectCreationFlags Flags, EObjectCreationFlags Which)
	{
		return(0 != ((int32)Flags & (int32)Which));
	}


	enum class EMessage : uint8
	{
		TakenAboard,
		CarrierKilled,
		Released,
		HumanKilled,
		HumanTakenAboard
	};


	class CGameObjectCollection;


	struct FExplosionParams
	{
		EColor YoungColor[2];
		EColor OldColor[2]     = { EColor::Red, EColor::Red };
		float  MinSpeed        = 110.0f;
		float  MaxSpeed        = 140.0f;
		float  MaxParticleSize =  10.0f;
		float  InertiaScale    =   1.0f;
		int32  NumParticles    =  50;
		bool   bFade           = true; // Debris fades over time
		bool   bCold           = false;
	};


	enum class EExplosionFireball : uint8
	{
		Plain,
		BrightBall
	};


	class IGameObject
	{
		// Base class for all game entities.
		// A shot or piece of debris can be 
		// based on this class (although their drag 
		// coefficients tend to be very low or even zero). 

		public:

			IGameObject();
			virtual ~IGameObject();

			virtual void          Init                  (const CFPoint& ArenaSize, const CFPoint& ScreenSize);
			virtual void          OnFinishedCreating    ();
			virtual bool          OccursFrequently      () const;

			// Linked list stuff.
			IGameObject*          GetNext               ();
			IGameObject*          GetPrev               ();
			void                  SetNext               (IGameObject* p);
			void                  SetPrev               (IGameObject* p);

			// Identify stuff.
			EObjType              GetParentType         () const;
			EObjType              GetCreatorType        () const;
			void                  SetCreatorType        (EObjType n);
			EObjType              GetType               () const;
			void                  SetType               (EObjType n);
								  
			virtual void          Tick                  (float DeltaTime);
			virtual void          Draw                  (FPainter&, const I2DCoordMapper&);
			virtual void          DrawSmall             (FPainter&, const I2DCoordMapper&, FSlateBrush& Brush) const;

			virtual void          Notify                (EMessage, void*);

			bool                  IsOurPositionVisible    () const;
			virtual IGameObject*  CreateExplosionFireball (EExplosionFireball, CGameObjectCollection&);
			virtual bool          ExplosionHasFireball    () const;
			virtual void          Explode                 (CGameObjectCollection&);
			virtual EColor        GetExplosionColorBase   () const;
			virtual float         GetExplosionMass        () const;
			virtual void          AddExplosionDebris      (const FExplosionParams& Params, CGameObjectCollection& Debris);
			
			void                  ZeroVelocity            ();
			const CFPoint&        GetVelocity             () const;

			void                  MakeHurtable            (bool b = true);
			bool                  IsMortal                () const;
			bool                  ReduceLifespanBy        (float DeltaTime);
			void                  MarkAsDead              ();
			bool                  MarkedForDeath          () const;
			virtual void          OnAboutToDie            ();

			virtual float         GetCollisionForce       () const;
			bool                  IsCollisionInjurious    () const;   
			void                  SetCollisionInjurious   (bool b = true);
			bool                  IsInjurious             () const;
			bool                  CanBeInjured            () const;
			virtual bool          CanBeInjuredBySmartbomb () const { return true; }
			virtual void          GetInjurePt             (CFPoint&) const;
			virtual bool          TestInjury              (const CFRect&) const;

			virtual int32         GetPointValue           () const;

			// todo: external ownership is only used by the options arena, which we currently don't use,
			// and we should revisit why it was externally owning stuff because maybe we can drop it.
			// If we add a settings view, it won't use any GameObject subclasses anyway.
			//bool                  ExternallyOwned       () const; // todo: smart pointers would be better for this
			//void                  SetExternalOwnership  (bool b);

			bool                  IsMissionTarget         () const;
			void                  SetAsMissionTarget      (bool b = true);

			virtual void          CreateSprite            (EObjType Kind);
			virtual void          InstallSprite           ();
			virtual void          UninstallSprite         ();

			const FLinearColor&   GetRadarColor           () const;

			int32                 GetID                   () const { return Id; }
			void                  SetID                   (int32 N) { Id = N; }



			Orient2D        Orientation;    // Way the object is pointing/facing.
			CFPoint         Position;       // Arena coords.
			CFPoint         Inertia;        // Momentum vector (indicates direction and magnitude of travel during current frame).
											// Computed manually in every object's Tick function by comparing old vs. new Position.
											// todo: s/b able to drop this and use a temp var in upstream code.
			CFPoint         BboxRadius;
			I2DCoordMapper* MapperPtr          = nullptr;
			float           Lifespan           = 1.0f;       // Decremented by tick time, when zero, object is destroyed.
			bool            bMortal            = false;      // Set to true to use Lifespan.

			TSharedPtr<Daylon::SpritePlayObject2D> Sprite;

			// Lifespan is great for temporary objects like weapons fire and 
			// debris particles. The object processor will automatically lower the 
			// lifespan and when it reaches or goes  below zero, the object 
			// will be destroyed. Set bMortal to true to enable lifespan.

		protected:

			IGameObject*    PrevPtr                 = nullptr;
			IGameObject*    NextPtr                 = nullptr;

			EObjType        ParentType              = EObjType::UNKNOWN;
			EObjType        Type                    = EObjType::UNKNOWN;
			EObjType        CreatorType             = EObjType::UNKNOWN;

			int32           Id                      = -1;

			CFPoint         ScreenSize; // todo: env data s/b obtained thru APIs? But cache coherency...
			CFPoint         ArenaSize;

			float           Age                     = 0.0f; // 0 = new

			CFPoint         Velocity; // arena coords per second.
			float           Mass                    = 1.0f;
			float           Drag                    = 0.0f;

			FLinearColor    RadarColor;
			int32           PointValue              = 0;
			float           AnimSpeed               = 1.0f; // todo: will likely be dropped
			bool            bMissionTarget          = false;
			bool            bGrounded               = false;
			bool            bInjurious              = false;
			bool            bCanBeInjured           = false;
			bool            bIsCollisionInjurious   = false;


	}; // IGameObject 

} // namespace Defcon
