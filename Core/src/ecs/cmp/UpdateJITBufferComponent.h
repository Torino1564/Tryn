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
			std::shared_ptr<gfx::JITUpdateBuffer> pJITBuffer = nullptr;
			void(*pFunc)(const std::shared_ptr<gfx::JITUpdateBuffer>& pBuffer, uint32_t entityID, const Archetype* pArchetype) = nullptr;
		};
	}
}