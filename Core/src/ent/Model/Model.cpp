#include "Model.h"

namespace tryn::ent
{
	Model::Model(std::vector<glm::vec3> vertices, std::vector<int> indices , std::string tag)
	{
		this->tag = tag;
		gfx::VertexLayout layout;

		layout.AppendElement(gfx::VertexLayout::VertexElement::Position3D);

		buffer = std::make_shared<gfx::VertexBuffer>(layout, vertices.size());
		this->indices = std::make_shared<std::vector<int>>(indices);
		
		for (int i = 0; i < vertices.size(); i++)
		{
			(*buffer)[i].Attr< gfx::VertexLayout::VertexElement::Position3D>() = vertices[i];
		}
	}
	void Model::MakeBindables(gfx::IGraphics& gfx)
	{
		gfx.MakeBindablesForModel(*this);
	}
	std::vector<std::shared_ptr<gfx::IBindable>>& Model::GetBindables()
	{
		return bindables;
	}
	std::shared_ptr<gfx::VertexBuffer> Model::GetBuffer()
	{
		return buffer;
	}
	const std::shared_ptr<const std::vector<int>>& Model::GetIndices() const
	{
		return std::const_pointer_cast<const std::vector<int>>(indices);
	}
	bool Model::HasBindables() const
	{
		return !bindables.empty();
	}
}


