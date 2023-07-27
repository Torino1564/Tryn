#pragma once
#include <vector>
#include <Core/third/glm/gtx/euler_angles.hpp>
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/Vertex.h>
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/Bindables/PolyVBuffer.h>
#include <Core/src/gfx/Bindables/IndexBuffer.h>
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>

namespace tryn::gfx
{
	class Mesh
	{
	public:
		~Mesh() {}
		virtual void MakeBindables( gfx::IGraphics& gfx ) = 0;
		bool HasBindables() const
		{
			return hasBinds;
		}
		const std::string GetTag() const
		{
			return tag;
		}
		virtual bool IsStatic() const = 0;
		const auto GetPolyVBufer() const
		{
			return pVertexBuffer;
		}
		const auto GetIndexBuffer() const
		{
			return pIndexBuffer;
		}
		void Bind( IGraphics& gfx )
		{
			trynass_msg(hasBinds, L"Attempted to bind a mesh with no bindings!");

			pVertexBuffer->Bind();
			pIndexBuffer->Bind();
			pTopology->Bind();
		}
		const int GetIndexCount() const
		{
			return indexCount;
		}
	protected:
		std::shared_ptr<IPolyVBuffer> pVertexBuffer;
		std::shared_ptr<IIndexBuffer> pIndexBuffer;
		std::shared_ptr<IPrimitiveTopology> pTopology;
		std::shared_ptr<std::vector<int>> pCpuIndexData;
		std::shared_ptr<std::vector<gfx::VertexBuffer>> pCpuVertexData;
		std::string tag;
		int indexCount = 0;
		bool hasBinds = false;
	};
}