#pragma once

#include "Entity.h"

namespace tryn::ent
{
	class Cube : public IEntity
	{
		Cube( int size );
		~Cube();
	};
}