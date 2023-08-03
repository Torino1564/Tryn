#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/Vertex.h>
#include <Core/src/utl/String.h>
#include <vector>
#include <memory>
#include <sstream>
#include <Core/src/gfx/BindablePool.h>
#include <any>

namespace tryn::gfx
{
	class IVertexBuffer : public IBindable
	{
	public:
		virtual std::vector<std::any> GetLayoutFromVB() const = 0;
		virtual std::vector<std::any> GetSlottedLayoutFromVB(int slot) const = 0;
		static std::shared_ptr<IVertexBuffer> Resolve(IGraphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer, std::string tag = "?")
		{
			return BindablePool::Resolve<IVertexBuffer>(gfx, cpuBuffer, tag);
		}
		static std::string GenerateID(IGraphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer, std::string tag = "?")
		{
			if (tag == "?") return tag;
			decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
			std::stringstream ss;
			ss << typeStr << "#VertexBuffer#" << std::to_string(cpuBuffer->Size()) << "#";

			for (auto& element : cpuBuffer->GetLayout().Elements)
			{
				ss << element.first.GetName() << element.second;
			};
			ss << "#" << tag;

			return ss.str();
		}
		virtual void Init() = 0;
		virtual ~IVertexBuffer() {}
		VertexBuffer& Get() const
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
		std::shared_ptr<VertexBuffer> CPUBuffer;
		std::string tag;
	};
}