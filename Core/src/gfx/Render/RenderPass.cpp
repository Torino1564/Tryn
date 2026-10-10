
#include <Core/src/gfx/Render/RenderPass.h>

namespace tryn::gfx
{
	IRenderPass::IRenderPass(std::string name): name(std::move(name))
	{
		pSink = std::make_unique<Sink>(this);
		pSource = std::make_unique<Source>(this);
		pSource->pPass = this;
	}

	IRenderPass::IRenderPass(IRenderPass&& rhs) noexcept
	{
		pSink = std::move(rhs.pSink);
		pSink->pPass = this;
		pSource = std::move(rhs.pSource);
		pSource->pPass = this;
		name = std::move(rhs.name);
	}

	const std::string& IRenderPass::GetName() const
	{
		return name;
	}

	uint16_t IRenderPass::RenderPassID::Resolve()
	{
		static uint16_t UIDcounter = 0;
		return UIDcounter++;
	}

	Sink& IRenderPass::GetSink() const
	{
		return *pSink;
	}

	Source& IRenderPass::GetSource() const
	{
		return *pSource;
	}
}
