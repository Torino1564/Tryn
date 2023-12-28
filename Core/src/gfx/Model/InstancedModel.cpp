#include "InstancedModel.h"

namespace tryn::gfx
{
	InstancedModelParent::InstancedModelParent(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale, Techniques defaultTechnique, std::optional<std::uint32_t> numInstances)
		:
		Model(gfx, path, scale, defaultTechnique)
	{
		instancedGroup = "InstaceGroup";
		instancedGroup += path.data();

		numInstanced = numInstances.value_or(0);

		ConstantBufferLayout layout;
	}
	void InstancedModelChild::Submit(const glm::mat4& transformation)
	{

	}
}
