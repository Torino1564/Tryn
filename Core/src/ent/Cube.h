#pragma once

#include "Entity.h"

namespace tryn::ent
{
	class Cube : public IEntity
	{
	public:
		Cube( float size );
		~Cube();
	};
}