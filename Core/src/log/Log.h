#pragma once
#include "Channel.h"
#include "EntryBuilder.h"

namespace tryn::log
{
	IChannel* GetDefaultChannel();

	void Boot();

#ifdef NDEBUG
	inline constexpr int defaultTraceSkip = 2;
#else 
	inline constexpr int defaultTraceSkip = 6;
#endif
}

#define trylog log::EntryBuilder{ __FILEW__ , __FUNCTIONW__ , __LINE__ }.chan(log::GetDefaultChannel()).trace_skip(log::defaultTraceSkip)