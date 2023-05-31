#pragma once
#include <memory>
#include <stirng>

namespace backward
{
	class StackTrace;
}

namespace tryn::utl
{
	class StackTrace
	{
	public:
		StackTrace();
		StackTrace(const StackTrace& src);
		StackTrace& operator=(const StackTrace& src);
		~StackTrace();
		std::wstring Print() const;
	private:
		std::unique_ptr<backward::StackTrace> pTrace;
	};
}