#pragma once
#include "Mesh/Mesh.h"
#include "RenderQueue/Technique.h"
#include "Mesh/StaticMeshPool.h"
#include <core/src/gfx/Bindables/TransformCBuf.h>

namespace tryn::gfx
{
	class Drawable
	{
	public:
		void Draw(IGraphics& gfx)
		{
			pMesh->Bind(gfx);
			pTransformCBuf->Bind();
			for (auto& technique : techniques)
			{
				technique.Draw(gfx);
			}
		}
		void InitTransformCBuf(IGraphics& gfx)
		{
			pTransformCBuf = gfx.CreateTransformCBuf();
			pTransformCBuf->BindParent(*this);
		}
		auto& GetMesh()
		{
			return *pMesh;
		}
		const auto& GetVertexBuffer() const
		{
			return *pMesh->GetPolyVBufer();
		}
		uint32_t GetIndexCount() const
		{
			return pMesh->GetIndexCount();
		}
		void AddTechnique(Technique technique)
		{
			techniques.push_back(std::move(technique));
		}
		glm::mat4 GetTransformMatrix() const
		{
			return glm::translate(glm::yawPitchRoll(yaw, pitch, roll), pos);
		}
	public:
		// Position and orientation in world space
		float yaw = 0.0f;
		float pitch = 0.0f;
		float roll = 0.0f;
		glm::vec3 pos = { 0.0f,0.0f,0.0f };
	protected:
		std::shared_ptr<Mesh> pMesh;
		std::unique_ptr<ITransformCBuf> pTransformCBuf;
		std::vector<Technique> techniques;
	};
}