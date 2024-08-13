#include "TrynPCH.h"
#include "BonedMesh.h"
#include <Core/src/gfx/Bindables/IBuffer.h>
#include <Core/src/mem/ArenaAllocator.h>
#include <Core/src/gfx/Bindables/JITUpdateBuffer.h>
#include <Core/src/gfx/Material.h>
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>

namespace tryn::gfx::ani
{
	glm::mat4 convertAiToGlm(const aiMatrix4x4& aiMat) {
		glm::mat4 glmMat;

		glmMat[0][0] = aiMat.a1; glmMat[0][1] = aiMat.b1; glmMat[0][2] = aiMat.c1; glmMat[0][3] = aiMat.d1;
		glmMat[1][0] = aiMat.a2; glmMat[1][1] = aiMat.b2; glmMat[1][2] = aiMat.c2; glmMat[1][3] = aiMat.d2;
		glmMat[2][0] = aiMat.a3; glmMat[2][1] = aiMat.b3; glmMat[2][2] = aiMat.c3; glmMat[2][3] = aiMat.d3;
		glmMat[3][0] = aiMat.a4; glmMat[3][1] = aiMat.b4; glmMat[3][2] = aiMat.c4; glmMat[3][3] = aiMat.d4;

		return glmMat;
	}

	BonedMesh::BonedMesh(const IGraphics& gfx, const Material& material, const aiMesh& mesh, std::string_view tag, ani::Skeleton& skeleton, glm::vec3 scale, std::optional<std::uint16_t> meshID)
		:
		skeleton(skeleton)
	{
		if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
		{
			this->tag = std::format("{}#Scale[X:{},Y:{},Z:{}]", tag, scale.x, scale.y, scale.z);
		}
		else
		{
			this->tag = tag;
		}

		if (!mesh.HasBones())
		{
			trylog.warn(L"BonedMesh created with a mesh with no bones!");
		}


		for (unsigned int i = 0; i < mesh.mNumBones; i++)
		{
			auto& bone = *mesh.mBones[i];
			if (bone.mNumWeights == 0)
				continue;
			// Find the bone in the bone array in the skeleton
			auto boneIt = std::find_if(skeleton.bones.begin(), skeleton.bones.end(), [&](const auto& boneInArray) {
				return strcmp(boneInArray.name.c_str(),bone.mName.C_Str()) ? false : true;
				});

			trynass_msg(boneIt != skeleton.bones.end(), L"Did not find a bone creatign a BonedMesh!");

			boneIt->inverseBP = convertAiToGlm(bone.mOffsetMatrix);
			
			boneIt->boneWeights.reserve(bone.mNumWeights);

			for (unsigned int i = 0; i < bone.mNumWeights; i++)
			{
				boneIt->boneWeights.push_back({ bone.mWeights[i].mVertexId, bone.mWeights[i].mWeight });
			}
		}

		ID = meshID.value_or(0);
		auto vertexBuffer = material.ExtractVertices(mesh, &skeleton);
		vertexBuffer.SetClean();
		const auto indices = material.ExtractIndices(mesh);

		indexCount = static_cast<uint32_t>(indices.Size());

		pVertexBuffer = IVertexBuffer::Resolve(gfx, std::make_shared<VertexBuffer>(vertexBuffer), this->tag);
		pIndexBuffer = IIndexBuffer::Resolve(gfx, std::make_shared<IndexBuffer>(indices));
		pTopology = IPrimitiveTopology::Resolve(gfx);
		InitTransformCBuf(gfx);

		ConstantBufferLayout cblayout;
		cblayout.Append(ConstantBufferLayout::Array, "boneArray");
		ConstantBufferLayout::Node arrayElement(ConstantBufferLayout::Matrix4, "boneTransform");
		cblayout["boneArray"].Set(std::move(arrayElement), skeleton.bones.size());
		cblayout.Solidify();
		pSkeletonCBuffer = gfx.CreateVtxConstantBuffer(std::move(cblayout), 5);

		pMaterials.push_back(std::make_unique<Material>(material));
		selectedMaterial = pMaterials.size() - 1;
	}
	MeshType BonedMesh::Type() const
	{
		return MeshType::Boned;
	}
	void BonedMesh::Submit(const IGraphics& gfx, const glm::mat4 finalTransform, std::span<const glm::mat4> boneTransforms)
	{
		extraBindPtrs = {};

		this->transform = finalTransform;

		auto jitBuffer = mem::ArenaAllocator<>::GP().MakeNew<JITUpdateBuffer>(pSkeletonCBuffer.get(), (void*)boneTransforms.data(), boneTransforms.size_bytes());

		AddExtraBind(jitBuffer);

		for (auto& technique : GetSelectedMaterial().GetTechniques())
		{
			technique->Submit(gfx, this);
		}
	}
	void BonedMesh::AddAnimation(std::shared_ptr<ani::Animation> pAnimation, const std::string& name)
	{
		animationNameMapper[name] = (uint16_t)pAnimations.size();
		pAnimations.push_back(pAnimation);
		interfaces.emplace_back(skeleton, *pAnimation);
	}
	AnimationSkeletonInterface* BonedMesh::GetAnimationInterface(const std::string& name)
	{
		return &interfaces[animationNameMapper.at(name)];
	}
	inline void BonedMesh::ExtraSubmitBehavior()
	{

	}
}
