#include <TrynCppUnitTest.h>

namespace tryn::ecs
{
	ZT_DEFINE_COMPONENT(TestComponent1)
	{
		ZT_COMPONENT_FIELDS(int testField = 0;);
	};

	template <typename T, ValidComponent C>
	struct LogMemberVariable
	{
		void operator()()
		{
			typename T::TypeName_t typeNameFunc;
			typename T::VarName_t varNameFunc;
			typename T::VarSize_t varSizeFunc;
			typename T::ElementNumber_t elNumberFunc;
			typename T::ByteOffset_t byteOffsetFunc;
			trylog.info(utl::ToWide(std::format("Element Number: {}\n Type Name: {}\n Var Name: {}\n Var Size: {}\n Byte Offset: {}\n\n", elNumberFunc(), typeNameFunc(), varNameFunc(), varSizeFunc(), byteOffsetFunc())));
		}
	};

	TEST_CLASS(EntitySystem)
	{
	public:
		TEST_METHOD_INITIALIZE(Init)
		{
			app::BootCore();
			pWnd = std::make_unique<win::Window>(std::make_shared<win::WindowClass>(), L"Test window", spa::DimensionsI(300, 300));
			pGfx = std::make_unique<gfx::dx11::Graphics>(pWnd->GetHandle(), pWnd->GetClientDimensions().width, pWnd->GetClientDimensions().height);
		}
		TEST_METHOD(StatefullBS)
		{


		}
	private:
		std::unique_ptr<gfx::dx11::Graphics> pGfx;
		std::unique_ptr<win::Window> pWnd;
		std::vector<std::unique_ptr<Entity>> entityPtrs;
	};
}