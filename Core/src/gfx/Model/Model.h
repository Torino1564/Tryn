#pragma once
#include <memory>
#include "Node.h"
#include <Core/src/gfx/IGraphics.h>
#include "Mesh.h"
#include <Core/src/gfx/ImguiManager.h>
#include <Core/src/gfx/Animation/Bone.h>
#include <Core/src/gfx/Animation/BonedMesh.h>
#include <Core/src/gfx/Material.h>
#include <Core/src/gfx/Assimp.h>
#include "ModelException.h"
#include <Core/src/gfx/Animation/AnimationManager.h>
#include <Core/src/gfx/Model/StaticMesh.h>
#include <Core/src/gfx/Animation/BonedMesh.h>

namespace tryn::gfx
{
	struct Settings
	{
		glm::vec3 angles; // (yaw, pitch, roll)
		glm::vec3 position; // (x, y, z)
	};

	class Model
	{
		friend class InstancedModelParent;
	public:
		template <TechniqueClass... T>
		static Model* MakeNew(gfx::IGraphics& gfx, const std::string& path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, bool instanced = false)
		{
			auto pModel = new Model(std::move(Model::Make<T...>(gfx, path, scale, instanced)));
			return pModel;
		}
		template <TechniqueClass... T>
		static auto MakeUnique(gfx::IGraphics& gfx, const std::string& path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, bool instanced = false)
		{
			auto pModel = new Model(std::move(Model::Make<T...>(gfx, path, scale, instanced)));
			return std::unique_ptr<Model>(pModel);
		}
		template <TechniqueClass... T>
		static Model Make(gfx::IGraphics& gfx, const std::string& path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, bool instanced = false)
		{
			Model tempModel(gfx, path);

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
				ani::AnimationManager::Get().New(tempModel.name, *pScene->mAnimations[i]);
			}

			int nextId = 0;
			tempModel.root = std::make_unique<Node>(tempModel.ParseNode(nextId, *pScene->mRootNode, scale, true));

			// parse materials
			std::vector<std::shared_ptr<Material>> pMaterials;
			pMaterials.reserve(pScene->mNumMaterials);

			for (size_t i = 0; i < pScene->mNumMaterials; i++)
			{
				if constexpr (sizeof...(T) == 0)
				{
					pMaterials.emplace_back(std::shared_ptr<Material>(new Material(Material::Make<ForwardPhong>(gfx, *pScene->mMaterials[i], path, instanced, tempModel.skeleton.has_value()))));
				}
				else
				{
					pMaterials.emplace_back(std::shared_ptr<Material>(new Material(Material::Make<T...>(gfx, *pScene->mMaterials[i], path, instanced, tempModel.skeleton.has_value()))));
				}
			}

			if (tempModel.skeleton.has_value())
			{
				for (size_t i = 0; i < pScene->mNumMeshes; i++)
				{
					const auto& mesh = *pScene->mMeshes[i];
					tempModel.pMeshes.push_back(std::make_shared<ani::BonedMesh>(gfx, pMaterials[mesh.mMaterialIndex], mesh, mesh.mName.C_Str(), tempModel.skeleton.value(), scale, tempModel.meshCounter++));
				}
			}
			else
			{
				for (size_t i = 0; i < pScene->mNumMeshes; i++)
				{
					const auto& mesh = *pScene->mMeshes[i];
					tempModel.pMeshes.push_back(std::make_shared<StaticMesh>(gfx, pMaterials[mesh.mMaterialIndex], mesh, mesh.mName.C_Str(), scale, tempModel.meshCounter++));
				}
			}

			// Set mesh Span
			std::queue<Node*> q;
			q.push(tempModel.root.get());

			while (!q.empty())
			{
				auto& current = *q.front();
				q.pop();

				current.SetMeshSpan({ tempModel.pMeshes });

				for (auto& child : current.GetChildren())
				{
					q.push(&child);
				}
			}

			return tempModel;
		}
		template <TechniqueClass... T>
		Model(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f }, bool instanced = false)
			:
			name(path.data()), gfx(gfx)
		{
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
				ani::AnimationManager::Get().New(name, *pScene->mAnimations[i]);
			}

			int nextId = 0;
			root = std::make_unique<Node>(ParseNode(nextId, *pScene->mRootNode, scale, true));

			// parse materials
			std::vector<std::shared_ptr<Material>> pMaterials;
			pMaterials.reserve(pScene->mNumMaterials);

			for (size_t i = 0; i < pScene->mNumMaterials; i++)
			{
				if constexpr (sizeof...(T) == 0)
				{
					pMaterials.emplace_back(std::shared_ptr<Material>(new Material(Material::Make<ForwardPhong>(gfx, *pScene->mMaterials[i], path, instanced, skeleton.has_value()))));
				}
				else
				{
					pMaterials.emplace_back(std::shared_ptr<Material>(new Material(Material::Make<T...>(gfx, *pScene->mMaterials[i], path, instanced, skeleton.has_value()))));
				}
			}

			if (skeleton.has_value())
			{
				for (size_t i = 0; i < pScene->mNumMeshes; i++)
				{
					const auto& mesh = *pScene->mMeshes[i];
					pMeshes.push_back(std::make_shared<ani::BonedMesh>(gfx, pMaterials[mesh.mMaterialIndex], mesh, mesh.mName.C_Str(), skeleton.value(), scale, meshCounter++));
				}
			}
			else
			{
				for (size_t i = 0; i < pScene->mNumMeshes; i++)
				{
					const auto& mesh = *pScene->mMeshes[i];
					pMeshes.push_back(std::make_shared<StaticMesh>(gfx, pMaterials[mesh.mMaterialIndex], mesh, mesh.mName.C_Str(), scale, meshCounter++));
				}
			}

			// Set mesh Span
			std::queue<Node*> q;
			q.push(root.get());

			while (!q.empty())
			{
				auto& current = *q.front();
				q.pop();

				current.SetMeshSpan({ pMeshes });

				for (auto& child : current.GetChildren())
				{
					q.push(&child);
				}
			}
		}
		void Submit(const glm::mat4& entityTransform);
		void Submit(const glm::mat4& entityTransform, std::span<const glm::mat4> boneTransforms);
		void SpawnControlWindow();
		void AddAnimation(std::shared_ptr<ani::Animation> pAnimation, const std::string&);
		glm::vec3 GetPosition() const;
		std::uint16_t GetMeshAmount() const;
		ani::BonedMesh* GetMainMesh();
	private:
		Model(IGraphics& gfx, const std::string& name);
		Node ParseNode(int& nextId, const aiNode& node, glm::vec3 scale, bool root = false);
		void ParseSkeleton(const aiNode& boneRoot);
		void ParseBone(const aiNode& bone, const uint32_t parentID);
		Settings settings = {};
	private:
		std::optional<ani::Skeleton> skeleton = std::nullopt;
		std::optional<std::shared_ptr<Material>> pMaterialOverride;
		std::uint16_t meshCounter = 0;
		gfx::IGraphics& gfx;
		std::string name;
		std::unique_ptr<Node> root;
		std::vector<std::shared_ptr<Mesh>> pMeshes;
	};
}
