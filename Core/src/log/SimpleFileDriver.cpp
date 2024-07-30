#include "TrynPCH.h"
#include "SimpleFileDriver.h"
#include "TextFormatter.h"

namespace tryn::log
{
	SimpleFileDriver::SimpleFileDriver(std::filesystem::path path, std::shared_ptr<ITextFormatter> pFormatter)
		:
		pFormatter_{ std::move(pFormatter) }
	{
		// create dirs
		std::filesystem::create_directories(path.parent_path());
		// open file, append if exists
		file_.open(path, file_.out | file_.app);
	}
	void SimpleFileDriver::Submit(const Entry& e)
	{
		if (pFormatter_)
		{
			file_ << pFormatter_->Format(e).c_str();
		}

		// TODO: How to log from the log system?
	}
	void SimpleFileDriver::SetFormatter(std::shared_ptr<ITextFormatter> pFormatter)
	{
		pFormatter_ = std::move(pFormatter);
	}
	void SimpleFileDriver::Flush()
	{
		file_.flush();
	}
}