#pragma once

namespace tryn
{
	namespace gfx
	{
		class JITUpdateBuffer;
	}

	namespace ecs
	{
		class Entity;

		struct UpdateJITBufferComponent
		{
			using Func_t = void(*)(const std::shared_ptr<gfx::JITUpdateBuffer>& pBuffer, uint32_t entityID, const Archetype* pArchetype);

			std::vector<std::pair<
				std::shared_ptr<gfx::JITUpdateBuffer>,
				Func_t>> jitCombinations;
		};
	}
}