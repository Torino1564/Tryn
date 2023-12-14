#pragma once
#include <memory>
#include <Core/src/gfx/Model/Model.h>
#include <Core/third/dynamic_bitset.hpp>
#include <bitset>
#include <array>

namespace tryn::ent
{
	class IEntity
	{
	public:
		virtual ~IEntity();
		IEntity();
		void SpawnControlWindow();
		void Mark();
		int GenerateID();

	protected:
		std::string name;
		int UID = -1;
		// Components
		// Entity ID
		static sul::dynamic_bitset<> IDbooker;
	};

	class BasicEntity : public IEntity
	{
	public:

		BasicEntity(gfx::IGraphics& gfx, std::string_view name, std::string_view path, glm::vec3 scale = { 1.0f,1.0f,1.0f });
		BasicEntity(const BasicEntity&) = delete;
		BasicEntity& operator=(const BasicEntity&) = delete;

		BasicEntity(BasicEntity&&) = default;
		BasicEntity& operator=(BasicEntity&&) = default;
	};
}