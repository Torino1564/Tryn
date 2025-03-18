#pragma once
#include <Core/src/gfx/Model/Mesh.h>
#include "Bone.h"
#include "Animation.h"

namespace tryn::gfx::ani
{
	class BonedMesh : public Mesh
	{
	public:
		BonedMesh(const IGraphics& gfx, const class Material& material, const aiMesh& mesh, std::string_view tag, ani::Skeleton& skeleton, glm::vec3 scale = { 1.0f,1.0f,1.0f }, std::optional<std::uint16_t> meshID = std::nullopt);
		[[nodiscard]] MeshType Type() const override;
		void Submit(const IGraphics& gfx, const glm::mat4 finalTransform, std::span<const glm::mat4> boneTransforms);
		void AddAnimation(std::shared_ptr<ani::Animation> pAnimation, const std::string& name);
		AnimationSkeletonInterface* GetAnimationInterface(const std::string& name = "?");
	private:
		void inline ExtraSubmitBehavior() override;
		Skeleton& skeleton;
		std::shared_ptr<IVtxConstantBuffer> pSkeletonCBuffer;
		std::unordered_map<std::string, uint16_t> animationNameMapper;
		std::vector<std::shared_ptr<ani::Animation>> pAnimations;
		std::vector<AnimationSkeletonInterface> interfaces;
		std::span<const glm::mat4> tempBoneTransforms;
	};
}
