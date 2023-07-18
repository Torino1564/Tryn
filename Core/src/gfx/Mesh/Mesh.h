#pragma once
#include <vector>
#include <Core/third/glm/glm.hpp>
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/Vertex.h>
#include <Core/src/gfx/IGraphics.h>

namespace tryn::gfx
{
	class Mesh
	{
	public:
		Mesh(std::vector<glm::vec3> vertices, std::vector<int> indices , std::string tag);
		void MakeBindables( gfx::IGraphics& gfx );
		std::vector<std::shared_ptr<gfx::IBindable>>& GetBindables();
		std::shared_ptr<gfx::VertexBuffer> GetBuffer();
		const std::shared_ptr<const std::vector<int>>& GetIndices() const;
		bool HasBindables() const;
		const std::string GetTag() const
		{
			return tag;
		}
	protected:
		std::vector<std::shared_ptr<gfx::IBindable>> bindables;
		std::shared_ptr<std::vector<int>> indices;
		std::shared_ptr<gfx::VertexBuffer> buffer;
		std::string tag;
	};
}