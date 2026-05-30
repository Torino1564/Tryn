#pragma once
#include "Core/src/gfx/Vertex.h"
#include <Core/src/gfx/Bindables/Bindable.h>

namespace tryn::gfx
{
	class IGraphics;
	class IVertexShader;

	// This class encapsulates a SOA vertex buffer.
	class ISOAVertexBuffer : public IBindable
	{
	public:
		static std::shared_ptr<ISOAVertexBuffer> Resolve(const IGraphics& gfx);
		static std::string GenerateID(const IGraphics& gfx);

		//void InitFields(const IGraphics& gfx, const Microsoft::glTF::MeshPrimitive& primitive, const WinGLTFLoaderContext& context);
		void InitFields(const IGraphics& gfx, const aiMesh& mesh);
		void Append(const std::shared_ptr<IVertexBuffer>& vertexBuffer, const std::string& name, uint16_t slot = 0);
		void AppendFrom(const ISOAVertexBuffer& soaVertexBuffer, const std::string& name, uint16_t slot = 0);
		void AppendFrom(const ISOAVertexBuffer& soaVertexBuffer, const VertexLayout& layout);
		virtual void SetVertexShader(const std::shared_ptr<IVertexShader>& pVS) = 0;
	protected:
		virtual void AssertApiMatch(const std::shared_ptr<IVertexBuffer>& pVB) = 0;
		std::unordered_map<std::string, std::pair<std::shared_ptr<IVertexBuffer>, uint16_t>> pBuffers = {};
		bool dirty = true;
	};
}