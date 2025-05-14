#pragma once
#include "Core/src/gfx/Vertex.h"

namespace tryn::gfx
{
	// This class encapsulates the idea of a SOA vertex buffer.
	// Binding this class to the pipeline ALSO binds the input layout
	// for the compound vertex buffer. 
	class ISOAVertexBuffer : public IBindable
	{
	public:
		static std::shared_ptr<ISOAVertexBuffer> Resolve(const IGraphics& gfx, const std::shared_ptr<IVertexShader>& pVS);
		
		void Append(const std::shared_ptr<IVertexBuffer>& vertexBuffer, const std::string& name, uint16_t slot);
		void AppendFrom(const ISOAVertexBuffer& soaVertexBuffer, const std::string& name, uint16_t slot);

	protected:
		virtual void AssertApiMatch(const std::shared_ptr<IVertexBuffer>& pVB) = 0;
		std::unordered_map<std::string, std::pair<std::shared_ptr<IVertexBuffer>, uint16_t>> pBuffers;
		bool dirty = true;
	};
}