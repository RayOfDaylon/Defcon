// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
//#include "Runtime/GeometryCore/Public/BoxTypes.h"
//#include "UMG/Public/Components/Widget.h"
//#include "UMG/Public/Components/Image.h"
//#include "UMG/Public/Components/CanvasPanel.h"
//#include "UMG/Public/Components/CanvasPanelSlot.h"
#include "DaylonWidgetUtils.h"
#include "DaylonGeometry.h"
#include "SDaylonSprite.h"


namespace Daylon
{
	// Slate-only 2D play object
	// Warning: play objects combine views with models; if you need a view-model separation, prefer AActor and 
	// arrange for a visuals-only class (e.g. SImage or your own custom UWidgets/SWidgets) to do rendering.
	// You can still use PlayObject2D just for visuals, but you'll want to handle stuff like Inertia in a model,
	// not call Move(), etc.


	template <class SWidgetT>
	class PlayObject2D : public SWidgetT
	{
		// Stuff common to visible game entities like ships, player, bullets, etc.

		protected:

		SConstraintCanvas::FSlot* Slot = nullptr; // We need this because we cannot get a slot directly from an SWidget.

		FWidgetTransform RenderTransform; // Easy way to support GetAngle/SetAngle.


		public:

		FVector2D Inertia; // Direction and velocity, px/sec
		FVector2D OldPosition;
		FVector2D UnwrappedNewPosition;

		float LifeRemaining = 0.0f;
		float RadiusFactor  = 0.5f;
		float SpinSpeed     = 0.0f; // degrees/second
		int32 Value         = 0;


		virtual ~PlayObject2D() {}


		virtual FVector2D GetActualSize() const = 0;

		virtual void Update    (float DeltaTime) {}

		void SetSlot   (SConstraintCanvas::FSlot* InSlot) { check(Slot == nullptr); Slot = InSlot; }
		void ClearSlot () { check(Slot != nullptr); Slot = nullptr; }
		bool IsValid   () const { return ((Slot != nullptr) && (Value >= 0)); }
		bool IsAlive   () const { return (LifeRemaining > 0.0f); }
		bool IsDead    () const { return !IsAlive(); }
		bool IsVisible () const { return (IsValid() ? (GetVisibility() != EVisibility::Collapsed && GetVisibility() != EVisibility::Hidden) : false); }
		void Show      (bool Visible = true) { Daylon::Show(this, Visible); }
		void Hide      () { Show(false); }
		void Kill      () { LifeRemaining = 0.0f; Hide(); }


		void UpdateWidgetSize() 
		{
			if(!IsValid())
			{
				return;
			}

			SetSizeInSlot(GetActualSize());
		}

	
		float GetRadius() const
		{
			return GetSize().X * RadiusFactor;
		}


		FVector2D GetPosition() const
		{
			if(!IsValid())
			{
				return FVector2D(0);
			}

			const auto Margin = Slot->GetOffset();

			return FVector2D(Margin.Left, Margin.Top);
		}


		void SetPosition(const FVector2D& P)
		{
			if(!IsValid())
			{
				return;
			}

			auto Margin = Slot->GetOffset();
			Margin.Left = P.X;
			Margin.Top  = P.Y;

			Slot->SetOffset(Margin);
		}


		FVector2D GetNextPosition(float DeltaTime) const
		{
			return GetPosition() + (Inertia * DeltaTime);
		}


		FVector2D GetSize() const
		{
			if(!IsValid())
			{
				return FVector2D(0);
			}

			const auto Margin = Slot->GetOffset();

			return FVector2D(Margin.Right, Margin.Bottom);
		}


		void SetSizeInSlot(const FVector2D& S) 
		{
			if(!IsValid())
			{
				return;
			}

			auto Margin = Slot->GetOffset();

			Margin.Right  = S.X;
			Margin.Bottom = S.Y;

			Slot->SetOffset(Margin);
		}


		float GetSpeed() const
		{
			return Inertia.Length();
		}


		float GetAngle() const
		{
			return RenderTransform.Angle;
		}


		void SetAngle(float Angle)
		{
			if(!IsValid())
			{
				return;
			}

			RenderTransform.Angle = Angle;
			SetRenderTransform(RenderTransform.ToSlateRenderTransform());
		}


		FVector2D GetDirectionVector() const
		{
			if(!IsValid())
			{
				return FVector2D(0.0f);
			}

			return AngleToVector2D(GetAngle());
		}


		void Move(float DeltaTime, const TFunction<FVector2D(const FVector2D&)>& WrapFunction)
		{
			const auto P = GetPosition();

			OldPosition          = P;
			UnwrappedNewPosition = P + Inertia * DeltaTime;

			SetPosition(WrapFunction(UnwrappedNewPosition));
		}


		void Start(const FVector2D& P, const FVector2D& InInertia, float InLifeRemaining)
		{
			if(!IsValid())
			{
				//UE_LOG(LogDaylon, Error, TEXT("FPlayObjectEx::Spawn: invalid widget!"));
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


	class ImagePlayObject2D : public PlayObject2D<SImage>
	{
		protected:

		FSlateBrush Brush;

		
		public:

		virtual FVector2D GetActualSize() const override { return Brush.GetImageSize(); }


		void SetBrush(const FSlateBrush& InBrush)
		{
			Brush = InBrush;
			SetImage(&Brush);
			UpdateWidgetSize();
		}
	};


	class SpritePlayObject2D : public PlayObject2D<SDaylonSprite>
	{
		public:

		virtual FVector2D GetActualSize() const override { return Size; }

		virtual void Update(float DeltaTime) override { SDaylonSprite::Update(DeltaTime); }
	};


	template <class SWidgetT>
	void Install(TSharedPtr<PlayObject2D<SWidgetT>> Widget, float InRadiusFactor)
	{
		Widget->SetRenderTransformPivot(FVector2D(0.5f));

		auto Canvas = GetRootCanvas()->GetCanvasWidget();

		auto SlotArgs = Canvas->AddSlot();

		Widget->SetSlot(SlotArgs.GetSlot());

		SlotArgs[Widget.ToSharedRef()];
		SlotArgs.AutoSize(true);
		SlotArgs.Alignment(FVector2D(0.5));

		Widget->RadiusFactor = InRadiusFactor;
	}


	DAYLONGRAPHICSLIBRARY_API TSharedPtr<ImagePlayObject2D>  SpawnImagePlayObject2D  (const FSlateBrush& Brush, float Radius);
	DAYLONGRAPHICSLIBRARY_API TSharedPtr<SpritePlayObject2D> SpawnSpritePlayObject2D (const FDaylonSpriteAtlas& Atlas, const FVector2D& S, float Radius);

	DAYLONGRAPHICSLIBRARY_API void UninstallImpl(TSharedPtr<SWidget> Widget);

	DAYLONGRAPHICSLIBRARY_API void Uninstall(TSharedPtr<ImagePlayObject2D> Widget);
	DAYLONGRAPHICSLIBRARY_API void Uninstall(TSharedPtr<SpritePlayObject2D> Widget);


	template <typename T>
	bool PlayObjectsIntersectBox(const TArray<TSharedPtr<T>>& PlayObjects, const UE::Geometry::FAxisAlignedBox2d& Box)
	{
		for(const auto& PlayObject : PlayObjects)
		{
			const auto ObjectHalfSize = PlayObject->GetSize() / 2;

			const UE::Geometry::FAxisAlignedBox2d ObjectBox
			(
				PlayObject->UnwrappedNewPosition - ObjectHalfSize,
				PlayObject->UnwrappedNewPosition + ObjectHalfSize
			);

			if(ObjectBox.Intersects(Box))
			{
				return true;
			}
		}
		return false;
	}
}
