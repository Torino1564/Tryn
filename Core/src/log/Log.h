#pragma once
#include "Channel.h"
#include "EntryBuilder.h"

namespace tryn::log
{
	IChannel* GetDefaultChannel();

	void Boot();
}

#define trylog log::EntryBuilder{ __FILEW__ , __FUNCTIONW__ , __LINE__ }.chan(log::GetDefaultChannel())