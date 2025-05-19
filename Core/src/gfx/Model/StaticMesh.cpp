#include "TrynPCH.h"
#include "StaticMesh.h"
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>
#include <Core/src/gfx/Bindables/IBufferBase.h>
#include <format>
#include <utility>
#include <Core/src/gfx/Material.h>

#include "Core/src/gfx/Bindables/SOAVertexBuffer.h"

namespace tryn::gfx
{
	StaticMesh::StaticMesh(const IGraphics& gfx, const aiMesh& mesh, std::string_view tag_, const std::shared_ptr<Material>& pMaterial, glm::vec3 scale, const std::optional<std::uint16_t> meshID)
		: Mesh()
	{
		auto material = pMaterial ? pMaterial : Material::MakeDefault(gfx);

		if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
		{
			tag = std::format("{}#Scale[X:{},Y:{},Z:{}]", tag_, scale.x, scale.y, scale.z);
		}
		else
		{
			tag = tag_;
		}

		ID = meshID.value_or(0);

		const auto indices = IndexBuffer(mesh);

		indexCount = static_cast<uint32_t>(indices.Size());
		pSOAVertexBuffer = ISOAVertexBuffer::Resolve(gfx);
		pSOAVertexBuffer->InitFields(gfx, mesh);
		pIndexBuffer = IIndexBuffer::Resolve(gfx, std::make_shared<IndexBuffer>(indices));
		pTopology = IPrimitiveTopology::Resolve(gfx);
		InitTransformCBuf(gfx);

		pMaterials.clear();
		pMaterials.emplace_back(std::move(material));
		selectedMaterial = pMaterials.size() - 1;
	}

	StaticMesh::StaticMesh(const IGraphics& gfx, const Shape3D& shape,
		std::string_view tag, const std::shared_ptr<Material>& pMaterial, glm::vec3 scale, std::optional<std::uint16_t> meshID)
			: Mesh()
	{
		// TODO: Finish this

		//auto material = pMaterial ? pMaterial : Material::MakeDefault(gfx);

		//if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
		//{
		//	this->tag = std::format("{}#Scale[X:{},Y:{},Z:{}]", tag, scale.x, scale.y, scale.z);
		//}
		//else
		//{
		//	this->tag = tag;
		//}

		//ID = meshID.value_or(0);
		//const auto indices = IndexBuffer(shape);

		//indexCount = static_cast<uint32_t>(indices.Size());

		//pIndexBuffer = IIndexBuffer::Resolve(gfx, std::make_shared<IndexBuffer>(indices));
		//pTopology = IPrimitiveTopology::Resolve(gfx);
		//InitTransformCBuf(gfx);

		//this->pMaterials.emplace_back(std::move(material));
		//this->selectedMaterial = pMaterials.size() - 1;
	}

	StaticMesh::StaticMesh(const IGraphics& gfx, const Microsoft::glTF::MeshPrimitive& primitive, const gfx::WinGLTFLoaderContext& context, std::string_view tag_,
		const std::shared_ptr<Material>& pMaterial, glm::vec3 scale, const std::optional<std::uint16_t> meshID)
			: Mesh()
	{
		auto material = pMaterial ? pMaterial : Material::MakeDefault(gfx);

		if (scale.x != 1.0f || scale.y != 1.0f || scale.z != 1.0f)
		{
			tag = std::format("{}#Scale[X:{},Y:{},Z:{}]", tag_, scale.x, scale.y, scale.z);
		}
		else
		{
			tag = tag_;
		}

		ID = meshID.value_or(0);
		
		const auto indices = IndexBuffer(primitive, context);

		indexCount = static_cast<uint32_t>(indices.Size());
		pSOAVertexBuffer = ISOAVertexBuffer::Resolve(gfx);
		pSOAVertexBuffer->InitFields(gfx, primitive, context);
		pIndexBuffer = IIndexBuffer::Resolve(gfx, std::make_shared<IndexBuffer>(std::move(indices)));
		pTopology = IPrimitiveTopology::Resolve(gfx);
		InitTransformCBuf(gfx);

		pMaterials.clear();
		pMaterials.emplace_back(std::move(material));
		selectedMaterial = pMaterials.size() - 1;
	}

	MeshType StaticMesh::Type() const
	{
		return MeshType::Static;
	}

	void StaticMesh::EnableOrAddTechnique(const IGraphics& gfx, const utl::UUID_t techniqueUUID,
		const std::span<uint16_t> materialIndex)
	{
		EnableOrAddTechniqueEx(gfx, techniqueUUID, false, materialIndex);
	}

	void StaticMesh::AddTechnique(const IGraphics& gfx, const utl::UUID_t techniqueUUID, const std::span<uint16_t> materialIndex,
		const bool enabled)
	{
		AddTechniqueEx(gfx, techniqueUUID, false, materialIndex, enabled);
	}
}
