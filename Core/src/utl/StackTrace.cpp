#include "StackTrace.h"
#include <Core/third/backward.hpp>
#include <sstream>

tryn::utl::StackTrace::StackTrace()
{
	backward::TraceResolver thisIsAWorkaround; // Search the internet for an issue with the backward library
	pTrace = std::make_unique<backward::StackTrace>();
	pTrace->load_here(64);
}

tryn::utl::StackTrace::StackTrace(const StackTrace& src)
	:
	pTrace{std::make_unique<backward::StackTrace>(*pTrace) }
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


}
