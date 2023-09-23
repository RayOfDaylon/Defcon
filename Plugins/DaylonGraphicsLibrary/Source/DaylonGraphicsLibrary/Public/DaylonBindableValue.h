// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#pragma once

#include "CoreMinimal.h"


namespace Daylon
{
	template <typename T> class TBindableValue
	{
		// A type that acts like a given type except that when the value is modified,
		// it invokes a delegate. While value binding is more efficient than using a
		// mediator, it's more tightly coupled because the consumer has to reference
		// the value's variable. See TMessageableValue for a bindable value that
		// uses a mediator.

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
}
