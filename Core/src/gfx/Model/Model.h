#pragma once
#include <memory>
#include "Node.h"
#include <Core/src/gfx/IGraphics.h>
#include "Mesh.h"
#include <Core/src/gfx/ImguiManager.h>
#include <Core/src/gfx/Animation/Bone.h>
#include <Core/src/gfx/Animation/BonedMesh.h>
#include <Core/src/gfx/Model/StaticMesh.h>
#include <concepts>
#include <Core/src/gfx/Assimp.h>
#include "ModelException.h"
#include <Core/src/gfx/Animation/AnimationManager.h>
#include "Core/src/gfx/Render/Techniques/ForwardPhong.h"
#include <Core/src/gfx/Material.h>
#include <Core/src/ser/StreamIO.h>

namespace tryn::gfx
{
	template <typename T>
	concept DerivedFromTechniqueBase = std::derived_from<typename T::Type, TechniqueBase>;

	template <template <bool, bool> typename T>
	concept BaseTechniqueClass = DerivedFromTechniqueBase<T<false, false>>;

	

	struct Settings
	{
		glm::vec3 angles; // (yaw, pitch, roll)
		glm::vec3 position; // (x, y, z)
	};

	class Model
	{
		friend class InstancedModelParent;
	public:
		Model(const gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, bool instanced = false );
		template <template <bool, bool> typename FirstTechnique = ForwardPhongBase, template <bool, bool> typename... OtherTechniques>
			requires BaseTechniqueClass<FirstTechnique> && (sizeof...(OtherTechniques) == 0 || BaseTechniqueClass<OtherTechniques...>)
		static std::unique_ptr<Model> Make(const gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, bool instanced = false);
		static std::unique_ptr<Model> Make(const gfx::IGraphics& gfx, std::string_view path, const std::span<utl::UUID_t> techniqueUUIDs, glm::vec3 scale = { 1.0f,1.0f,1.0f }, bool instanced = false);

		void Submit(const glm::mat4& entityTransform);
		void Submit(const glm::mat4& entityTransform, std::span<const glm::mat4> boneTransforms);
		void SpawnControlWindow();
		void AddAnimation(std::shared_ptr<ani::Animation> pAnimation, const std::string&);
		glm::vec3 GetPosition() const;
		std::uint16_t GetMeshAmount() const;
		ani::BonedMesh* GetMainMesh();
		gfx::IGraphics* GetGfx();
	private:
		Model(std::string_view path, const gfx::IGraphics& gfx);
		Node ParseNode(int& nextId, const aiNode& node, glm::vec3 scale, bool root = false);
		void ParseSkeleton(const aiNode& boneRoot);
		void ParseBone(const aiNode& bone, const uint32_t parentID);
		Settings settings = {};
	private:
		std::optional<ani::Skeleton> skeleton = std::nullopt;
		std::uint16_t meshCounter = 0;
		const gfx::IGraphics& gfx;
		std::string name;
		std::unique_ptr<Node> root;
		std::vector<std::shared_ptr<Mesh>> pMeshes;

	public:
		struct Serializer : public tryn::ser::Serializer<Model>
		{
			static void Write(const tryn::ser::StreamWriter& streamWriter, const Model& data, const bool binary = true,
			                  const std::string& name = "")
			{
				streamWriter.Serialize(data.name, binary, name);

			}

			template <typename Data = void>
			static Model Read(const tryn::ser::StreamReader& streamReader, const bool binary = true, const Data* pExtraData = nullptr)
			{
			}

			template <typename Data = void>
			static void Read(Model& data, const tryn::ser::StreamReader& streamReader, const bool binary = true, const Data* pExtraData = nullptr)
			{
			}
		};
	};

	template <template <bool, bool> typename FirstTechnique, template <bool, bool> typename... OtherTechniques>
		requires BaseTechniqueClass<FirstTechnique> && (sizeof...(OtherTechniques) == 0 || BaseTechniqueClass<OtherTechniques...>)
	std::unique_ptr<Model> Model::Make(const gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale, bool instanced)
	{
		auto pModel = std::make_unique<Model>(std::move(Model{ path, gfx }));

		auto& imp = AssimpManager::Get();
		const auto pScene = imp.ReadFile(path.data(),
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace
		);

		if (pScene == nullptr)
		{
			throw ModelException(imp.GetErrorString());
		}

		if (scale != glm::vec3{ 1.0f,1.0f,1.0f })
		{
			for (size_t i = 0; i < pScene->mNumMeshes; i++)
			{
				const auto& mesh = *pScene->mMeshes[i];
				for (size_t j = 0; j < mesh.mNumVertices; j++)
				{
					auto& vertex = mesh.mVertices[j];
					vertex.x *= scale.x;
					vertex.y *= scale.y;
					vertex.z *= scale.z;
				}
			}
		}

		for (unsigned int i = 0; i < pScene->mNumAnimations; i++)
		{
			ani::AnimationManager::Get().New(pModel->name, *pScene->mAnimations[i]);
		}

		int nextId = 0;
		pModel->root = std::make_unique<Node>(pModel->ParseNode(nextId, *pScene->mRootNode, scale, true));

		// parse materials
		std::vector<Material> materials;
		materials.reserve(pScene->mNumMaterials);

		int switchCase = 0;

		if (instanced && pModel->skeleton.has_value())
		{
			switchCase = 0;
		}
		else
		{
			if (instanced)
			{
				switchCase = 1;
			}
			else if (pModel->skeleton.has_value())
			{
				switchCase = 2;
			}
			else
			{
				switchCase = 3;
			}
		}

		for (size_t i = 0; i < pScene->mNumMaterials; i++)
		{
			switch (switchCase)
			{
			case 0:
				if constexpr (sizeof...(OtherTechniques) != 0)
					materials.emplace_back(Material::Make<FirstTechnique<true, true>, OtherTechniques<true, true>...>(gfx, *pScene->mMaterials[i], path));
				else
					materials.emplace_back(Material::Make<FirstTechnique<true, true>>(gfx, *pScene->mMaterials[i], path));
				break;
			case 1:
				if constexpr (sizeof...(OtherTechniques) != 0)
					materials.emplace_back(Material::Make<FirstTechnique<true, false>, OtherTechniques<true, false>...>(gfx, *pScene->mMaterials[i], path));
				else
					materials.emplace_back(Material::Make<FirstTechnique<true, false>>(gfx, *pScene->mMaterials[i], path));
				break;
			case 2:
				if constexpr (sizeof...(OtherTechniques) != 0)
					materials.emplace_back(Material::Make<FirstTechnique<false, true>, OtherTechniques<false, true>...>(gfx, *pScene->mMaterials[i], path));
				else
					materials.emplace_back(Material::Make<FirstTechnique<false, true>>(gfx, *pScene->mMaterials[i], path));
				break;
			case 3:
				if constexpr (sizeof...(OtherTechniques) != 0)
					materials.emplace_back(Material::Make<FirstTechnique<false, false>, OtherTechniques<false, false>...>(gfx, *pScene->mMaterials[i], path));
				else
					materials.emplace_back(Material::Make<FirstTechnique<false, false>>(gfx, *pScene->mMaterials[i], path));
				break;
			default:
				break;
			}
		}

		if (pModel->skeleton.has_value())
		{
			for (size_t i = 0; i < pScene->mNumMeshes; i++)
			{
				const auto& mesh = *pScene->mMeshes[i];
				pModel->pMeshes.push_back(std::make_shared<ani::BonedMesh>(gfx, materials[mesh.mMaterialIndex], mesh, mesh.mName.C_Str(), pModel->skeleton.value(), scale, pModel->meshCounter++));
			}
		}
		else
		{
			for (size_t i = 0; i < pScene->mNumMeshes; i++)
			{
				const auto& mesh = *pScene->mMeshes[i];
				auto pMesh = std::make_shared<StaticMesh>(gfx, materials[mesh.mMaterialIndex], mesh, mesh.mName.C_Str(), scale, pModel->meshCounter++);
				pModel->pMeshes.push_back(std::move(pMesh));
			}
		}

		// Set mesh Span
		std::queue<Node*> q;
		q.push(pModel->root.get());

		while (!q.empty())
		{
			auto& current = *q.front();
			q.pop();

			current.SetMeshSpan({ pModel->pMeshes });

			for (auto& child : current.GetChildren())
			{
				q.push(&child);
			}
		}

		return pModel;
	}
}
