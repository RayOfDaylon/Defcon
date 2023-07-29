// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DaylonUtils.h"
#include "Common/PaintArguments.h"
#include "GameObjects/obj_types.h"
#include "Globals/prefs.h"
#include "Common/util_geom.h"
#include "Common/util_color.h"

#include "Globals/GameObjectResources.h"

//#define LIFESPAN_DEAD 0.0001f



class I2DCoordMapper;


namespace Defcon
{
	enum class Message : uint8
	{
		takenaboard,
		carrier_killed,
		released,
		human_killed,
		human_takenaboard
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
			ObjType               GetParentType         () const;
			ObjType               GetCreatorType        () const;
			void                  SetCreatorType        (ObjType n);
			ObjType               GetType               () const;
			void                  SetType               (ObjType n);
								  
			virtual void          Move                  (float DeltaTime);
			virtual void          Draw                  (FPaintArguments&, const I2DCoordMapper&);
			virtual void          DrawSmall             (FPaintArguments&, const I2DCoordMapper&, FSlateBrush& Brush);

			virtual void          Notify                (Message, void*);

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

			virtual size_t        GetPointValue         () const;

			bool                  ExternallyOwned       () const; // todo: smart pointers would be better for this
			void                  SetExternalOwnership  (bool b);

			bool                  IsMissionTarget       () const;
			void                  SetAsMissionTarget    (bool b = true);

			virtual void          CreateSprite          (ObjType Kind);
			virtual void          InstallSprite         ();
			virtual void          UninstallSprite       ();

			const FLinearColor&   GetRadarColor         () const;



			Orient2D        m_orient;       // Way the object is pointing/facing.
			CFPoint         m_pos;          // Arena coords.
			CFPoint         m_inertia;      // Momentum vector (indicates direction and magnitude of travel during current frame).
											// Computed manually in every object's Move function by comparing old vs. new m_pos.
											// todo: s/b able to drop this and use a temp var in upstream code.
			CFPoint         m_bboxrad;
			I2DCoordMapper* m_pMapper          = nullptr;
			float           m_fLifespan        = 1.0f;    // Decremented by tick time, when zero, object is destroyed.
			bool            m_bDrawSmall       = true;
			bool            m_bMortal          = false;      // Set to true to use m_fLifespan.
			UObject*        WorldContextObject = nullptr;

			TSharedPtr<Daylon::SpritePlayObject2D> Sprite;

			// Lifespan is great for temporary objects like weapons fire and 
			// debris particles. The object processor will automatically lower the 
			// lifespan and when it reaches or goes  below zero, the object 
			// will be destroyed. Set m_bMortal to true to enable lifespan.

		protected:

			IGameObject*    m_pPrev                 = nullptr;
			IGameObject*    m_pNext                 = nullptr;

			bool            m_bExternallyOwned      = false;


			CFPoint         m_screensize; // todo: env data s/b obtained thru APIs? But cache coherency...
			CFPoint         m_arenasize;

			float           m_fAge                  = 0.0f; // 0 = new

			CFPoint         m_velocity; // arena coords per second.
			float           m_fMass                 = 1.0f;
			float           m_fDrag                 = 0.0f;

			bool            m_bInjurious            = false;
			bool            m_bCanBeInjured         = false;
			bool            m_bIsCollisionInjurious = false;

			ObjType         m_parentType            = ObjType::UNKNOWN;
			ObjType         m_type                  = ObjType::UNKNOWN;
			ObjType         m_creatorType           = ObjType::UNKNOWN;

			FLinearColor    m_smallColor;
			size_t          m_pointValue            = 0;
			float           m_fAnimSpeed            = 1.0f; // todo: will likely be dropped
			bool            m_bMissionTarget        = false;
			bool            m_grounded              = false;

	}; // IGameObject 


	typedef std::function<void(IGameObject*, void*)> GameObjectProcessDelegate;

	class GameObjectProcessingParams
	{
		public:
			GameObjectProcessDelegate   fnOnDeath                      = nullptr;
			GameObjectProcessDelegate   fnOnEvery                      = nullptr;
			I2DCoordMapper*             pMapper                        = nullptr;  
			void*                       pvUser                         = nullptr;
			float                       fElapsedTime                   = 0.0f;
			float                       fArenaWidth                    = 0.0f;
			float                       fArenaHeight                   = 0.0f;
			bool                        UninstallSpriteIfObjectDeleted = false;
	};

} // namespace Defcon
