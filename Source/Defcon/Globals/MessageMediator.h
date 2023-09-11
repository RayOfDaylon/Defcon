// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/*
	MessageMediator.h

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


namespace Daylon
{
	// Message delegates have a simple API: a single void* argument, void return type.

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


	template <typename Tenum> class CMessageMediator
	{
		// No copying allowed.
		CMessageMediator (const CMessageMediator&) = delete;
		CMessageMediator& operator= (const CMessageMediator&) = delete;


		public:

			CMessageMediator() {}

			void Send(Tenum Message, void* Payload = nullptr)
			{
				for(const auto& Consumer : Consumers)
				{
					if(Consumer.Message == Message)
					{
						Consumer.MessageDelegate(Payload);
					}
				}

				/* We could also use Object and Message together as a unique key in a TMap, 
				to quicken search time, although we'd need 64 + 8 bits. But a linear search
				is quick given that we will not have many recipients (?). Or we might 
				given all the humans.*/
				/*
				   We could also store a TMap of recipients with just Object as a key,
				   and the value would be an TArray or TMap of consumers. So we do a 
				   map<object> lookup to find the object's consumers, then do another 
				   lookup to find the particular consumer.
				*/ 
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


	template <typename Tval, typename Tenum> class TBroadcastableValue
	{
		protected:

			Tval                     Value;
			CMessageMediator<Tenum>* MessageMediator = nullptr;
			Tenum                    Message         = Tenum::Unknown;
			bool                     FirstTime       = true;

		public:

			Tval Get() const { return Value; }

			
			void Set(Tval Val) 
			{
				check(MessageMediator != nullptr);
				check(Message != Tenum::Unknown);

				const bool Different = (Value != Val);

				Value = Val; 

				if(Different || FirstTime)
				{
					MessageMediator->Send(Message, &Value);
					FirstTime = false;
				}
			}


			void Bind(CMessageMediator<Tenum>* InMessageMediator, Tenum InMessage)
			{
				check(InMessageMediator != nullptr);
				check(InMessage != Tenum::Unknown);

				MessageMediator = InMessageMediator;
				Message         = InMessage;
			}
	};
}


namespace Defcon
{
	// Defcon-specific message mediator.

	struct FShieldStrengthInfo 
	{
		class ILiveGameObject* Object; 
		float Value; 

		bool operator == (const FShieldStrengthInfo& Rhs) const { return (Object == Rhs.Object && Value == Rhs.Value); }
		bool operator != (const FShieldStrengthInfo& Rhs) const { return !(*this == Rhs); }
	};


	enum class EMessageEx
	{
		// Message                Payload type

		Unknown = 0,            // nullptr
		AbductionCountChanged,  // TArray<bool>*
		SmartbombCountChanged,  // int32*
		ShieldStrengthChanged   // FShieldStrengthInfo*
	};


	typedef Daylon::FMessageConsumer<EMessageEx> FMessageConsumer;


	class CMessageMediator : public Daylon::CMessageMediator<EMessageEx> 
	{
		CMessageMediator (const CMessageMediator&) = delete;
		CMessageMediator& operator= (const CMessageMediator&) = delete;

		public:
			
			CMessageMediator() {}
	};


	extern CMessageMediator GMessageMediator;


	template <typename Tval> class TBroadcastableValue : public Daylon::TBroadcastableValue<Tval, EMessageEx>
	{
		public:

			void Bind(EMessageEx InMessage)
			{
				Daylon::TBroadcastableValue<Tval, EMessageEx>::Bind(&GMessageMediator, InMessage);
			}
	};
}
