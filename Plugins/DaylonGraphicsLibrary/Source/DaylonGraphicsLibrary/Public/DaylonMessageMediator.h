// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#pragma once

#include "CoreMinimal.h"


namespace Daylon
{
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

		For efficiency during message sending, the mediator maintains an array of
		consumers for each message ID, finding the array using a TMap. An even faster 
		mediator called TFastMediator uses a TArray. Would be nice to consolidate 
		the two classes by abstracting the consumer storage.
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
		// Default message mediator class.
		// Uses TMap to correlate messages to consumer arrays, which is slower
		// than an array or even switch(), but allows enum values to be
		// outside the enum ordinal count.

		// No copying allowed.
		TMessageMediator (const TMessageMediator&) = delete;
		TMessageMediator& operator= (const TMessageMediator&) = delete;


		public:

			TMessageMediator() {}


			void Send(Tenum Message, void* Payload = nullptr) const
			{
				check(Message != Tenum::Unknown);

				const auto Consumers = ConsumerArrays.Find(Message);

				if(Consumers != nullptr)
				{
					for(const auto& Consumer : *Consumers)
					{
						Consumer.MessageDelegate(Payload);
					}
				}
			}


			void RegisterConsumer(const FMessageConsumer<Tenum>& Consumer)
			{
				check(Consumer.IsValid());

				auto Consumers = ConsumerArrays.Find(Consumer.Message);

				if(Consumers == nullptr)
				{
					// No consumers exist for the message, so start a consumers array for it.
					TArray<FMessageConsumer<Tenum>> Array;
					Array.Add(Consumer);
					ConsumerArrays.Add(Consumer.Message, Array);
				}
				else if(nullptr == Consumers->FindByPredicate([Consumer](const FMessageConsumer<Tenum>& Elem){ return (Elem.Object == Consumer.Object); }))
				{
					// Consumers exist for the message but not for the consumer's object, so add consumer to existing array.
					Consumers->Add(Consumer);
				}
			}


			void UnregisterConsumer(void* Object)
			{
				// Remove all delegates which a recipient object had registered.

				// Called when object no longer needs to receive messages, or
				// when the object is being deleted.

				for(auto& Pair : ConsumerArrays)
				{
					auto& Consumers = Pair.Value;

					for(int32 Idx = Consumers.Num() - 1; Idx >= 0; Idx--)
					{
						if(Consumers[Idx].Object == Object)
						{
							Consumers.RemoveAtSwap(Idx);
						}
					}
				}

				// We don't expect to need unregistering of a particular delegate i.e. Object + Message.
			}


		protected:

			TMap<Tenum, TArray<FMessageConsumer<Tenum>>> ConsumerArrays;
	};


	template <typename Tenum> class TFastMessageMediator
	{
		// Fast version of TMessageMediator that requires enum type argument
		// to have a Count value, and also that all other values range from 0 to Count - 1.

		// No copying allowed.
		TFastMessageMediator (const TFastMessageMediator&) = delete;
		TFastMessageMediator& operator= (const TFastMessageMediator&) = delete;


		public:

			TFastMessageMediator() 
			{
				ConsumerArrays.SetNum((int32)Tenum::Count);
			}


			void Send(Tenum Message, void* Payload = nullptr) const
			{
				check(Message != Tenum::Unknown);
				check(ConsumerArrays.IsValidIndex((int32)Message));

				for(const auto& Consumer : ConsumerArrays[(int32)Message])
				{
					Consumer.MessageDelegate(Payload);
				}
			}


			void RegisterConsumer(const FMessageConsumer<Tenum>& Consumer)
			{
				check(Consumer.IsValid());
				check(ConsumerArrays.IsValidIndex((int32)Consumer.Message));

				auto& Consumers = ConsumerArrays[(int32)Consumer.Message];

				if(nullptr == Consumers.FindByPredicate([Consumer](const FMessageConsumer<Tenum>& Elem){ return (Elem.Object == Consumer.Object); }))
				{
					// Consumers exist for the message but not for the consumer's object, so add consumer to existing array.
					Consumers.Add(Consumer);
				}
			}


			void UnregisterConsumer(void* Object)
			{
				// Remove all delegates which a recipient object had registered.

				// Called when object no longer needs to receive messages, or
				// when the object is being deleted.

				for(auto& Consumers : ConsumerArrays)
				{
					for(int32 Idx = Consumers.Num() - 1; Idx >= 0; Idx--)
					{
						if(Consumers[Idx].Object == Object)
						{
							Consumers.RemoveAtSwap(Idx);
						}
					}
				}

				// We don't expect to need unregistering of a particular delegate i.e. Object + Message.
			}


		protected:

			// Instead of a TMap, we use a TArray to correlate messages directly with consumer arrays.
			
			TArray<TArray<FMessageConsumer<Tenum>>> ConsumerArrays;
	};


	template <typename Tval, typename Tmediator, typename Tenum> class TMessageableValue
	{
		// Use this template instead of a normal value type in order to 
		// make it sendable to a message mediator.
		// If Tval is non-scalar, it must provide assignment operator and operator != .

		protected:

			Tval        Value;
			Tmediator*  MessageMediator = nullptr;
			Tenum       Message         = Tenum::Unknown;
			bool        FirstTime       = true;


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


			void Bind(Tmediator* InMessageMediator, Tenum InMessage)
			{
				check(InMessageMediator != nullptr);
				check(InMessage != Tenum::Unknown);

				MessageMediator = InMessageMediator;
				Message         = InMessage;
			}
	};
}
