#include "TrynPCH.h"
#include "Channel.h"
#include "Driver.h"
#include "Policy.h"

tryn::log::Channel::Channel(std::vector<std::shared_ptr<IDriver>> driverPtrs)
	:
	driverPtrs_{ std::move(driverPtrs) }
{
}

tryn::log::Channel::~Channel()
{}

void tryn::log::Channel::Submit(Entry& e)
{
	for (auto& pPolicy : policyPtrs_)
	{
		if (!pPolicy->TransformFilter(e))
		{
			return;
		}
	}
	for (auto& pDriver : driverPtrs_)
	{
		pDriver->Submit(e);
	}
}

void tryn::log::Channel::Flush()
{
	for (auto& pDriver : driverPtrs_) {
		pDriver->Flush();
	}
}

void tryn::log::Channel::AttachDriver(std::shared_ptr<IDriver> pDriver)
{
	driverPtrs_.push_back(std::move(pDriver));
}

void tryn::log::Channel::AttachPolicy(std::shared_ptr<IPolicy> pPolicy)
{
	policyPtrs_.push_back(std::move(pPolicy));
}