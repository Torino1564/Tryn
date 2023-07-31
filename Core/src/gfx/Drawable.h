// ReSharper disable CppInconsistentNaming
#pragma once
#include "Model/Mesh.h"
#include "RenderQueue/Technique.h"
#include <core/src/gfx/Bindables/TransformCBuf.h>

namespace tryn::gfx
{
	class Material;
	struct aiMesh;

	class Drawable
	{
	public:
		Drawable(IGraphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f);

		void Draw(IGraphics& gfx);
		void InitTransformCBuf(IGraphics& gfx);
		[[nodiscard]] Mesh& GetMesh() const;
		[[nodiscard]] IPolyVBuffer& GetVertexBuffer() const;
		[[nodiscard]] uint32_t GetIndexCount() const;
		void AddTechnique(Technique technique);
		[[nodiscard]] glm::mat4 GetTransformMatrix() const;
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