#pragma once
namespace tryn
{
	namespace gfx
	{
		class IJITUpdateBuffer;
	}

	namespace ecs
	{
		class Entity;
		class Archetype;

		struct UpdateJITBufferComponent
		{
			using Func_t = void(*)(const std::shared_ptr<gfx::IJITUpdateBuffer>& pBuffer, uint32_t entityID, const Archetype* pArchetype);

			std::vector<std::pair<
				std::shared_ptr<gfx::IJITUpdateBuffer>,
				Func_t>> jitCombinations;
		};
	}
}