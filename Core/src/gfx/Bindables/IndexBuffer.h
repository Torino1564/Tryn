#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <string_view>

namespace tryn::gfx
{
	class CPUBuffer;
	class IGraphics;
	class IndexBuffer;

	class IIndexBuffer : public IBindable
	{
	public:
		static std::shared_ptr<IIndexBuffer> Resolve(const IGraphics& gfx, std::shared_ptr<IndexBuffer> indices, const std::string& tag = "?");
		static std::string GenerateID(const IGraphics& gfx, std::shared_ptr<IndexBuffer> indices, const std::string& tag = "?");
		void Bind() override = 0;
		void Bind(const class IContext& context) override = 0;
		const IndexBuffer& GetIndexBuffer() const;
		IndexBuffer& GetIndexBuffer();
		std::string_view GetPath() const;
		std::string_view GetTag() const;

	protected:
		std::shared_ptr<CPUBuffer> pCPUBuffer;
		std::string path;
		std::string tag;
	};
}