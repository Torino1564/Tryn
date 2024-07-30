#include "TrynPCH.h"
#include "StackTrace.h"
#include <sstream>
#include <Core/src/utl/String.h>

#pragma warning (push)
#pragma warning (disable : 26495 26439 26451 5105)
#include <Core/third/backward.hpp>
#pragma warning (pop)

tryn::utl::StackTrace::StackTrace(int depth)
{
	backward::TraceResolver thisIsAWorkaround; // Search the internet for an issue with the backward library
	pTrace = std::make_unique<backward::StackTrace>();
	pTrace->load_here(64);
	pTrace->skip_n_firsts(depth);
}

tryn::utl::StackTrace::StackTrace(const StackTrace& src)
	:
	pTrace{ std::make_unique<backward::StackTrace>(*pTrace) }
{
}

tryn::utl::StackTrace& tryn::utl::StackTrace::operator=(const StackTrace& src)
{
	pTrace = std::make_unique<backward::StackTrace>(*pTrace);
	return *this;
}

tryn::utl::StackTrace::~StackTrace()
{
}

std::wstring tryn::utl::StackTrace::Print() const
{
	std::ostringstream oss;
	backward::Printer printer;
	printer.print(*pTrace, oss);
	return utl::ToWide(oss.str());
}