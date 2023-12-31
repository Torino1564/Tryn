#include "InstancedModel.h"

namespace tryn::gfx
{
	InstancedModelParent::InstancedModelParent(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale, Techniques defaultTechnique, std::optional<std::uint32_t> numInstances)
		:
		Model(gfx, path, scale, defaultTechnique, true)
	{
		instancedGroup = "InstaceGroup";
		instancedGroup += path.data();

		numInstanced = numInstances.value_or(0);

		ConstantBufferLayout::Node arrayElement(ConstantBufferLayout::Type::Struct, "arrayStruct");
		arrayElement["arrayStruct"].Append(ConstantBufferLayout::Type::Matrix4, "transform");

		pTransformationBuffer = gfx.CreateInstanceBuffer(arrayElement, numInstanced);
	}
	void InstancedModelParent::Submit(const glm::mat4& transformation)
	{
		Model::Submit(transformation);

	}
}
