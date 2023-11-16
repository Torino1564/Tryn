#include <TrynCppUnitTest.h>
#include <Core/src/ioc/Container.h>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace tryn;

struct Base
{
	virtual int Test() { return 420; }
	virtual ~Base() = default;
};

struct Derived : public Base
{
	int Test() override { return 69; }
};

namespace Ioc
{
	TEST_CLASS(IocContainerTests)
	{
	public:
		TEST_METHOD_INITIALIZE(Init)
		{
			pIoc = std::make_unique<ioc::Container>();
		}
		TEST_METHOD(SimpleResolve)
		{
			pIoc->Register<Base>([] {return std::make_shared<Derived>(); });
			Assert::AreEqual(69, pIoc->Resolve<Base>()->Test());
		}
		TEST_METHOD(SimpleResolveFailure)
		{
			Assert::ExpectException<std::exception>([this] {pIoc->Resolve<Base>(); });
		}
	private:
		std::unique_ptr<ioc::Container> pIoc;
	};
}