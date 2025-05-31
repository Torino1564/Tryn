#include "TrynPCH.h"
#include "BonedMesh.h"
#include <Core/src/gfx/Bindables/IBufferBase.h>
#include <Core/src/mem/ArenaAllocator.h>
#include <Core/src/gfx/Bindables/JITUpdateBuffer.h>
#include <Core/src/gfx/Material.h>
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>

static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
	glm::mat4 to;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

namespace tryn::gfx::ani
{
	BonedMesh::BonedMesh(const IGraphics& gfx, std::shared_ptr<Material> pMaterial, const aiMesh& mesh, std::string_view tag, ani::Skeleton& skeleton, glm::vec3 scale, std::optional<std::uint16_t> meshID)
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
			auto boneIt = std::ranges::find_if(skeleton.bones, [&](const auto& boneInArray) {
				return strcmp(boneInArray.name.c_str(),bone.mName.C_Str()) ? false : true;
				});

			trynass_msg(boneIt != skeleton.bones.end(), L"Did not find a bone creatign a BonedMesh!");

			boneIt->inverseBP = ConvertMatrixToGLMFormat(bone.mOffsetMatrix);
			
			boneIt->boneWeights.reserve(bone.mNumWeights);

			for (unsigned int i = 0; i < bone.mNumWeights; i++)
			{
				boneIt->boneWeights.push_back({ bone.mWeights[i].mVertexId, bone.mWeights[i].mWeight });
			}
		}

		ID = meshID.value_or(0);
		const auto indices = IndexBuffer(mesh);

		indexCount = static_cast<uint32_t>(indices.Size());

		pIndexBuffer = IIndexBuffer::Resolve(gfx, std::make_shared<IndexBuffer>(indices));
		pTopology = IPrimitiveTopology::Resolve(gfx);
		InitTransformCBuf(gfx);

		ConstantBufferLayout cblayout;
		cblayout.Append(ConstantBufferLayout::Array, "boneArray");
		ConstantBufferLayout::Node arrayElement(ConstantBufferLayout::Matrix4, "boneTransform");
		cblayout["boneArray"].Set(std::move(arrayElement), skeleton.bones.size());
		cblayout.Solidify();
		pSkeletonCBuffer = gfx.CreateVtxConstantBuffer(std::move(cblayout), 5);

		pMaterials.push_back(std::move(pMaterial));
		selectedMaterial = pMaterials.size() - 1;
	}

	BonedMesh::BonedMesh(const IGraphics& gfx, std::shared_ptr<Material> pMaterial, const Microsoft::glTF::Mesh& mesh,
		std::string_view tag, ani::Skeleton& skeleton, glm::vec3 scale, std::optional<std::uint16_t> meshID)
			:
		skeleton(skeleton)
	{

	}

	MeshType BonedMesh::Type() const
	{
		return MeshType::Boned;
	}
	void BonedMesh::SubmitBoned(const IGraphics& gfx, const glm::mat4& finalTransform, std::span<const glm::mat4> boneTransforms)
	{
		extraBindPtrs = {};

		this->transform = finalTransform;

		auto& jitBuffer = mem::ArenaAllocator<>::GP().Emplace(JITUpdateBuffer::Make(pSkeletonCBuffer, (void*)boneTransforms.data(), boneTransforms.size_bytes()));

		AddExtraBind(&jitBuffer);

		for (auto& [enabled, technique]: pTechniques)
		{
			if (enabled)
				technique->Submit(gfx, this);
		}
	}
	void BonedMesh::AddAnimation(const std::shared_ptr<ani::Animation>& pAnimation, const std::string& name)
	{
		animationNameMapper[name] = (uint16_t)pAnimations.size();
		pAnimations.push_back(pAnimation);
		interfaces.emplace_back(skeleton, *pAnimation);
	}
	AnimationSkeletonInterface* BonedMesh::GetAnimationInterface(const std::string& name)
	{
		if (name == "?")
		{
			trylog.warn(L"Defaulted to first animation");

			if (interfaces.size() == 0)
			{
				trylog.warn(L"No animations found!");
				return nullptr;
			}
			return &interfaces[0];
		}
		return &interfaces[animationNameMapper.at(name)];
	}

	void BonedMesh::EnableOrAddTechnique(const IGraphics& gfx, const utl::UUID_t techniqueUUID,
		const std::span<uint16_t> materialIndex)
	{
		EnableOrAddTechniqueEx(gfx, techniqueUUID, true, materialIndex);
	}

	void BonedMesh::AddTechnique(const IGraphics& gfx, const utl::UUID_t techniqueUUID, const std::span<uint16_t> materialIndex,
		const bool enabled)
	{
		AddTechniqueEx(gfx, techniqueUUID, true, materialIndex, enabled);
	}

	inline void BonedMesh::ExtraSubmitBehavior()
	{

	}
}
