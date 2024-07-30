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
		ISink& GetSink() const;

		ISource& GetSource() const;

	protected:
		// resources
		std::unique_ptr<ISink> pSink;
		std::unique_ptr<ISource> pSource;

		// identification
		std::string name;
		static inline const uint16_t ID = RenderPassID::Resolve();
	};
}