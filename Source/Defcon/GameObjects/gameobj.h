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

//#define LIFESPAN_DEAD	0.0001f


#if 0
class C2DSprite
{
	// The appearance of a 2D object.
	public:
		C2DSprite();
		virtual ~C2DSprite();

		virtual void Load(const char*);

	private:
		FPaintArguments		m_dib;
			
}; // C2DSprite
#endif

class I2DCoordMapper;


namespace Defcon
{
	enum class Message
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

			virtual void    Init(const CFPoint& ArenaSize, const CFPoint& ScreenSize) { m_arenasize = ArenaSize; m_screensize = ScreenSize; }

			// Identify stuff. Every object has a type
			// and also the type of the object that made it.
			ObjType			GetParentType	() const { return m_parentType; }
			ObjType			GetCreatorType	() const { return m_creatorType; }
			void			SetCreatorType	(ObjType n) { m_creatorType = n; }
			ObjType			GetType			() const { return m_type; }
			void            SetType         (ObjType n) { m_type = n; }
			virtual void	Notify			(Message, void*);

			virtual void    OnFinishedCreating () {}


			virtual bool	OccursFrequently() const { return false; }

#ifdef _DEBUG
			virtual const char* GetClassname() const = 0;
#endif

			virtual IGameObject*	CreateFireball			(CGameObjectCollection&, float&);
			virtual bool			Fireballs				() const { return false; }
			virtual void			Explode					(CGameObjectCollection&);
			virtual int				GetExplosionColorBase	() const;
			virtual float			GetExplosionMass		() const { return 1.0f; }
			

			// Move() simply updates position based on thrust/etc.
			virtual void Move		(float);
			virtual void Draw		(FPaintArguments&, const I2DCoordMapper&);
			//virtual void DrawBbox	(FPaintArguments&, const I2DCoordMapper&);
			virtual void DrawSmall	(FPaintArguments&, const I2DCoordMapper&, FSlateBrush& Brush);

			IGameObject* GetNext	() { return m_pNext; }
			IGameObject* GetPrev	() { return m_pPrev; }

			void SetNext	(IGameObject* p) { m_pNext = p; }
			void SetPrev	(IGameObject* p) { m_pPrev = p; }

			void			ZeroVelocity	()				{ m_velocity.set(0,0); }
			const CFPoint&	GetVelocity		() const				{ return m_velocity; }


			bool IsMortal			() const { return m_bMortal; }
			bool ReduceLifespanBy	(float f)
				{
					m_fLifespan = FMath::Max(0.0f, m_fLifespan - f);
					return (m_fLifespan <= 0.0f);
				}

			void MarkAsDead		()			{ m_fLifespan = 0.0001f; m_bMortal = true; }
			bool MarkedForDeath	() const 	{ return (m_bMortal && m_fLifespan <= 0.0001f); }

			virtual float GetCollisionForce	() const
				{
					// todo: we want to make this a 
					// per-class value and multiply by 
					// the object's velocity.
					return 0.01f * COLLISION_DAMAGE; 
				}

			bool IsCollisionInjurious	() const				{ return m_bIsCollisionInjurious; }
			void SetCollisionInjurious	(bool b = true) 		{ m_bIsCollisionInjurious = b; }
			bool IsInjurious			() const 				{ return m_bInjurious; }
			bool CanBeInjured			() const				{ return m_bCanBeInjured; }

			virtual void GetInjurePt	(CFPoint&) const;
			virtual bool TestInjury		(const CFRect&) const;



			virtual void	OnAboutToDie();

			virtual size_t	GetPointValue() const				{ return m_pointValue; }


			bool ExternallyOwned		() const				{ return m_bExternallyOwned; }

			void SetExternalOwnership	(bool b)				{ m_bExternallyOwned = b; }


			bool IsMissionTarget		() const { return m_bMissionTarget; }
			void SetAsMissionTarget		(bool b = true) { m_bMissionTarget = b; }
			void MakeHurtable			(bool b = true) { m_bCanBeInjured = b; }

			IGameObject* CreateFireblast	(CGameObjectCollection& debris, float& fBrightBase);
			virtual bool Fireblasts			() const { return false; }


			// Lifespan is great for 
			// temporary objects like weapons fire and 
			// debris particles. The system will automatically 
			// lower the lifespan and when it reaches or goes 
			// below zero, the object will be destroyed.
			// Set m_bMortal to true to enable lifespan.

			Orient2D        m_orient;       // Way the object is pointing/facing.
			CFPoint         m_pos;		    // Arena coords.
			CFPoint         m_inertia;	    // Momentum vector (indicates direction and magnitude of travel during current frame).
			                                // Computed manually in every object's Move function by comparing old vs. new m_pos.

			CFPoint         m_bboxrad;
			I2DCoordMapper* m_pMapper = nullptr;
			float           m_fLifespan; // Lifespan in seconds.
			bool            m_bDrawSmall;
			bool            m_bMortal;


			virtual void CreateSprite(ObjType Kind)
			{
				const auto& AtlasInfo = GameObjectResources.Get(Kind);

				Sprite = SNew(Daylon::SpritePlayObject2D);

				Sprite->SetAtlas(AtlasInfo.Atlas->Atlas);
				Sprite->SetSize(AtlasInfo.Size);
				Sprite->UpdateWidgetSize();
			}

			// Called when a view that renders sprites becomes active and goes away.
			virtual void  InstallSprite    ();
			virtual void  UninstallSprite  ();

			UObject*    WorldContextObject = nullptr;

			TSharedPtr<Daylon::SpritePlayObject2D> Sprite;

			const FLinearColor&   GetRadarColor() const { return m_smallColor; }

			//void          SetDoesMaterialization  (bool b = true) { m_bMaterializes = b; }
			//virtual bool  IsMaterializing         () const { return m_bMaterializes && m_materializationAge < ENEMY_BIRTHDURATION; }
			//virtual void  UpdateMaterialization   (float DeltaTime) {};
			//virtual void  DrawMaterialization     (FPaintArguments&, const I2DCoordMapper&) const {};



		protected:

			bool		m_bExternallyOwned;

			CFPoint		m_screensize;
			CFPoint		m_arenasize;


			//bool        m_bMaterializes = false;
			//float       m_materializationAge = 0.0f;


			float		m_fAge = 0.0f; // 0 = new
			//float		m_fNow;

			CFPoint		m_velocity;	// arena coords per second.
			float		m_fMass;
			float		m_fDrag;

			IGameObject*	m_pPrev = nullptr;
			IGameObject*	m_pNext = nullptr;

			bool		m_bInjurious;
			bool		m_bCanBeInjured;
			bool		m_bIsCollisionInjurious;

			ObjType       m_parentType;
			ObjType		  m_type;
			ObjType		  m_creatorType;
			FLinearColor  m_smallColor;
			size_t		  m_pointValue;
			float		  m_fAnimSpeed; // todo: will likely be dropped
			bool		  m_bMissionTarget;
			bool		  m_grounded = false;

	}; // IGameObject 


	//typedef void (*GameObjectProcessCallback)(IGameObject*, void*);
	typedef std::function<void(IGameObject*, void*)> GameObjectProcessCallback;

	class GameObjectProcess
	{
		public:
			I2DCoordMapper*				pMapper			= nullptr;	
			GameObjectProcessCallback	fnOnDeath		= nullptr;
			GameObjectProcessCallback	fnOnEvery		= nullptr;
			void*						pvUser			= nullptr;
			float						fElapsedTime	= 0.0f;
			float						fArenaWidth		= 0.0f;
			float                       fArenaHeight    = 0.0f;
			bool                        UninstallSpriteIfObjectDeleted = false;
	};

} // namespace Defcon
