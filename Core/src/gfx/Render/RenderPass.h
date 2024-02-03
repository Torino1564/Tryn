#pragma once
#include <stdint.h>
#include "SinkAndSource.h"
#include <memory>
#include <vector>

namespace tryn::gfx
{
	class IRenderPass
	{
		public:
			virtual void Execute() = 0;

		struct RenderPassID
		{
		private:
			static uint16_t Resolve()
			{
				static uint16_t UIDcounter = 0;
				return UIDcounter++;
			}
		};
	};

	template <typename T>
	class BaseRenderPass : public IRenderPass
	{
	public:
		void Execute() override
		{
			T::Execute();
		}
	private:
		std::vector<std::unique_ptr<ISink>> pSinks;
		std::vector<std::unique_ptr<ISource>> pSources;

		static inline const uint16_t ID = RenderPassID::Resolve();
	};
}