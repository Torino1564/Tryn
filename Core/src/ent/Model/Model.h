#pragma once
#include <vector>
#include <Core/third/glm/glm.hpp>
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/Vertex.h>
#include <Core/src/gfx/IGraphics.h>

namespace tryn::ent
{
	class Model
	{
	public:
		Model(std::vector<glm::vec3> vertices, std::vector<int> indices);
		void MakeBindables( gfx::IGraphics& gfx );
		std::vector<std::unique_ptr<gfx::IBindable>>& GetBindables();
		std::shared_ptr<gfx::VertexBuffer> GetBuffer();
		const std::shared_ptr<const std::vector<int>>& GetIndices() const;
		bool HasBindables() const;
	private:
		std::vector<std::unique_ptr<gfx::IBindable>> bindables;
		std::shared_ptr<std::vector<int>> indices;
		std::shared_ptr<gfx::VertexBuffer> buffer;
	};
}