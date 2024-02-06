#include "Assert.h"
#include <Core/src/log/Log.h>

namespace tryn::utl
{
	namespace {
#ifdef NDEBUG
		constexpr int skip_depth = 3;
		constexpr int skip_depth_ex = 23;
#else
		constexpr int skip_depth = 7;
		constexpr int skip_depth_ex = 29;
#endif
	}

	Assertion::Assertion(std::wstring expression, const wchar_t* file, const wchar_t* function, int line, Consequence consequence, std::wstring msg)
		:
		file_{ file },
		function_{ function },
		line_{ line },
		consequence_{ consequence },
		skip_depth_{ skip_depth }
	{
		stream_ << L"Assertion Failed! " << expression << "\n" << msg << "\n";
	}
	Assertion::~Assertion()
	{
		log::EntryBuilder{ file_, function_, line_ }
		.trace_skip(skip_depth_)
			.chan(log::GetDefaultChannel())
			.level(consequence_ == Consequence::Terminate ? log::Level::Fatal : level)
			.note(stream_.str());
		if (consequence_ == Consequence::Terminate) {
			log::GetDefaultChannel()->Flush();
			std::terminate();
		}
	}
	Assertion& Assertion::msg(const std::wstring& message)
	{
		stream_ << L"  Msg: " << message << L"\n";
		return *this;
	}
	Assertion& Assertion::lvl(const log::Level level)
	{
		this->level = level;
		return *this;
	}
	void Assertion::ex()
	{
		consequence_ = Consequence::Exception;
		skip_depth_ = skip_depth_ex;
		throw FailedAssertion{};
	}
}