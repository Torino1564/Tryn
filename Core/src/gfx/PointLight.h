#pragma once
#include "IGraphics.h"
#include "Mesh/StaticMesh.h"
#include "BindablePool.h"

namespace tryn::gfx
{
	class PointLight
	{
		PointLight(IGraphics& gfx, float radius = 0.5f);
		void ShowControls();
		void Reset();
		void Draw() const;
		void Bind() const;
	private:
		glm::vec3 position = { 0.0f,0.0f,0.0f };
		std::shared_ptr<StaticMesh> mesh;
		std::shared_ptr<IConstantBuffer> cbuf;
	};
}