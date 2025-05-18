#include "TrynPCH.h"
#include "Mesh.h"

namespace tryn::gfx
{
	Mesh::Mesh()
		: Drawable()
	{
	}

	std::string Mesh::GetTag() const
	{
		return tag;
	}
}
