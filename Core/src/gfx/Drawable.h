#pragma once
#include "RenderQueue/Technique.h"
#include <core/src/gfx/Bindables/TransformCBuf.h>

struct aiMesh;

namespace tryn::gfx
{
	class Material;

	class Drawable
	{
	public:
		virtual ~Drawable() = default;

		void Draw(IGraphics& gfx);
		void InitTransformCBuf(IGraphics& gfx);
		[[nodiscard]] IVertexBuffer& GetVertexBuffer() const;
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

		std::shared_ptr<IVertexBuffer> pVertexBuffer;
		std::shared_ptr<IIndexBuffer> pIndexBuffer;
		std::shared_ptr<IPrimitiveTopology> pTopology;
		std::unique_ptr<ITransformCBuf> pTransformCBuf;
		uint32_t indexCount = 0;
		std::vector<Technique> techniques;
	};
}