#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/Vertex.h>
#include <vector>
#include <memory>

namespace tryn::gfx
{
	class IVertexBuffer : public IBindable
	{
	public:
		virtual std::vector<char> GetLayoutFromVB() const = 0;
		virtual std::vector<char> GetSlottedLayoutFromVB( int slot ) const = 0;

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
		virtual void BindSlotted(int slot , int buffCount) = 0;
	protected:
		std::shared_ptr<VertexBuffer> pCPUBuffer;
	};
}