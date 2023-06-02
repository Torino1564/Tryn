#include <TrynCppUnitTest.h>
#include <Core/src/log/Channel.h>
#include <Core/src/log/EntryBuilder.h>
#include <Core/src/log/Driver.h>
#include <Core/src/log/Policy.h>
#include <Core/src/log/SeverityLevelPolicy.h>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace tryn;
using namespace std::string_literals;

#define trylog log::EntryBuilder{ __FILEW__ , __FUNCTIONW__, __LINE__ }

class MockDriver : public log::IDriver
{
public:
	void Submit(const log::Entry& e) override
	{
		entry_ = e;
	}
	log::Entry entry_;
	void Flush() override {};
};

class TestChannel : public log::Channel
{
public:
	void Submit(log::Entry& e) override
	{
		entry_ = e;
	}
	log::Entry entry_;
};

namespace Log
{
	TEST_CLASS(LogEntryTests)
	{
	public:
		TEST_METHOD(ShowOffFluent)
		{
			TestChannel chan;
			trylog.info(L"HI").chan(&chan);
			Assert::AreEqual(L"HI"s, chan.entry_.note_);
			Assert::IsTrue(log::Level::Info == chan.entry_.level_);
		}
	};
	TEST_CLASS(LogChannelTest)
	{
	public:
		TEST_METHOD(TestForwarding)
		{
			log::Channel chan;
			auto pDriver1 = std::make_shared<MockDriver>();
			auto pDriver2 = std::make_shared<MockDriver>();
			chan.AttachDriver(pDriver1);
			chan.AttachDriver(pDriver2);
			trylog.info(L"HI").chan(&chan);
			Assert::IsTrue(L"HI" == pDriver1->entry_.note_);
			Assert::AreEqual((int)log::Level::Info, (int)pDriver1->entry_.level_);
			Assert::IsTrue(L"HI" == pDriver2->entry_.note_);
			Assert::AreEqual((int)log::Level::Info, (int)pDriver2->entry_.level_);
		}
		TEST_METHOD(TestChannelPolicyFiltering)
		{
			log::Channel chan;
			auto pDriver1 = std::make_shared<MockDriver>();
			auto pPolicy = std::make_unique<log::SeverityLevelPolicy>(log::Level::Info);
			chan.AttachDriver(pDriver1);
			chan.AttachPolicy(std::move(pPolicy));
			trylog.info(L"HI").chan(&chan);
			Assert::AreEqual(L"HI"s, pDriver1->entry_.note_);
			Assert::AreEqual((int)log::Level::Info, (int)pDriver1->entry_.level_);
			trylog.debug(L"Goodbye"s).chan(&chan);
			Assert::AreEqual(L"HI"s, pDriver1->entry_.note_);
			Assert::AreEqual((int)log::Level::Info, (int)pDriver1->entry_.level_);
		}
	};
	
}