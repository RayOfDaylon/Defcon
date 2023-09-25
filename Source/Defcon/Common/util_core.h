// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#pragma once

/* 
	util_core.h
	Core General-purpose utility header file.
*/

#include "CoreMinimal.h"


//float GameTime();

#ifdef __cplusplus
extern "C"
{
#endif

// Useful macros.

#define DELETE_AND_NULL(_p)  { delete (_p); (_p) = nullptr; }
#define SAFE_DELETE(_p)      { if((_p) != nullptr) { DELETE_AND_NULL(_p); }}


#define array_size(_a)	(sizeof(_a) / sizeof((_a)[0]))
//#define STRLOOKUP(n, s)	(strlookup((int32)(n), (s), array_size(s)))
#define ZEROFILL(obj)	memset(&obj, 0, sizeof(obj))

//char* strlookup(int32 n, char** ppsz, int32 tablelen);


// Some DLLs just can't link to MSVCRT's stricmp(), 
// so if this macro is defined, use our own version.
#ifdef __USE_LOCAL_STRICMP
	int our_stricmp(const char*, const char*);
#else
	#define our_stricmp(s1, s2)  _stricmp(s1, s2)
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

namespace Daylon
{
	// Function that returns true if a bitwise enum value has a given flag.
	// todo: move to plugin.

	template<typename E> bool HasFlag(E Flags, E Which)
	{
		return(0 != ((int32)Flags & (int32)Which));
	}


	// A candidate for the plugin, but it'd be also nice to consolidate with FScheduledTask.
	// IScheduledTask is when you prefer to subclass and the delegate is a virtual override.

	class IScheduledTask
	{
		public:
			IScheduledTask() {}
			virtual ~IScheduledTask() {}

			float   Countdown = 0.0f; // If this is not set, task will run immediately.

			virtual void Do() = 0;
	};


	class CScheduledTaskList
	{
		// Tasks to be run at some time in the future.

		public:
			virtual ~CScheduledTaskList();

			void  Add           (IScheduledTask*);
			void  Process       (float DeltaTime);
			void  DeleteAll     ();
			bool  IsEmpty       () const { return Tasks.IsEmpty(); }
			void  ForEach       (TFunction<void(IScheduledTask*)> Function) const;
			void  ForEachUntil  (TFunction<bool(IScheduledTask*)> Function) const;

		private:
			TArray<IScheduledTask*>  Tasks;
	};
}
