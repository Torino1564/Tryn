#pragma once
#include <stdint.h>
#include "SinkAndSource.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace tryn::gfx
{
	class IRenderPass
	{
	public:
		virtual ~IRenderPass() = default;
		IRenderPass(std::string name) : name(std::move(name)) {}
		IRenderPass(IRenderPass&& rhs) noexcept
		{
			pSink = std::move(rhs.pSink);
			pSource = std::move(rhs.pSource);
			name = std::move(rhs.name);
		}
		virtual void Execute(const class IGraphics& gfx) = 0;
		const std::string& GetName() const
		{
			return name;
		}
		struct RenderPassID
		{
			static uint16_t Resolve()
			{
				static uint16_t UIDcounter = 0;
				return UIDcounter++;
			}
		};
		ISink& GetSink()
		{
			return *pSink;
		}
		ISource& GetSource()
		{
			return *pSource;
		}
	protected:
		// resources
		std::unique_ptr<ISink> pSink;
		std::unique_ptr<ISource> pSource;

		// identification
		std::string name;
		static inline const uint16_t ID = RenderPassID::Resolve();
	};
}