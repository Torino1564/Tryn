#pragma once
#include <stdint.h>
#include "SinkAndSource.h"
#include <memory>
#include <string>

namespace tryn::gfx
{
	class IRenderPass
	{
	public:
		virtual ~IRenderPass() = default;
		IRenderPass(std::string name);
		IRenderPass(IRenderPass&& rhs) noexcept;
		virtual void Execute(const class IGraphics& gfx) = 0;
		const std::string& GetName() const;

		struct RenderPassID
		{
			static uint16_t Resolve();
		};
		Sink& GetSink() const;

		Source& GetSource() const;

	protected:
		// resources
		std::unique_ptr<Sink> pSink;
		std::unique_ptr<Source> pSource;

		// identification
		std::string name;
		static inline const uint16_t ID = RenderPassID::Resolve();
	};
}