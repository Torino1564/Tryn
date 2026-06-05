
#include "EntryBuilder.h"
#include "Channel.h"
#include <Core/src/win/TrynWin.h>
#include <Core/src/utl/String.h>

#pragma warning(push)
#pragma warning(disable: 26815)
namespace tryn::log
{
	EntryBuilder::EntryBuilder(const wchar_t* sourceFile, const wchar_t* sourceFunctionName, int sourceLine)
		:
		Entry{
			.level_ = Level::Error,
			.sourceFile_ = sourceFile,
			.sourceFunctionName_ = sourceFunctionName,
			.sourceLine_ = sourceLine,
			.timestamp_ = std::chrono::system_clock::now(),
		}
	{}

	tryn::log::EntryBuilder::EntryBuilder(const char* sourceFile, const char* sourceFunctionName, int sourceLine)
		:
		Entry{
			.level_ = Level::Error,
			.sourceFile_ = utl::ToWide(sourceFile).data(),
			.sourceFunctionName_ = utl::ToWide(sourceFunctionName).data(),
			.sourceLine_ = sourceLine,
			.timestamp_ = std::chrono::system_clock::now(),
		}
	{}

	EntryBuilder& tryn::log::EntryBuilder::note(std::wstring note)
	{
		note_ = std::move(note);
		return *this;
	}

	EntryBuilder& tryn::log::EntryBuilder::level(Level lv)
	{
		level_ = lv;
		return *this;
	}
	EntryBuilder& EntryBuilder::verbose(std::wstring note)
	{
		note_ = std::move(note);
		level_ = Level::Verbose;
		return *this;
	}
	EntryBuilder& EntryBuilder::debug(std::wstring note)
	{
		note_ = std::move(note);
		level_ = Level::Debug;
		return *this;
	}
	EntryBuilder& EntryBuilder::info(std::wstring note)
	{
		note_ = std::move(note);
		level_ = Level::Info;
		return *this;
	}
	EntryBuilder& EntryBuilder::warn(std::wstring note)
	{
		note_ = std::move(note);
		level_ = Level::Warn;
		return *this;
	}
	EntryBuilder& EntryBuilder::error(std::wstring note)
	{
		note_ = std::move(note);
		level_ = Level::Error;
		return *this;
	}
	EntryBuilder& EntryBuilder::fatal(std::wstring note)
	{
		note_ = std::move(note);
		level_ = Level::Fatal;
		return *this;
	}
	EntryBuilder& tryn::log::EntryBuilder::chan(IChannel* pChan)
	{
		pDest_ = pChan;
		return *this;
	}
	EntryBuilder& EntryBuilder::trace_skip(int depth)
	{
		traceSkipDepth_ = depth;
		return *this;
	}
	EntryBuilder& EntryBuilder::no_trace()
	{
		captureTrace_ = false;
		return *this;
	}
	EntryBuilder& EntryBuilder::trace()
	{
		captureTrace_ = true;
		return *this;
	}
	EntryBuilder& EntryBuilder::no_line()
	{
		showSourceLine_ = false;
		return *this;
	}
	EntryBuilder& EntryBuilder::line()
	{
		showSourceLine_ = true;
		return *this;
	}
	EntryBuilder& EntryBuilder::hr()
	{
		hResult_ = GetLastError();
		return *this;
	}
	EntryBuilder& EntryBuilder::hr(unsigned int hr)
	{
		hResult_ = hr;
		return *this;
	}
	EntryBuilder::~EntryBuilder()
	{
		if (pDest_ != nullptr)
		{
			if (captureTrace_.value_or((int)level_ <= (int)Level::Error))
			{
				trace_.emplace(traceSkipDepth_);
			}
			pDest_->Submit(*this);
		}
	}
}

#pragma warning(pop)