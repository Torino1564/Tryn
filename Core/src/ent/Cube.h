#pragma once
#include <memory>
#include "Entity.h"

namespace tryn::ent
{
	class Cube : public IEntity
	{
	public:
		Cube( float size );
		const std::shared_ptr<Model> GetModel() const;
	private:
		inline static std::shared_ptr<Model> model;
		inline static bool isStaticInitialized = false;
	};
}