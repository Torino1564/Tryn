#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/Vertex.h>
#include <Core/src/utl/String.h>
#include <vector>
#include <memory>
#include <sstream>

namespace tryn::gfx
{
	class IVertexBuffer : public IBindable
	{
	public:
		virtual std::vector<char> GetLayoutFromVB() const = 0;
		virtual std::vector<char> GetSlottedLayoutFromVB( int slot ) const = 0;
		static std::string GenerateID(IGraphics& gfx, VertexBuffer& cpuBuffer , std::string tag = "?")
		{
			if (tag == "?") return tag;
			decltype(auto) typeStr = IGraphics::GetAPIArray()[static_cast<int>(gfx.GetType())];
			std::stringstream ss;
			ss << typeStr << "#VertexBuffer#" << std::to_string(cpuBuffer.Size()) << "#";

			for (auto& element : cpuBuffer.GetLayout().Elements)
			{
				ss << element.first.GetName() << element.second;
			};
			ss << "#" << tag;

			return ss.str();
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