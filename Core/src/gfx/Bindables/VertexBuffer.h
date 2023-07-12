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
		static std::string GenerateID(IGraphics& gfx, VertexBuffer& cpuBuffer , std::string tag = "?")
		{
			decltype(auto) typeStr = IGraphics::GetAPIArray()[static_cast<int>(gfx.GetType())];
			std::string UID(typeStr);
			UID += "#VertexBuffer#";
			UID += std::to_string(cpuBuffer.Size());
			UID += "#";
			for (auto& element : cpuBuffer.GetLayout().Elements)
			{
				UID += element.first.GetName();
				UID += element.second;
			};
			UID.append("#");
			UID.append(tag);

			return UID;
		}
		virtual void Init() = 0;
		virtual ~IVertexBuffer() {}
		VertexBuffer& Get()
		{
			return *CPUBuffer;
		}
		const VertexBuffer& ConstGet() const
		{
			return *CPUBuffer;
		}
		Vertex operator[](int i)
		{
			return Get()[i];
		}
		const std::string& GetTag() const
		{
			return tag;
		}
	protected:
		std::unique_ptr<VertexBuffer> CPUBuffer;
		std::string tag;
	};
}