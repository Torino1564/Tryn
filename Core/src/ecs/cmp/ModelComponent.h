#pragma once
#include "ComponentManager.h"

namespace tryn::gfx
{
	class Model;
}

namespace tryn::ecs::cmp
{
	ZT_DEFINE_COMPONENT(ModelComponent)
	{
		public: struct SubresourceData
		{
			std::unique_ptr<gfx::Model> pModel;
			using pModel_t = tryn::utl::CTM::Map_t<std::unique_ptr<gfx::Model>, "std::unique_ptr<gfx::Model>", "pModel", sizeof(std::unique_ptr<gfx::Model>), UUID>;
			~SubresourceData();
			SubresourceData();
		};
		const static inline SubresourceData srd = {};
		static const unsigned int index;
		static const std::vector<tryn::utl::CTM::ElementData>& GetReflectData_();;
	};
}