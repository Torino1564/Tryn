#include "Model.h"
#include <Core/src/gfx/Assimp.h>
#include "ModelException.h"
#include <Core/src/gfx/Material.h>
#include <Core/third/glm/gtx/transform.hpp>
#include <Core/third/glm/gtc/type_ptr.hpp>
#include <Core/third/glm/glm.hpp>
#include <Core/src/gfx/Model/StaticMesh.h>
#include <Core/src/gfx/Animation/BonedMesh.h>
#include <Core/src/mem/ArenaAllocator.h>
#include <queue>
#include <Core/src/gfx/Animation/Bone.h>

namespace tryn::gfx
{
	glm::mat4 ScaleTranslation(const glm::mat4& mat, const glm::vec3& scale)
	{
		glm::mat4 scaledMat = mat;
		scaledMat[3][0] *= scale.x;
		scaledMat[3][1] *= scale.y;
		scaledMat[3][2] *= scale.z;
		return scaledMat;
	}

	glm::mat4 AssimpMat4ToGLMMat4(const aiMatrix4x4& aiMatrix) {
		glm::mat4 glmMatrix;
		glmMatrix[0][0] = aiMatrix.a1; glmMatrix[1][0] = aiMatrix.a2;
		glmMatrix[2][0] = aiMatrix.a3; glmMatrix[3][0] = aiMatrix.a4;
		glmMatrix[0][1] = aiMatrix.b1; glmMatrix[1][1] = aiMatrix.b2;
		glmMatrix[2][1] = aiMatrix.b3; glmMatrix[3][1] = aiMatrix.b4;
		glmMatrix[0][2] = aiMatrix.c1; glmMatrix[1][2] = aiMatrix.c2;
		glmMatrix[2][2] = aiMatrix.c3; glmMatrix[3][2] = aiMatrix.c4;
		glmMatrix[0][3] = aiMatrix.d1; glmMatrix[1][3] = aiMatrix.d2;
		glmMatrix[2][3] = aiMatrix.d3; glmMatrix[3][3] = aiMatrix.d4;
		return glmMatrix;
	}

	Model::Model(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale , Techniques defaultTechnique, bool instanced)
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

		int nextId = 0;
		root = std::make_unique<Node>(ParseNode(nextId, *pScene->mRootNode, scale, true));

		// parse materials
		std::vector<Material> materials;
		materials.reserve(pScene->mNumMaterials);
		for (size_t i = 0; i < pScene->mNumMaterials; i++)
		{
			materials.emplace_back(gfx, *pScene->mMaterials[i], path, defaultTechnique, instanced, skeleton.has_value());
		}


		if (skeleton.has_value())
		{
			for (size_t i = 0; i < pScene->mNumMeshes; i++)
			{
				const auto& mesh = *pScene->mMeshes[i];
				pMeshes.push_back(std::make_unique<ani::BonedMesh>(gfx, materials[mesh.mMaterialIndex], mesh, mesh.mName.C_Str(), skeleton.value(), scale, meshCounter++));
			}
		}
		else
		{
			for (size_t i = 0; i < pScene->mNumMeshes; i++)
			{
				const auto& mesh = *pScene->mMeshes[i];
				pMeshes.push_back(std::make_unique<StaticMesh>(gfx, materials[mesh.mMaterialIndex], mesh, mesh.mName.C_Str(), scale, meshCounter++));
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

	void Model::Submit(const glm::mat4& entityTransform = glm::identity<glm::mat4>())
	{
		const auto rotation = glm::yawPitchRoll(settings.angles.x, settings.angles.y, settings.angles.z);
		const auto translation = glm::translate(glm::mat4(1.0f), settings.position);
		const auto transform = translation * rotation;
		root->Submit(gfx, entityTransform * transform);
	}
	void Model::SpawnControlWindow()
	{
		ImGui::Begin(name.c_str());
		ImGui::Text("Model Settings");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Yaw", &settings.angles.x, -180.f, 180.f);
		ImGui::SliderAngle("Pitch", &settings.angles.y, -90.f, 90.f);
		ImGui::SliderAngle("Roll", &settings.angles.z, -180.f, 180.f);
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &settings.position.x, -20.0f, 20.0f);
		ImGui::SliderFloat("Y", &settings.position.y, -20.0f, 20.0f);
		ImGui::SliderFloat("Z", &settings.position.z, -20.0f, 20.0f);
		ImGui::End();
	}
	glm::vec3 Model::GetPosition() const
	{
		return settings.position;
	}
	std::uint16_t Model::GetMeshAmount() const
	{
		return meshCounter + 1;
	}
	Node Model::ParseNode(int& nextId, const aiNode& node, glm::vec3 scale, bool root)
	{
		auto skeletonNodeIndex = -1;
		// If its a root node, find if theres a skeleton
		if (root)
		{
			for (int i = 0; i < node.mNumChildren; i++)
			{
				auto& child = *node.mChildren[i];

				// find if any children is a skeleton

				auto isSkeleton = true;
				auto numChildren = 0;

				std::queue<aiNode*> q;
				q.push(&child);

				while (!q.empty())
				{
					auto current = q.front();
					q.pop();
					if (current->mNumMeshes != 0)
					{
						isSkeleton = false;
						break;
					}
					for (int i = 0; i < current->mNumChildren; i++)
					{
						numChildren++;
						q.push(current->mChildren[i]);
					}
				}
				
				// Parse Skeleton if found any
				if (isSkeleton && (numChildren != 0))
				{
					ParseSkeleton(child);
					// Supports only one skeleton!
					skeletonNodeIndex = i;
					break;
				}

			}
		}

		const auto transform = ScaleTranslation(transpose(glm::make_mat4(reinterpret_cast<const float*>(&node.mTransformation))), scale);

		std::vector<uint16_t> meshIds;
		meshIds.reserve(node.mNumMeshes);
		for (unsigned int i = 0; i < node.mNumMeshes; i++)
		{
			const auto meshIdx = node.mMeshes[i];
			meshIds.push_back(meshIdx);
		}

		Node node_(nextId++, node.mName.C_Str(), std::move(meshIds), transform);
		for (auto i = 0; i < node.mNumChildren; i++)
		{
			if (root && i == skeletonNodeIndex)
				continue;
			node_.AddChild(std::move(ParseNode(nextId, *node.mChildren[i], scale)));
		}

		return node_;
	}
	void Model::ParseSkeleton(const aiNode& boneRoot)
	{
		skeleton.emplace();
		skeleton->bones.emplace_back(boneRoot.mName.C_Str(), 0, 0);
		for (auto i = 0; i < boneRoot.mNumChildren; i++)
		{
			ParseBone(*boneRoot.mChildren[i], 0);
		}
	}
	void Model::ParseBone(const aiNode& bone, const uint32_t parentID)
	{
		auto thisID = skeleton->NextID();
		skeleton->bones.emplace_back(bone.mName.C_Str(), thisID, parentID);
		for (auto i = 0; i < bone.mNumChildren; i++)
		{
			ParseBone(*bone.mChildren[i], thisID);
		}
	}
}