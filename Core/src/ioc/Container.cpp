#include "Container.h"

namespace tryn::ioc
{
	Container& Get() noexcept
	{
		static Container container;
		return container;
	}
}