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

namespace tryn::gfx
{
	template <typename T>
	concept DeriedFromTechnique = std::derived_from<T, Technique>;

	struct Settings
	{
		glm::vec3 angles; // (yaw, pitch, roll)
		glm::vec3 position; // (x, y, z)
	};

	class Model
	{
		friend class InstancedModelParent;
	public:
		Model(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, bool instanced = false );

		template<DeriedFromTechnique FirstTechnique = ForwardPhong, DeriedFromTechnique... OtherTechniques>
		static std::unique_ptr<Model> Make(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, bool instanced = false);
		void Submit(const glm::mat4& entityTransform);
		void Submit(const glm::mat4& entityTransform, std::span<const glm::mat4> boneTransforms);
		void SpawnControlWindow();
		void AddAnimation(std::shared_ptr<ani::Animation> pAnimation, const std::string&);
		glm::vec3 GetPosition() const;
		std::uint16_t GetMeshAmount() const;
		ani::BonedMesh* GetMainMesh();
	private:
		Model(std::string_view path, gfx::IGraphics& gfx);
		Node ParseNode(int& nextId, const aiNode& node, glm::vec3 scale, bool root = false);
		void ParseSkeleton(const aiNode& boneRoot);
		void ParseBone(const aiNode& bone, const uint32_t parentID);
		Settings settings = {};
	private:
		std::optional<ani::Skeleton> skeleton = std::nullopt;
		std::uint16_t meshCounter = 0;
		gfx::IGraphics& gfx;
		std::string name;
		std::unique_ptr<Node> root;
		std::vector<std::shared_ptr<Mesh>> pMeshes;
	};

	template<DeriedFromTechnique FirstTechnique, DeriedFromTechnique... OtherTechniques>
	static std::unique_ptr<Model> Model::Make(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale, bool instanced)
	{
		std::unique_ptr<Model> pModel = std::make_unique<Model>(std::move(Model{ path, gfx }));

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

		for (auto i = 0; i < pScene->mNumAnimations; i++)
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
					materials.emplace_back(Material::Make<ReplaceTemplateParam<FirstTechnique, true, true>::type, ReplaceTemplateParam<OtherTechniques..., true, true>::type>(gfx, *pScene->mMaterials[i], path));
				else
					materials.emplace_back(Material::Make<ReplaceTemplateParam<FirstTechnique, true, true>::type>(gfx, *pScene->mMaterials[i], path));
				break;
			case 1:
				if constexpr (sizeof...(OtherTechniques) != 0)
					materials.emplace_back(Material::Make<ReplaceTemplateParam<FirstTechnique, true, false>::type, ReplaceTemplateParam<OtherTechniques..., true, false>::type>(gfx, *pScene->mMaterials[i], path));
				else
					materials.emplace_back(Material::Make<ReplaceTemplateParam<FirstTechnique, true, false>::type>(gfx, *pScene->mMaterials[i], path));
				break;
			case 2:
				if constexpr (sizeof...(OtherTechniques) != 0)
					materials.emplace_back(Material::Make<ReplaceTemplateParam<FirstTechnique, false, true>::type, ReplaceTemplateParam<OtherTechniques..., false, true>::type>(gfx, *pScene->mMaterials[i], path));
				else
					materials.emplace_back(Material::Make<ReplaceTemplateParam<FirstTechnique, false, true>::type>(gfx, *pScene->mMaterials[i], path));
				break;
			case 3:
				if constexpr (sizeof...(OtherTechniques) != 0)
					materials.emplace_back(Material::Make<ReplaceTemplateParam<FirstTechnique, false, false>::type, ReplaceTemplateParam<OtherTechniques..., false, false>::type>(gfx, *pScene->mMaterials[i], path));
				else
					materials.emplace_back(Material::Make<ReplaceTemplateParam<FirstTechnique, false, false>::type>(gfx, *pScene->mMaterials[i], path));
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
