#include "TrynPCH.h"
#include "StaticMesh.h"
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>
#include <Core/src/gfx/Bindables/IBuffer.h>
#include <format>
#include <utility>
#include <Core/src/gfx/Material.h>

namespace tryn::gfx
{
	StaticMesh::StaticMesh(const IGraphics& gfx, const aiMesh& mesh, std::string_view tag, std::shared_ptr<Material> pMaterial, glm::vec3 scale, std::optional<std::uint16_t> meshID)
	{
		auto material = pMaterial ? pMaterial : Material::MakeDefault(gfx);

		if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
		{
			this->tag = std::format("{}#Scale[X:{},Y:{},Z:{}]", tag, scale.x, scale.y, scale.z);
		}
		else
		{
			this->tag = tag;
		}

		ID = meshID.value_or(0);
		auto vertexBuffer = material->ExtractVertices(mesh);
		vertexBuffer.SetClean();
		const auto indices = material->ExtractIndices(mesh);

		indexCount = static_cast<uint32_t>(indices.Size());

		pVertexBuffer = IVertexBuffer::Resolve(gfx, std::make_shared<VertexBuffer>(vertexBuffer), this->tag);
		pIndexBuffer = IIndexBuffer::Resolve(gfx, std::make_shared<IndexBuffer>(indices));
		pTopology = IPrimitiveTopology::Resolve(gfx);
		InitTransformCBuf(gfx);

		this->pMaterials.emplace_back(std::move(material));
		this->selectedMaterial = pMaterials.size() - 1;
	}

	StaticMesh::StaticMesh(const IGraphics& gfx, const Shape3D& shape,
		std::string_view tag, std::shared_ptr<Material> pMaterial, glm::vec3 scale, std::optional<std::uint16_t> meshID)
	{
		auto material = pMaterial ? pMaterial : Material::MakeDefault(gfx);

		if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
		{
			this->tag = std::format("{}#Scale[X:{},Y:{},Z:{}]", tag, scale.x, scale.y, scale.z);
		}
		else
		{
			this->tag = tag;
		}

		ID = meshID.value_or(0);
		auto vertexBuffer = material->ExtractVertices(shape);
		vertexBuffer.SetClean();
		const auto indices = material->ExtractIndices(shape);

		indexCount = static_cast<uint32_t>(indices.Size());

		pVertexBuffer = IVertexBuffer::Resolve(gfx, std::make_shared<VertexBuffer>(vertexBuffer), this->tag);
		pIndexBuffer = IIndexBuffer::Resolve(gfx, std::make_shared<IndexBuffer>(indices));
		pTopology = IPrimitiveTopology::Resolve(gfx);
		InitTransformCBuf(gfx);

		this->pMaterials.emplace_back(std::move(material));
		this->selectedMaterial = pMaterials.size() - 1;
	}

	StaticMesh::StaticMesh(const IGraphics& gfx, const Microsoft::glTF::Mesh& mesh, const Microsoft::glTF::Document& document, std::string_view tag,
		std::shared_ptr<Material> pMaterial, glm::vec3 scale, std::optional<std::uint16_t> meshID)
	{
		auto material = pMaterial ? pMaterial : Material::MakeDefault(gfx);

		if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
		{
			this->tag = std::format("{}#Scale[X:{},Y:{},Z:{}]", tag, scale.x, scale.y, scale.z);
		}
		else
		{
			this->tag = tag;
		}

		ID = meshID.value_or(0);
		auto vertexBuffer = material->ExtractVertices(mesh, document);
		vertexBuffer.SetClean();
		const auto indices = material->ExtractIndices(mesh, document);

		indexCount = static_cast<uint32_t>(indices.Size());

		pVertexBuffer = IVertexBuffer::Resolve(gfx, std::make_shared<VertexBuffer>(std::move(vertexBuffer)), this->tag);
		pIndexBuffer = IIndexBuffer::Resolve(gfx, std::make_shared<IndexBuffer>(std::move(indices)));
		pTopology = IPrimitiveTopology::Resolve(gfx);
		InitTransformCBuf(gfx);

		this->pMaterials.emplace_back(std::move(material));
		this->selectedMaterial = pMaterials.size() - 1;
	}

	MeshType StaticMesh::Type() const
	{
		return MeshType::Static;
	}
}