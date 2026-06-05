#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/ConstantBuffer.h>

namespace tryn::gfx
{
	class IInstanceBuffer : public IBindable
	{
	public:
		static std::shared_ptr<IInstanceBuffer> Resolve(const IGraphics& gfx, const ConstantBufferLayout::Node& node, int slot, const std::size_t num_instances);
		static std::string GenerateID(const IGraphics& gfx, const ConstantBufferLayout::Node& node, int slot, const std::size_t num_instances);
		virtual void Resize(const std::size_t newSize) = 0;
		std::string_view GetPath() const;
		std::string_view GetTag() const;
		ConstantBuffer& GetConstantBuffer();
		const ConstantBuffer& GetConstantBuffer() const;

	protected:
		std::shared_ptr<CPUBuffer> pCPUBuffer;
		std::string path;
		std::string tag;
		uint16_t slot = 0;
		size_t gpuSize = 0;
	};
}