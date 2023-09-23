// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#pragma once


#if 0

// -------------------------------------------------------------------------------------------------------------------
// Legacy UMG-based 2D play object types.


namespace Daylon
{
	struct IPlayObject
	{
		virtual UCanvasPanelSlot*       GetWidgetSlot () = 0;
		virtual const UCanvasPanelSlot* GetWidgetSlot () const = 0;

		virtual void      UpdateWidgetSize () = 0;
		virtual void      DestroyWidget    () = 0;
									   
		virtual bool      IsValid          () const = 0;
		virtual bool      IsAlive          () const = 0;
		virtual bool      IsDead           () const = 0;
		virtual bool      IsVisible        () const = 0;
		virtual void      Show             (bool Visible = true) = 0;
		virtual void      Hide             () = 0;
		virtual void      Kill             () = 0;
		virtual float     GetRadius        () const = 0;
		virtual FVector2D GetPosition      () const = 0;
		virtual void      SetPosition      (const FVector2D& P) = 0;
		virtual FVector2D GetNextPosition  (float DeltaTime) const = 0;
		virtual FVector2D GetSize          () const = 0;
		virtual float     GetSpeed         () const = 0;
		virtual float     GetAngle         () const = 0;
		virtual void      SetAngle         (float Angle) = 0;
		virtual void      Tick             (float DeltaTime) = 0;
	};



	struct ImageWidgetSpecifics
	{
		FVector2D GetSize(UImage* Image) const { return Image->Brush.GetImageSize(); }
	};


	struct SpriteWidgetSpecifics
	{
		FVector2D GetSize(UDaylonSpriteWidget* Sprite) const { return Sprite->Size; }
	};


	template <class WidgetT, class WidgetSpecificsT>
	struct FPlayObject : public IPlayObject
	{
		// Stuff common to visible game entities like ships, player, bullets, etc.

		WidgetT* Widget = nullptr; // Associated widget

		WidgetSpecificsT WidgetSpecifics;


		void FinishCreating(float InRadiusFactor)
		{
			check(Widget);

			Widget->SetRenderTransformPivot(FVector2D(0.5f));

			auto CanvasSlot = Daylon::GetRootCanvas()->AddChildToCanvas(Widget);
			CanvasSlot->SetAnchors(FAnchors());
			CanvasSlot->SetAutoSize(true);
			CanvasSlot->SetAlignment(FVector2D(0.5));

			// Autosize(true) doesn't automatically set the slot offsets to the image size, so do it here.
			UpdateWidgetSize();

			RadiusFactor = InRadiusFactor;
		}


		void UpdateWidgetSize() 
		{
			auto Size = WidgetSpecifics.GetSize(Widget); 
		
			auto Margin = GetWidgetSlot()->GetOffsets();

			Margin.Right  = Size.X;
			Margin.Bottom = Size.Y;

			GetWidgetSlot()->SetOffsets(Margin);
		}


		void DestroyWidget()
		{
			if(Widget == nullptr)
			{
				return;
			}

			Widget->GetParent()->RemoveChild(Widget);
			//WidgetTree->RemoveWidget (Widget);
		}


		FVector2D Inertia; // Direction and velocity, px/sec
		FVector2D OldPosition;
		FVector2D UnwrappedNewPosition;

		float LifeRemaining = 0.0f;
		float RadiusFactor  = 0.5f;
		float SpinSpeed     = 0.0f; // degrees/second
		int32 Value         = 0;


		bool IsAlive   () const { return (LifeRemaining > 0.0f); }
		bool IsDead    () const { return !IsAlive(); }
		bool IsVisible () const { return (Widget != nullptr ? Widget->IsVisible() : false); }
		void Show      (bool Visible = true) { Daylon::Show(Widget, Visible); }
		void Hide      () { Show(false); }
		void Kill      () { LifeRemaining = 0.0f; Hide(); }

	
		bool IsValid   () const 
		{ 
			return (Value >= 0) && (Widget != nullptr) && (Cast<UCanvasPanelSlot>(Widget->Slot) != nullptr);
		}

	
		float GetRadius() const
		{
			return GetSize().X * RadiusFactor;
		}


		UCanvasPanelSlot*       GetWidgetSlot () { return Cast<UCanvasPanelSlot>(Widget->Slot); }
		const UCanvasPanelSlot* GetWidgetSlot () const { return Cast<UCanvasPanelSlot>(Widget->Slot); }


		FVector2D GetPosition() const
		{
			if(Widget == nullptr || Widget->Slot == nullptr)
			{
				return FVector2D(0);
			}

			return GetWidgetSlot()->GetPosition();
		}


		void SetPosition(const FVector2D& P)
		{
			if(Widget == nullptr || Widget->Slot == nullptr)
			{
				return;
			}

			return GetWidgetSlot()->SetPosition(P);
		}


		FVector2D GetNextPosition(float DeltaTime) const
		{
			return GetPosition() + (Inertia * DeltaTime);
		}


		FVector2D GetSize() const
		{
			if(Widget == nullptr || Widget->Slot == nullptr)
			{
				return FVector2D(0);
			}

			return GetWidgetSlot()->GetSize();
		}


		float GetSpeed() const
		{
			return Inertia.Length();
		}


		float GetAngle() const
		{
			return (Widget == nullptr ? 0.0f : Widget->GetRenderTransformAngle());
		}


		void SetAngle(float Angle)
		{
			if(Widget == nullptr)
			{
				return;
			}

			Widget->SetRenderTransformAngle(Angle);
		}


		virtual void Tick(float DeltaTime) {}


		void Move(float DeltaTime, const TFunction<FVector2D(const FVector2D&)>& WrapFunction)
		{
			const auto P = GetPosition();

			OldPosition          = P;
			UnwrappedNewPosition = P + Inertia * DeltaTime;

			SetPosition(WrapFunction(UnwrappedNewPosition));
		}


		void Spawn(const FVector2D& P, const FVector2D& InInertia, float InLifeRemaining)
		{
			if(Widget == nullptr)
			{
				UE_LOG(LogDaylon, Error, TEXT("FPlayObject::Spawn: no widget member."));
				return;
			}

			OldPosition          =
			UnwrappedNewPosition = P;
			Inertia              = InInertia;
			LifeRemaining        = InLifeRemaining;

			SetPosition(P);
			Show();
		}

	};


	struct DAYLONGRAPHICSLIBRARY_API FImagePlayObject  : public FPlayObject<UImage, ImageWidgetSpecifics> 
	{
		void Create(const FSlateBrush& Brush, float Radius)
		{
			Widget = Daylon::MakeWidget<UImage>();
			check(Widget);

			Widget->Brush = Brush;

			FinishCreating(Radius);
		}


		void SetBrush(const FSlateBrush& Brush)
		{
			Widget->Brush = Brush;
		}
	};

	
	struct DAYLONGRAPHICSLIBRARY_API FSpritePlayObject : public FPlayObject<UDaylonSpriteWidget, SpriteWidgetSpecifics> 
	{
		void Create(UDaylonSpriteWidgetAtlas* WidgetAtlas, float InRadiusFactor, const FLinearColor& Tint, const FVector2D& Size)
		{
			check(WidgetAtlas);
			check(Size.X > 0 && Size.Y > 0);

			Widget = Daylon::MakeWidget<UDaylonSpriteWidget>();
			check(Widget);

			Widget->TextureAtlas = WidgetAtlas;
			Widget->TextureAtlas->Atlas.AtlasBrush.TintColor = Tint;
			Widget->Size = Size;

			FinishCreating(InRadiusFactor);

			Widget->SynchronizeProperties();
		}
	};
}
#endif // 0
