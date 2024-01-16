#pragma once
#include <Core/src/gfx/Model/Mesh.h>
#include <Core/src/gfx/Material.h>
#include "Bone.h"

namespace tryn::gfx::ani
{
	class BonedMesh : public Mesh
	{
	public:
		BonedMesh(IGraphics& gfx, const Material& material, const aiMesh& mesh, std::string_view tag, ani::Skeleton& skeleton, glm::vec3 scale = { 1.0f,1.0f,1.0f }, std::optional<std::uint16_t> meshID = std::nullopt);
		bool IsStatic() const override;
	private:
		void inline ExtraSubmitBehavior() override;
		Skeleton& skeleton;
		std::shared_ptr<IVtxConstantBuffer> pSkeletonCBuffer;
	};
}
