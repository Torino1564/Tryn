#include "Model.h"
#include <Core/src/gfx/Assimp.h>
#include "ModelException.h"
#include <Core/src/gfx/Material.h>
#include <Core/third/glm/gtx/transform.hpp>
#include <Core/third/glm/gtc/type_ptr.hpp>
#include <Core/third/glm/glm.hpp>
#include <Core/src/gfx/Model/StaticMesh.h>

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

	Model::Model(gfx::IGraphics& gfx, std::string_view path, glm::vec3 scale , Techniques defaultTechnique)
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

		// parse materials
		std::vector<Material> materials;
		materials.reserve(pScene->mNumMaterials);
		for (size_t i = 0; i < pScene->mNumMaterials; i++)
		{
			materials.emplace_back(gfx, *pScene->mMaterials[i], path, defaultTechnique);
		}

		for (size_t i = 0; i < pScene->mNumMeshes; i++)
		{
			const auto& mesh = *pScene->mMeshes[i];
			pMeshes.push_back(std::make_unique<StaticMesh>(gfx, materials[mesh.mMaterialIndex], mesh, mesh.mName.C_Str(), scale));
		}

		int nextId = 0;
		root = std::make_unique<Node>(ParseNode(nextId, *pScene->mRootNode, scale));
	}

	void Model::Draw()
	{
		const auto rotation = glm::yawPitchRoll(settings.angles.x, settings.angles.y, settings.angles.z);
		const auto translation = glm::translate(glm::mat4(1.0f), settings.position);
		const auto transform = translation * rotation;
		root->Draw(gfx, transform);
	}
	void Model::SpawnControlWindow()
	{
		ImGui::Begin(name.c_str());
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
	Node Model::ParseNode(int& nextId, const aiNode& node, glm::vec3 scale)
	{
		const auto transform = ScaleTranslation(glm::transpose(glm::make_mat4(reinterpret_cast<const float*>(&node.mTransformation))), scale);

		std::vector<Mesh*> meshes;
		meshes.reserve(node.mNumMeshes);
		for (unsigned int i = 0; i < node.mNumMeshes; i++)
		{
			const auto meshIdx = node.mMeshes[i];
			meshes.push_back(pMeshes.at(meshIdx).get());
		}

		Node node_(nextId++, node.mName.C_Str(), std::move(meshes), transform);
		for (size_t i = 0; i < node.mNumChildren; i++)
		{
			node_.AddChild(std::move(ParseNode(nextId, *node.mChildren[i], scale)));
		}

		return node_;
	}
}