#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <any>
#include <vector>
#include <cstdint>

namespace tryn::gfx {
	class VertexBuffer;
	class VertexLayout;
	class IGraphics;
	class CPUBuffer;

	class IVertexBuffer : public IBindable
	{
	public:
		static std::shared_ptr<IVertexBuffer> Resolve(const IGraphics& gfx, const std::shared_ptr<VertexBuffer>& cpuBuffer, const std::string& tag = "?");
		static std::string GenerateID(const IGraphics& gfx, const std::shared_ptr<VertexBuffer>& cpuBuffer, const std::string& tag = "?");

		virtual std::vector<std::any> GetLayoutFromVB() const = 0;
		virtual std::vector<std::any> GetSlottedLayoutFromVB(int slot) const = 0;
		const VertexLayout& GetLayout() const;
		CPUBuffer& GetCPUBuffer();
		const VertexBuffer& GetVertexBuffer() const;
		VertexBuffer& GetVertexBuffer();
		std::string_view GetPath() const;
		std::string_view GetTag() const;

	protected:
		std::shared_ptr<CPUBuffer> pCPUBuffer;
		std::string path;
		std::string tag;
		uint16_t slot = 0;
		size_t gpuSize = 0;
	};

}