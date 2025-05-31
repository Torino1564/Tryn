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

	bool Mesh::IsParentMesh() const
	{
		return isParentMesh;
	}

	const std::vector<std::shared_ptr<Mesh>>& Mesh::GetChildren() const
	{
		return children;
	}

	std::vector<std::shared_ptr<Mesh>>& Mesh::GetChildren()
	{
		return children;
	}
}
