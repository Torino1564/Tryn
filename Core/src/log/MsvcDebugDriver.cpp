#include "MsvcDebugDriver.h"
// TODO replace with custom windos header
#include <Windows.h>
#include "TextFormatter.h"

tryn::log::MsvcDebugDriver::MsvcDebugDriver(std::shared_ptr<ITextFormatter> pFormatter)
	:
	pFormatter_{std::move(pFormatter)}
{
}

void tryn::log::MsvcDebugDriver::Submit(const Entry& e)
{
	if (pFormatter_)
	{
		OutputDebugStringW(pFormatter_->Format(e).c_str());
	}
	// TODO: log warning or error on a logger backup logger
}

void tryn::log::MsvcDebugDriver::SetFormatter(std::shared_ptr<ITextFormatter> pFormatter)
{
	pFormatter_ = std::move(pFormatter);
}

void tryn::log::MsvcDebugDriver::Flush()
{}
