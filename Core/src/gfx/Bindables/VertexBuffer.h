#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>

namespace tryn::gfx {

	class IVertexBuffer : public IBindable
	{
	public:
		static std::shared_ptr<IVertexBuffer> Resolve(const IGraphics& gfx, const std::shared_ptr<VertexBuffer>& cpuBuffer, const std::string& tag = "?");
		static std::string GenerateID(const IGraphics& gfx, const std::shared_ptr<VertexBuffer>& cpuBuffer, const std::string& tag = "?");

		void Bind() override = 0;
		void Bind(const IContext& context) override = 0;
		virtual std::vector<std::any> GetLayoutFromVB() const = 0;
		virtual std::vector<std::any> GetSlottedLayoutFromVB(int slot) const = 0;
		virtual void Resize(const std::size_t newSize);
		const VertexLayout& GetLayout() const;
		virtual CPUBuffer& GetCPUBuffer() = 0;
		const VertexBuffer& GetVertexBuffer() const;
		VertexBuffer& GetVertexBuffer() const;
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