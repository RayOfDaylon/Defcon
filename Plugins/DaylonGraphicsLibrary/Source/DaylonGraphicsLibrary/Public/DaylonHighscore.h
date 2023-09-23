// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#pragma once

#include "CoreMinimal.h"


namespace Daylon
{
	struct DAYLONGRAPHICSLIBRARY_API FHighScore
	{
		// A single entry in a high score table.
		
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

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template<int32 MaxEntries, int32 MaxNameLength> struct THighScoreTable
	{
		// A simple high score table.
		// MaxNameLength is the max. number of characters a name can have. Original Asteroids game used 3 chars.

		TArray<FHighScore> Entries;

		void   Clear() 
		{
			Entries.Empty(); 
		}


		int32 GetMaxNameLength() const { return MaxNameLength; }


		int32 GetLongestNameLength() const
		{
			int32 Len = 0;

			for(const auto& Entry : Entries)
			{
				Len = FMath::Max(Len, Entry.Name.Len());
			}
			return Len;
		}


		bool CanAdd(int32 Score) const 
		{
			if(Entries.Num() < MaxEntries)
			{
				return true;
			}

			// We're full, but see if the score beats an existing entry.

			for(const auto& Entry : Entries)
			{
				if(Score > Entry.Score)
				{
					return true;
				}
			}

			return false;
		}


		void Add(int32 Score, const FString& Name)
		{
			if(!CanAdd(Score))
			{
				return;
			}

			Entries.Add(FHighScore(Score, Name));

			Entries.Sort();
			Algo::Reverse(Entries);

			// Cull any entries past the max allowable.
			while(Entries.Num() > MaxEntries)
			{
				Entries.Pop();
			}
		}
	};
}
