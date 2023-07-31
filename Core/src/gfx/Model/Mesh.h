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
		virtual ~Mesh() = default;

		[[nodiscard]] bool HasBindables() const
		{
			return hasBinds;
		}

		[[nodiscard]] std::string GetTag() const
		{
			return tag;
		}

		[[nodiscard]] virtual bool IsStatic() const = 0;

		[[nodiscard]] auto GetPolyVBuffer() const
		{
			return pVertexBuffer;
		}

		[[nodiscard]] auto GetIndexBuffer() const
		{
			return pIndexBuffer;
		}
		void Bind( IGraphics& gfx ) const
		{
			trynass_msg(hasBinds, L"Attempted to bind a mesh with no bindings!");

			pVertexBuffer->Bind();
			pIndexBuffer->Bind();
			pTopology->Bind();
		}

		[[nodiscard]] uint32_t GetIndexCount() const
		{
			return indexCount;
		}
	protected:
		std::shared_ptr<IPolyVBuffer> pVertexBuffer;
		std::shared_ptr<IIndexBuffer> pIndexBuffer;
		std::shared_ptr<IPrimitiveTopology> pTopology;
		std::string tag;
		uint32_t indexCount = 0;
		bool hasBinds = false;
	};
}
