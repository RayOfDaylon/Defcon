// Copyright 2023 Daylon Graphics Ltd. All rights reserved.

#pragma once

#include "CoreMinimal.h"


namespace Daylon
{
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

	// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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
}
