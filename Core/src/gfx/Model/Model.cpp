#include "Model.h"
#include <Core/src/gfx/Assimp.h>
#include "ModelException.h"
#include <Core/src/gfx/Material.h>

namespace tryn::gfx
{
	Model::Model(gfx::IGraphics & gfx, std::string_view path, glm::vec3 scale)
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

		// parse materials
		std::vector<Material> materials;
		materials.reserve(pScene->mNumMaterials);
		for (size_t i = 0; i < pScene->mNumMaterials; i++)
		{
			materials.emplace_back(gfx, *pScene->mMaterials[i], path);
		}

		for (size_t i = 0; i < pScene->mNumMeshes; i++)
		{
			const auto& mesh = *pScene->mMeshes[i];
			pMeshes.push_back(std::make_unique<Mesh>(gfx, materials[mesh.mMaterialIndex], mesh, scale));
		}

		int nextId = 0;
		root = ParseNode(nextId, *pScene->mRootNode, scale);
	}
	std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node, glm::vec3 scale)
	{
		std::vector<Mesh&> meshes;
		meshes.reserve(node.mNumMeshes);
		for ( int i = 0 ; i < node.mNumMeshes ; i++ )
		{
			const auto meshIdx = node.mMeshes[i];
			meshes.push_back(*(pMeshes.at(meshIdx)));
		}

		auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(meshes), transform);
		for (size_t i = 0; i < node.mNumChildren; i++)
		{
			pNode->AddChild(ParseNode(nextId, *node.mChildren[i], scale));
		}
	}
}