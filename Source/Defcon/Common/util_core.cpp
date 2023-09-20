// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

/*
	util_core.cpp
*/

#include "util_core.h"

#include "DefconUtils.h"




#define DEBUG_MODULE      0

#if(DEBUG_MODULE == 1)
#pragma optimize("", off)
#endif



Daylon::CScheduledTaskList::~CScheduledTaskList()
{
	DeleteAll();
}


void Daylon::CScheduledTaskList::Add(IScheduledTask* Task)
{
	check(Task != nullptr);

	Tasks.Add(Task);
}


void Daylon::CScheduledTaskList::Process(float DeltaTime)
{
	for(int32 Index = Tasks.Num() - 1; Index >= 0; Index--)
	{
		auto Task = Tasks[Index];

		Task->Countdown -= DeltaTime;

		if(Task->Countdown <= 0.0f)
		{
			Task->Do();

			// If the task was major then the task list may have emptied, so check for that.
			if(Tasks.IsEmpty())
			{
				return;
			}

			delete Task;

			Tasks.RemoveAtSwap(Index);
		}
	}
}


void Daylon::CScheduledTaskList::DeleteAll()
{
	for(auto Task : Tasks)
	{
		check(Task != nullptr);
		
		delete Task;
	}

	Tasks.Empty();
}


void Daylon::CScheduledTaskList::ForEach(TFunction<void(IScheduledTask*)> Function) const
{
	check(Function);

	for(auto Task : Tasks)
	{
		check(Task != nullptr);

		Function(Task);
	}
}


void Daylon::CScheduledTaskList::ForEachUntil(TFunction<bool(IScheduledTask*)> Function) const
{
	check(Function);

	for(auto Task : Tasks)
	{
		check(Task != nullptr);

		if(!Function(Task))
		{
			break;
		}
	}
}


#if(DEBUG_MODULE == 1)
#pragma optimize("", on)
#endif

#undef DEBUG_MODULE
