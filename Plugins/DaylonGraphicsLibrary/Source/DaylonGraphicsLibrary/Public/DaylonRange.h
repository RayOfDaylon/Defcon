// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#pragma once

#include "CoreMinimal.h"


namespace Daylon
{
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
}
