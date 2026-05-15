
#include "SOAVertexBuffer.h"

#undef min
#undef max

#include <GLTFSDK/Document.h>
#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceReader.h>
#include <GLTFSDK/MeshPrimitiveUtils.h>
#include <Core/src/gfx/win/gltfSDK.h>
#include "IBufferBase.h"
#include "Core/src/gfx/BindablePool.h"
#include <string_view>

using namespace std::literals;

namespace tryn::gfx
{
	static constexpr auto MAX_ITERABLE_NUMBER = 3;
	std::shared_ptr<ISOAVertexBuffer> ISOAVertexBuffer::Resolve(const IGraphics& gfx)
	{
		return BindablePool::Resolve<ISOAVertexBuffer>(gfx);
	}

	std::string ISOAVertexBuffer::GenerateID(const IGraphics& gfx)
	{
		return "?";
	}

	template <unsigned SIZE, unsigned N = 0>
	static void InsertElement(ISOAVertexBuffer& buffer, const IGraphics& gfx, const Microsoft::glTF::MeshPrimitive& primitive,
	                          const WinGLTFLoaderContext& context)
	{
		static constexpr auto NthElement = static_cast<VertexLayout::VertexElement>(N);

		if constexpr (VertexLayout::VertexElementAttr<NthElement>::semantic == "COLOR"sv ||
			VertexLayout::VertexElementAttr<NthElement>::semantic == "TEXCOORD"sv)
		{
			// These are iterable elements. Eg: COLOR_0, COLOR_1, etc

			for (unsigned n = 0; n < MAX_ITERABLE_NUMBER; n++)
			{
				const auto id = std::string(VertexLayout::VertexElementAttr<NthElement>::semantic) + "_" + std::to_string(n);
				if (primitive.HasAttribute(id))
				{
					try
					{
						VertexLayout layout;
						layout.AppendElement(NthElement);
						auto nThBuffer = std::make_shared<VertexBuffer>(layout, primitive, context);
						auto pBuffer = IVertexBuffer::Resolve(gfx, nThBuffer);
						buffer.Append(pBuffer, VertexLayout::VertexElementAttr<NthElement>::semantic.data());
					}
					catch(std::exception& e)
					{
						trylog.warn(L"Could not add element of type: " + utl::ToWide(id));
					}
				}
			}
		}

		if (primitive.HasAttribute(VertexLayout::VertexElementAttr<NthElement>::semantic.data()))
		{
			const auto id = VertexLayout::VertexElementAttr<NthElement>::semantic.data();
			if (primitive.HasAttribute(id))
			{
				try
				{
					VertexLayout layout;
					layout.AppendElement(NthElement);
					auto nThBuffer = std::make_shared<VertexBuffer>(layout, primitive, context);
					auto pBuffer = IVertexBuffer::Resolve(gfx, nThBuffer);

					buffer.Append(pBuffer, id);
				}
				catch (std::exception& e)
				{
					trylog.warn(L"Could not add element of type: " + utl::ToWide(id));
				}
			}
		}

		// Iterate over all elements
		if constexpr (N < SIZE)
		{
			return InsertElement<SIZE, N + 1>(buffer, gfx, primitive, context);
		}
	}

	template <unsigned SIZE, unsigned N = 0>
	static void InsertElement(ISOAVertexBuffer& buffer, const IGraphics& gfx, const aiMesh& mesh)
	{
		static constexpr auto elementType = static_cast<VertexLayout::VertexElement>(N);
		using Attr = VertexLayout::VertexElementAttr<elementType>;

		if constexpr (elementType == VertexLayout::Float4Color || elementType == VertexLayout::UV)
		{
			// Iterate over COLOR_n or TEXCOORD_n
			for (unsigned n = 0; n < MAX_ITERABLE_NUMBER; n++)
			{
				std::string_view id = Attr::semantic;
				if ((mesh.HasVertexColors(n) && elementType == VertexLayout::Float4Color ) ||
					(mesh.HasTextureCoords(n) && elementType == VertexLayout::UV))
				{
					VertexLayout layout;
					layout.AppendElement(elementType);

					auto nThBuffer = std::make_shared<VertexBuffer>(layout, mesh);
					auto pBuffer = IVertexBuffer::Resolve(gfx, nThBuffer);

					buffer.Append(pBuffer, id.data());
				}
			}
		}
		else
		{
			bool shouldAdd = false;
			std::string_view semanticName = Attr::semantic;

			if constexpr (elementType == VertexLayout::Position3D)
				shouldAdd = mesh.HasPositions();
			else if constexpr (elementType == VertexLayout::Normal)
				shouldAdd = mesh.HasNormals();
			else if constexpr (elementType == VertexLayout::Tangent || elementType == VertexLayout::Bitangent)
				shouldAdd = mesh.HasTangentsAndBitangents();
			else if constexpr (elementType == VertexLayout::Float4Color)
				shouldAdd = mesh.HasVertexColors(0);
			else if constexpr (elementType == VertexLayout::UV)
				shouldAdd = mesh.HasTextureCoords(0);
			else if constexpr (elementType == VertexLayout::BoneIds || elementType == VertexLayout::BoneWeights)
				shouldAdd = mesh.HasBones();

			if (shouldAdd)
			{
				VertexLayout layout;
				layout.AppendElement(elementType);

				auto nThBuffer = std::make_shared<VertexBuffer>(std::move(layout), mesh);
				auto pBuffer = IVertexBuffer::Resolve(gfx, nThBuffer);

				buffer.Append(std::move(pBuffer), semanticName.data());
			}
		}

		// Recurse to next element
		if constexpr (N < SIZE - 1)
		{
			InsertElement<SIZE, N + 1>(buffer, gfx, mesh);
		}
	}

	void ISOAVertexBuffer::InitFields(const IGraphics& gfx, const Microsoft::glTF::MeshPrimitive& primitive,
		const WinGLTFLoaderContext& context)
	{
		InsertElement<std::to_underlying(VertexLayout::VertexElement::Unknown)>(*this, gfx, primitive, context);
	}

	void ISOAVertexBuffer::InitFields(const IGraphics& gfx, const aiMesh& mesh)
	{
		InsertElement<std::to_underlying(VertexLayout::VertexElement::Unknown)>(*this, gfx, mesh);
	}

	void ISOAVertexBuffer::Append(const std::shared_ptr<IVertexBuffer>& vertexBuffer, const std::string& name, uint16_t slot)
	{
		if (pBuffers.contains(name))
		{
			trylog.warn(L"Cannot append the vertex buffer [" + utl::ToWide(name) + L"]");
		}
		else
		{
			dirty = true;
			AssertApiMatch(vertexBuffer);
			pBuffers[name] = std::pair{vertexBuffer, slot};
		}
	}

	void ISOAVertexBuffer::AppendFrom(const ISOAVertexBuffer& soaVertexBuffer, const std::string& name, uint16_t slot)
	{
		if (const auto it = soaVertexBuffer.pBuffers.find(name); it != soaVertexBuffer.pBuffers.end())
		{
			dirty = true;
			const auto& buffer = it->second.first;
			AssertApiMatch(buffer);
			pBuffers[name] = std::pair{ buffer, slot };
		}
		else
		{
			trylog.warn(L"Unable to find the vertex buffer [" + utl::ToWide(name) + L"]");
		}
	}

	void ISOAVertexBuffer::AppendFrom(const ISOAVertexBuffer& soaVertexBuffer, const VertexLayout& layout)
	{
		for (auto [slot, element] : layout.Elements | std::views::enumerate)
		{
			AppendFrom(soaVertexBuffer, element.Id() != "" ? element.Id() : element.GetName().data(), slot);
		}
	}
}
