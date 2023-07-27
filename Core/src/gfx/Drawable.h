#pragma once
#include "Mesh/Mesh.h"
#include "RenderQueue/Technique.h"
#include <core/src/gfx/Bindables/TransformCBuf.h>

namespace tryn::gfx
{
	class Drawable
	{
	public:
		Drawable() = default;
		Drawable(Drawable&& moveFrom) noexcept;
		Drawable(const Drawable& copyFrom) = delete;
		Drawable operator=(const Drawable& copyFrom) = delete;
		virtual ~Drawable() = default;
		void Draw(IGraphics& gfx);
		void InitTransformCBuf(IGraphics& gfx);
		Mesh& GetMesh() const;
		IPolyVBuffer& GetVertexBuffer() const;
		uint32_t GetIndexCount() const;
		void AddTechnique(Technique technique);
		glm::mat4 GetTransformMatrix() const;
		void BindParent();
		void SetYaw(float yaw);
		void SetPitch(float pitch);
		void SetRoll(float roll);
		float& GetYaw();
		float& GetPitch();
		float& GetRoll();
	public:
		// Position and orientation in world space
		glm::vec3 angles{};
		glm::vec3 pos = { 0.0f,0.0f,0.0f };
	protected:
		std::shared_ptr<Mesh> pMesh;
		std::unique_ptr<ITransformCBuf> pTransformCBuf;
		std::vector<Technique> techniques;
	};
}