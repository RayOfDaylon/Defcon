// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DaylonUtils.h"
#include "GameObjects/obj_types.h"
#include "Globals/prefs.h"
#include "Common/util_geom.h"
#include "Common/util_color.h"
#include "Globals/GameObjectResources.h"

//#define LIFESPAN_DEAD 0.0001f


struct FPaintArguments;


namespace Defcon
{
	class I2DCoordMapper;

	enum class EMessage : uint8
	{
		TakenAboard,
		CarrierKilled,
		Released,
		HumanKilled,
		HumanTakenAboard
	};


	class CGameObjectCollection;


	class IGameObject
	{
		// Base class for all game entities.
		// A shot or piece of debris can be 
		// based on this class (although their drag 
		// coefficients tend to be very low or even zero). 

		public:

			IGameObject();
			virtual ~IGameObject();

#ifdef _DEBUG
			virtual const char* GetClassname() const = 0;
#endif

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
								  
			virtual void          Move                  (float DeltaTime);
			virtual void          Draw                  (FPaintArguments&, const I2DCoordMapper&);
			virtual void          DrawSmall             (FPaintArguments&, const I2DCoordMapper&, FSlateBrush& Brush);

			virtual void          Notify                (EMessage, void*);

			virtual IGameObject*  CreateFireball        (CGameObjectCollection&, float&);
			virtual bool          Fireballs             () const;
			virtual void          Explode               (CGameObjectCollection&);
			virtual int           GetExplosionColorBase () const;
			virtual float         GetExplosionMass      () const;
			IGameObject*          CreateFireblast       (CGameObjectCollection& debris, float& fBrightBase);
			virtual bool          Fireblasts            () const;
			
			void                  ZeroVelocity          ();
			const CFPoint&        GetVelocity           () const;

			void                  MakeHurtable          (bool b = true);
			bool                  IsMortal              () const;
			bool                  ReduceLifespanBy      (float DeltaTime);
			void                  MarkAsDead            ();
			bool                  MarkedForDeath        () const;
			virtual void          OnAboutToDie();

			virtual float         GetCollisionForce     () const;
			bool                  IsCollisionInjurious  () const;   
			void                  SetCollisionInjurious (bool b = true);
			bool                  IsInjurious           () const;
			bool                  CanBeInjured          () const;
			virtual void          GetInjurePt           (CFPoint&) const;
			virtual bool          TestInjury            (const CFRect&) const;

			virtual int32         GetPointValue         () const;

			bool                  ExternallyOwned       () const; // todo: smart pointers would be better for this
			void                  SetExternalOwnership  (bool b);

			bool                  IsMissionTarget       () const;
			void                  SetAsMissionTarget    (bool b = true);

			virtual void          CreateSprite          (EObjType Kind);
			virtual void          InstallSprite         ();
			virtual void          UninstallSprite       ();

			const FLinearColor&   GetRadarColor         () const;



			Orient2D        Orientation;    // Way the object is pointing/facing.
			CFPoint         Position;       // Arena coords.
			CFPoint         Inertia;        // Momentum vector (indicates direction and magnitude of travel during current frame).
											// Computed manually in every object's Move function by comparing old vs. new Position.
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

			bool            bExternallyOwned        = false;

			CFPoint         ScreenSize; // todo: env data s/b obtained thru APIs? But cache coherency...
			CFPoint         ArenaSize;

			float           Age                     = 0.0f; // 0 = new

			CFPoint         Velocity; // arena coords per second.
			float           Mass                    = 1.0f;
			float           Drag                    = 0.0f;

			bool            bInjurious              = false;
			bool            bCanBeInjured           = false;
			bool            bIsCollisionInjurious   = false;

			FLinearColor    RadarColor;
			int32           PointValue              = 0;
			float           AnimSpeed               = 1.0f; // todo: will likely be dropped
			bool            bMissionTarget          = false;
			bool            bGrounded               = false;

	}; // IGameObject 

} // namespace Defcon
