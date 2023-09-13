// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/GeometryCore/Public/BoxTypes.h"
#include "UMG/Public/Blueprint/WidgetTree.h"
#include "UMG/Public/Components/Widget.h"
#include "UMG/Public/Components/Image.h"
#include "UMG/Public/Components/CanvasPanel.h"
#include "UMG/Public/Components/CanvasPanelSlot.h"
#include "UDaylonSpriteWidget.h"
#include "DaylonUtils.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogDaylon, Log, All);

class UTextBlock;


UCLASS()
class DAYLONGRAPHICSLIBRARY_API UDaylonUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// Classname reserved for future use.
};


namespace Daylon
{
	extern UWidgetTree*  WidgetTree;
	extern UCanvasPanel* RootCanvas;

	extern const double Epsilon;

	DAYLONGRAPHICSLIBRARY_API FORCEINLINE double Square (double x) { return x * x; }

	DAYLONGRAPHICSLIBRARY_API void          SetWidgetTree                     (UWidgetTree* InWidgetTree);
	DAYLONGRAPHICSLIBRARY_API UWidgetTree*  GetWidgetTree                     ();
	DAYLONGRAPHICSLIBRARY_API void          SetRootCanvas                     (UCanvasPanel* InCanvas);
	DAYLONGRAPHICSLIBRARY_API UCanvasPanel* GetRootCanvas                     ();

	DAYLONGRAPHICSLIBRARY_API FVector2D     GetWidgetPosition                 (const UWidget* Widget);
	DAYLONGRAPHICSLIBRARY_API FVector2D     GetWidgetSize                     (const UWidget* Widget);
	DAYLONGRAPHICSLIBRARY_API FVector2D     GetWidgetDirectionVector          (const UWidget* Widget);
	DAYLONGRAPHICSLIBRARY_API void          Show                              (UWidget*, bool Visible = true);
	DAYLONGRAPHICSLIBRARY_API void          Show                              (SWidget*, bool Visible = true);
	DAYLONGRAPHICSLIBRARY_API void          Hide                              (UWidget* Widget);
	DAYLONGRAPHICSLIBRARY_API void          Hide                              (SWidget* Widget);
	DAYLONGRAPHICSLIBRARY_API void          UpdateRoundedReadout              (UTextBlock* Readout, float Value, int32& OldValue);

	DAYLONGRAPHICSLIBRARY_API FVector2D     AngleToVector2D                   (float Angle);
	DAYLONGRAPHICSLIBRARY_API FVector2D     RandVector2D                      ();
	DAYLONGRAPHICSLIBRARY_API FVector2D     Rotate                            (const FVector2D& P, float Angle); 
	DAYLONGRAPHICSLIBRARY_API FVector2D     DeviateVector                     (const FVector2D& VectorOld, float MinDeviation, float MaxDeviation);
	DAYLONGRAPHICSLIBRARY_API float         Vector2DToAngle                   (const FVector2D& Vector);
	DAYLONGRAPHICSLIBRARY_API float         WrapAngle                         (float Angle);
	DAYLONGRAPHICSLIBRARY_API float         Normalize                         (float N, float Min, float Max);

	DAYLONGRAPHICSLIBRARY_API bool          DoesLineSegmentIntersectCircle    (const FVector2D& P1, const FVector2D& P2, const FVector2D& CP, double R);
	DAYLONGRAPHICSLIBRARY_API bool          DoesLineSegmentIntersectTriangle  (const FVector2D& P1, const FVector2D& P2, const FVector2D Triangle[3]);
	DAYLONGRAPHICSLIBRARY_API bool          DoTrianglesIntersect              (const FVector2D TriA[3], const FVector2D TriB[3]);
	DAYLONGRAPHICSLIBRARY_API bool          DoCirclesIntersect                (const FVector2D& C1, float R1, const FVector2D& C2, float R2);
	DAYLONGRAPHICSLIBRARY_API FVector2D     RandomPtWithinBox                 (const FBox2d& Box);
	DAYLONGRAPHICSLIBRARY_API FVector2D     ComputeFiringSolution             (const FVector2D& LaunchP, float TorpedoSpeed, const FVector2D& TargetP, const FVector2D& TargetInertia);


	template<class WidgetT> static WidgetT* MakeWidget()
	{
		auto Widget = GetWidgetTree()->ConstructWidget<WidgetT>();
		check(Widget);
		return Widget;
	}


	UENUM()
	enum class EListNavigationDirection : int32
	{
		// These values are casted to int
		Backwards = -1,
		Forwards  =  1,
	};


	
	UENUM()
	enum class ERotationDirection : int32
	{
		// These values are casted to int
		CounterClockwise = -1,
		Clockwise        =  1,
	};


	template <typename T> void Order(T& A, T& B)
	{
		if(A > B)
		{
			Swap(A, B);
		}
	}


	template <typename T> struct FRange
	{
		FRange() : _Low(0), _High(0) {}
		FRange(T Lo, T Hi) : _Low(Lo), _High(Hi) {}

		void Set       (T Lo, T Hi) { _Low = Lo; _High = Hi; Fix(); }
		void SetLow    (T Lo)       { _Low = Lo; Fix(); }
		void setHigh   (T Hi)       { _High = Hi; Fix(); }

		void SetUnsafe (T Lo, T Hi)  { _Low = Lo; _High = Hi; }

		operator T () const { return (_High - _Low); }
		void operator = (T N) { Set(N, N); }

		void Include(T N) 
		{
			if(N < _Low)
			{
				_Low = N;
			}

			if(N > _High)
			{
				_High = N;
			}
		}

		void Include(const FRange<T>& Range) 
		{
			Include(Range.Low());
			Include(Range.High());
		}

		T Low  () const { return _Low; }
		T High () const { return _High; }
		/*T Map(T N, const FRange<T>& Target)
		{
			return ((n - _Low) / *this) * Target + Target._Low();
		}*/

		bool Contains(T N) const { return (N >= _Low && N <= _High); }

		//T Lerp(T F) const { return FMath::Lerp(_Low, _High, F); }


		protected:

			T	_Low, _High;

			void Fix() { Order(_Low, _High); }
	};


	template <typename T> T Lerp(const FRange<T>& Range, float F)
	{
		return FMath::Lerp(Range.Low(), Range.High(), F);
	}


	template <typename T> T Average(const FRange<T>& Range)
	{
		return (Range.Low() + Range.High()) / 2;
	}


	struct DAYLONGRAPHICSLIBRARY_API FLoopedSound
	{
		// A hacked sound loop created by simply playing the same sound
		// over and over if Tick() is called.

		USoundBase* Sound = nullptr;

		float VolumeScale  = 1.0f;
		float VolumeScale2 = 1.0f;

		UObject* WorldContextPtr = nullptr;

		void Set(UObject* Context, USoundBase* InSound, float InVolumeScale = 1.0f)
		{
			WorldContextPtr = Context;
			Sound           = InSound;
			VolumeScale     = InVolumeScale;
		}

		void Start(float InVolumeScale = 1.0f);

		void Tick(float DeltaTime);

		protected:

			float TimeRemaining = 0.0f;
	};


	struct DAYLONGRAPHICSLIBRARY_API FScheduledTask
	{
		// A TFunction that runs a specified number of seconds after the task is created.
		// You'll normally want to specify What as a lambda that captures a TWeakObjPtr<UObject-derived class>
		// e.g. This=this, so that you can test the weak pointer before running the lambda.


		float  When = 0.0f;   // Number of seconds into the future.

		TFunction<void()> What;

		bool Tick(float DeltaTime)
		{
			// Return true when function finally executes.

			if(!What)
			{
				return false;
			}

			When -= DeltaTime;

			if(When > 0.0f)
			{
				return false;
			}
		
			What();

			return true;
		}
	};


	struct DAYLONGRAPHICSLIBRARY_API FDurationTask
	{
		// A TFunction that runs every time Tick() is called until the task duration reaches zero, 
		// at which point its completion TFunction will be called.
		// See FScheduledTask for more comments.


		float  Duration = 0.0f;   // number of seconds to run the task for.

		TFunction<void(float)> What;
		TFunction<void()>      Completion;

		bool Tick(float DeltaTime)
		{
			// Return true if task is still active.

			if(!What)
			{
				return false;
			}

			if(Elapsed >= Duration)
			{
				if(Completion)
				{
					Completion();
				}
				return false;
			}

			What(Elapsed);

			Elapsed += DeltaTime;

			return true;
		}

		protected:
			float Elapsed = 0.0f;
	};


	// A type that acts like a given type except that when the value is modified,
	// it invokes a delegate.
	template <typename T>
	class TBindableValue
	{
		protected:

			T                             Value;
			TFunction<void(const T& Val)> Delegate;


		public:

			TBindableValue()
			{
			}


			TBindableValue(const T& Val, TFunction<void(const T& Val)> Del) : Value(Val), Delegate(Del) 
			{
			}


			operator T& () { return Value; }
			operator const T& () const { return Value; }

			// Some calls to operator T& won't compile e.g. as variadic args, but these should work.
			T& GetValue() { return (T&)*this; }
			const T& GetValue() const { return (T&)*this; }

			void Bind(TFunction<void(const T& Val)> Del)
			{
				Delegate = Del;

				if(Delegate)
				{
					Delegate(Value);
				}
			}



			TBindableValue& operator = (const T& Val)
			{
				if(Value != Val)
				{
					Value = Val;

					if(Delegate)
					{
						Delegate(Value);
					}
				}

				return *this;
			}

			TBindableValue& operator += (const T& Val) { *this = *this + Val; return *this;	}
			TBindableValue& operator -= (const T& Val) { *this = *this - Val; return *this;	}
			TBindableValue& operator *= (const T& Val) { *this = *this * Val; return *this;	}
			TBindableValue& operator /= (const T& Val) { *this = *this / Val; return *this; }

			T operator -- (int)  { T temp = *this; *this -= (T)1; return temp; }
			TBindableValue& operator -- ()  { *this -= (T)1; return *this; }

			T operator ++ (int)  { T temp = *this; *this += (T)1; return temp; }
			TBindableValue& operator ++ ()  { *this += (T)1; return *this; }

			bool operator <  (const T& Val) const { return (Value < Val); }
			bool operator <= (const T& Val) const { return (Value <= Val); }
			bool operator == (const T& Val) const { return (Value == Val); }
			bool operator >= (const T& Val) const { return (Value >= Val); }
			bool operator >  (const T& Val) const { return (Value > Val); }
			bool operator != (const T& Val) const { return (Value != Val); }
	};


	// --------------------------------------------------------------------------------------------------------

	/*
		TMessageMediator (and helper types).

		Class which promotes loose coupling by mediating messages between objects.
		The mediator acts like a post office, registering recipients' delegates as message consumers, 
		receiving messages and then forwarding them to them. Senders and recipients have no 
		knowledge about each other, but must agree on the message payload format.

									 ______________          _________________________
		 ________                   |              |        |                         |
		|        |                  |              | -----> | msg consumer (delegate) |
		| sender | --- message ---> |   Mediator   |        |_________________________|
		|________|                  |              |        |                         |
									|______________|        |      consumer #2        |
															|_________________________|
																	   ...
															 _________________________
															|                         |
															|      consumer #n        |
															|_________________________|


		No async operation; senders are blocked until a sent message is processed;
		the entire codepath is on the same thread.

		Messages are "fire and forget"; delegate signature has void return type.

		The Tenum template argument must be an enum class with at least one member called "Unknown".
	*/


	// Message delegates have a simple API: a single void* argument, void return type.
	// If a sender wants a return value, they can have their payload include a non-const pointer
	// to some data that the consumer can modify.

	typedef TFunction<void(void* Payload)> FMessageDelegate;


	template <typename Tenum> struct FMessageConsumer 
	{
		void*                  Object   = nullptr; 
		Tenum                  Message  = Tenum::Unknown; 
		FMessageDelegate       MessageDelegate; 

		FMessageConsumer(void* InObject, Tenum InMessage, const FMessageDelegate& InMessageDelegate)
		{
			check(InObject != nullptr);
			check(InMessage != Tenum::Unknown);
			check(InMessageDelegate);

			Object          = InObject;
			Message         = InMessage;
			MessageDelegate = InMessageDelegate;
		}


		bool IsValid() const 
		{
			return (Object != nullptr && Message != Tenum::Unknown && MessageDelegate); 
		}


		bool operator == (const FMessageConsumer& Rhs) const
		{
			return (Object == Rhs.Object && Message == Rhs.Message);
		}
	};


	template <typename Tenum> class TMessageMediator
	{
		// No copying allowed.
		TMessageMediator (const TMessageMediator&) = delete;
		TMessageMediator& operator= (const TMessageMediator&) = delete;


		public:

			TMessageMediator() {}

			void Send(Tenum Message, void* Payload = nullptr)
			{
				for(const auto& Consumer : Consumers)
				{
					if(Consumer.Message == Message)
					{
						Consumer.MessageDelegate(Payload);
					}
				}
			}


			void RegisterConsumer(const FMessageConsumer<Tenum>& Consumer)
			{
				check(Consumer.IsValid());

				if(!HasConsumer(Consumer))
				{
					Consumers.Add(Consumer);
				}
			}


			void UnregisterConsumer(void* Object)
			{
				// Remove all delegates which recipient had registered.

				// Called when object no longer needs to receive messages, or
				// when the object is being deleted.

			    for(int32 Idx = Consumers.Num() - 1; Idx >= 0; Idx--)
				{
					if(Consumers[Idx].Object == Object)
					{
						Consumers.RemoveAtSwap(Idx);
					}
				}

				// We don't expect to need unregistering of a particular delegate i.e. Object + Message.
			}


		protected:

			TArray<FMessageConsumer<Tenum>> Consumers;

			bool HasConsumer(const FMessageConsumer<Tenum>& InConsumer) const
			{
				for(const auto& Consumer : Consumers)
				{
					if(Consumer == InConsumer)
					{
						return true;
					}
				}

				return false;
			}
	};


	template <typename Tval, typename Tenum> class TMessageableValue
	{
		// Use this template instead of a normal value type in order to 
		// make it sendable to a message mediator.
		// If Tval is non-scalar, it must provide assignment operator and operator != .

		protected:

			Tval                     Value;
			TMessageMediator<Tenum>* MessageMediator = nullptr;
			Tenum                    Message         = Tenum::Unknown;
			bool                     FirstTime       = true;


		public:

			Tval Get() const { return Value; }

			
			void Set(Tval Val, bool Force = false) 
			{
				check(MessageMediator != nullptr);
				check(Message != Tenum::Unknown);

				const bool Different = (Value != Val);

				Value = Val; 

				if(Different || FirstTime || Force)
				{
					MessageMediator->Send(Message, &Value);
					FirstTime = false;
				}
			}


			void Bind(TMessageMediator<Tenum>* InMessageMediator, Tenum InMessage)
			{
				check(InMessageMediator != nullptr);
				check(InMessage != Tenum::Unknown);

				MessageMediator = InMessageMediator;
				Message         = InMessage;
			}
	};

	// --------------------------------------------------------------------------------------------------------
	
	
	struct DAYLONGRAPHICSLIBRARY_API FHighScore
	{
	
		FString Name;
		int32   Score = 0;

		
		FHighScore() {}
		FHighScore(int32 InScore, const FString& InName) : Name(InName), Score(InScore)	{}

		
		void Set(int32 InScore, const FString& InName)
		{
			Name  = InName;
			Score = InScore;
		}


		bool operator == (const FHighScore& Rhs) const
		{
			return (Name == Rhs.Name && Score == Rhs.Score);
		}


		bool operator < (const FHighScore& Rhs) const
		{
			if(Score == Rhs.Score)
			{
				return (Name.Compare(Rhs.Name) < 0);
			}

			return (Score < Rhs.Score); 
		}
	};


	struct DAYLONGRAPHICSLIBRARY_API FHighScoreTable
	{
		int32 MaxEntries    = 10;
		int32 MaxNameLength = 30; // Max. number of characters a name can have. Original Asteroids game used 3 chars.

		TArray<FHighScore> Entries;

		int32  GetLongestNameLength () const;
		bool   CanAdd               (int32 Score) const;
		void   Add                  (int32 Score, const FString& Name);
		void   Clear                () { Entries.Empty(); }
	};




	// -------------------------------------------------------------------------------------------------------------------
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


	// Mersenne Twister random number generator -- a C++ class MTRand
	// Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
	// Richard J. Wagner  v1.0  15 May 2003  rjwagner@writeme.com

	// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
	// Copyright (C) 2000 - 2003, Richard J. Wagner
	// All rights reserved.
	
	// Modified slightly (e.g. args passed by const scalar reference are passed by value,
	// unneeded functions ommitted, no 'register' keyword, etc.)

	class MTRand
	{
		// Data
		public:
	
			enum { N = 624 };       // length of state vector
			enum { SAVE = N + 1 };  // length of array for save()


		protected:

			enum { M = 397 };  // period parameter
	
			uint32  state[N];   // internal state
			uint32* pNext;      // next value to get from state
			int     left;       // number of values left before reload needed


		public:

			MTRand(const uint32& oneSeed);  // initialize with a simple uint32
			MTRand();  // auto-initialize with /dev/urandom or time() and clock()
	
			// Do NOT use for CRYPTOGRAPHY without securely hashing several returned
			// values together, otherwise the generator state can be learned after
			// reading 624 consecutive values.
	
			// Access to 32-bit random numbers
			double rand         ();              // real number in [0,1]
			double rand         (double n);      // real number in [0,n]
			uint32 randInt      ();              // integer in [0,2^32-1]
			uint32 randInt      (uint32 n);      // integer in [0,n] for n < 2^32
			double operator()   () { return this->rand(); }  // same as rand()
	
			// Access to 53-bit random numbers (capacity of IEEE double precision)
			double rand53       ();              // real number in [0,1)
	
			// Re-seeding functions with same behavior as initializers
			void seed           ();
			void seed           (uint32);


		protected:
			void            initialize (uint32 oneSeed);
			void            reload     ();
			uint32          hiBit      (uint32 u) const { return u & 0x80000000UL; }
			uint32          loBit      (uint32 u) const { return u & 0x00000001UL; }
			uint32          loBits     (uint32 u) const { return u & 0x7fffffffUL; }
			uint32          mixBits    (uint32 u, uint32 v) const { return hiBit(u) | loBits(v); }
			uint32          twist      (uint32 m, uint32 s0, uint32 s1) const { return m ^ (mixBits(s0,s1)>>1) ^ (negate(loBit(s1)) & 0x9908b0dfUL); }
			static uint32   hash       (time_t t, clock_t c);
			uint32          flipHiBit  (uint32 u) const { return (hiBit(u ^ 0x80000000UL)) | loBits(u); }
			uint32          negate     (uint32 u) const { int n = -(const int&)u; return *(uint32*)&n; }
	};


	inline        MTRand::MTRand  ()                       { this->seed(); }
	inline        MTRand::MTRand  (const uint32& oneSeed)  { this->seed(oneSeed); }
	inline double MTRand::rand    ()                       { return double(randInt()) * (1.0/4294967295.0); }
	inline double MTRand::rand    (double n)               { return this->rand() * n; }

	inline double MTRand::rand53()
	{
		uint32 a = randInt() >> 5, b = randInt() >> 6;
		return ( a * 67108864.0 + b ) * (1.0 / 9007199254740992.0);  // by Isaku Wada
	}


	inline uint32 MTRand::randInt()
	{
		// Pull a 32-bit integer from the generator state
		// Every other access function simply transforms the numbers extracted here
	
		if( left == 0 ) 
		{
			reload();
		}

		--left;
		
		uint32 s1 = *pNext++;

		s1 ^= (s1 >> 11);
		s1 ^= (s1 <<  7) & 0x9d2c5680UL;
		s1 ^= (s1 << 15) & 0xefc60000UL;
		
		return ( s1 ^ (s1 >> 18) );
	}


	inline uint32 MTRand::randInt(uint32 n)
	{
		// Find which bits are used in n
		// Optimized by Magnus Jonsson (magnus@smartelectronix.com)
		uint32 used = n;

		used |= used >> 1;
		used |= used >> 2;
		used |= used >> 4;
		used |= used >> 8;
		used |= used >> 16;
	
		// Draw numbers until one is found in [0,n]
		uint32 i;
		do
		{
			i = randInt() & used;  // toss unused bits to shorten search
		}
		while( i > n );

		return i;
	}


	inline void MTRand::seed(uint32 oneSeed)
	{
		// Seed the generator with a simple uint32
		this->initialize(oneSeed);
		this->reload();
	}


	inline void MTRand::seed()
	{
		// Seed the generator with an array from /dev/urandom if available
		// Otherwise use a hash of time() and clock() values
	
#if 0
		// First try getting an array from /dev/urandom
		FILE* urandom = fopen( "/dev/urandom", "rb" );
		if( urandom )
		{
			uint32 bigSeed[N];
			uint32 *s = bigSeed;
			int i = N;
			bool success = true;
			while( success && (0 != i--) )
				success = (0 != fread( s++, sizeof(uint32), 1, urandom ));
			fclose(urandom);
			if( success ) { this->seed( bigSeed, N );  return; }
		}
#endif	
		// Was not successful, so use time() and clock() instead
		this->seed(hash(time(NULL), ::clock()));
	}


	inline void MTRand::initialize(uint32 Seed)
	{
		// Initialize generator state with seed
		// See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
		// In previous versions, most significant bits (MSBs) of the seed affect
		// only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto.

		uint32* s = state;
		uint32* r = state;

		int i = 1;
		*s++ = Seed & 0xffffffffUL;

		for( ; i < N; ++i )
		{
			*s++ = ( 1812433253UL * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffUL;
			r++;
		}
	}


	inline void MTRand::reload()
	{
		// Generate N new values in state
		// Made clearer and faster by Matthew Bellew (matthew.bellew@home.com)

		uint32 *p = state;
		int i;

		for( i = N - M; i--; ++p )
			*p = twist( p[M], p[0], p[1] );

		for( i = M; --i; ++p )
			*p = twist( p[M-N], p[0], p[1] );

		*p = twist( p[M-N], p[0], state[0] );

		left = N, pNext = state;
	}


	inline uint32 MTRand::hash(time_t t, clock_t c)
	{
		// Get a uint32 from t and c
		// Better than uint32(x) in case x is floating point in [0,1]
		// Based on code by Lawrence Kirby (fred@genesis.demon.co.uk)

		static uint32 differ = 0;  // guarantee time-based seeds will change

		uint32 h1 = 0;

		uint8* p = (uint8*) &t;

		for(size_t i = 0; i < sizeof(t); ++i)
		{
			h1 *= UCHAR_MAX + 2U;
			h1 += p[i];
		}

		uint32 h2 = 0;
		p = (uint8*) &c;

		for(size_t j = 0; j < sizeof(c); ++j)
		{
			h2 *= UCHAR_MAX + 2U;
			h2 += p[j];
		}

		return ( h1 + differ++ ) ^ h2;
	}


	// Mimic UE's FMath RNG APIs.

	extern MTRand Rng;

	DAYLONGRAPHICSLIBRARY_API double FRand      ();
	DAYLONGRAPHICSLIBRARY_API double FRandRange (double Min, double Max);
	DAYLONGRAPHICSLIBRARY_API int32  RandRange  (int32 Min, int32 Max);
	DAYLONGRAPHICSLIBRARY_API bool   RandBool   ();

	inline float FRandRange(const FRange<float>& Range)
	{
		return (float)FRandRange(Range.Low(), Range.High());
	}

} // namespace Daylon


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
