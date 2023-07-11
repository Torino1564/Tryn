#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/Vertex.h>
#include <Core/src/utl/String.h>
#include <vector>
#include <memory>

namespace tryn::gfx
{
	class IVertexBuffer : public IBindable
	{
	public:
		virtual std::vector<char> GetLayoutFromVB() const = 0;
		virtual std::vector<char> GetSlottedLayoutFromVB( int slot ) const = 0;
		static std::string GenerateID(IGraphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer)
		{
			std::string UID = toString(gfx.GetType()) + "#VertexBuffer";
			for (auto& element : cpuBuffer->GetLayout().Elements)
			{
				UID += element.first.GetName();
				UID += element.second;
			};
			return UID;
		}
		virtual void Init() = 0;
		virtual ~IVertexBuffer() {}
		VertexBuffer& Get()
		{
			return *pCPUBuffer;
		}
		const VertexBuffer& ConstGet() const
		{
			return *pCPUBuffer.get();
		}
		Vertex operator[](int i)
		{
			return Get()[i];
		}
	protected:
		std::shared_ptr<VertexBuffer> pCPUBuffer;
	};
}