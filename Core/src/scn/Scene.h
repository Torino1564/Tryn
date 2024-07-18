#pragma once
#include <vector>
#include <Core/src/ecs/Entity.h>
#include <Core/src/ecs/cmp/ComponentManager.h>
#include <Core/src/ecs/sys/SystemManager.h>
#include <Core/src/ser/StreamIO.h>

namespace tryn::scn
{
	class Scene
	{
	private:
		std::vector<std::unique_ptr<ecs::Entity>> entities;

	public:
		ZT_DEFINE_SERIALIZER(Scene)
		{
			static void Write(const ser::StreamWriter& streamWriter, const Scene& data, const bool binary = true, const std::string& name = "")
			{
				streamWriter.Serialize(data.entities, binary, name);
			}
			static Scene Read(const ser::StreamReader& streamReader, const bool binary = true)
			{
				Scene newScene;
				newScene.entities = streamReader.ReadSerialized(newScene.entities, binary);

				return newScene;
			}
		};
	};
}