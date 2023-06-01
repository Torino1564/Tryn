#pragma once
#include <memory>
#include <string>

namespace backward
{
	class StackTrace;
}

namespace tryn::utl
{
	class StackTrace
	{
	public:
		StackTrace(int depth = 6);
		StackTrace(const StackTrace& src);
		StackTrace& operator=(const StackTrace& src);
		~StackTrace();
		std::wstring Print() const;
	private:
		std::unique_ptr<backward::StackTrace> pTrace;
	};
}