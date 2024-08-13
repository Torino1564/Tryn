#pragma once
#include "Render/Technique.h"
#include <core/src/gfx/Bindables/TransformCBuf.h>

struct aiMesh;

namespace tryn::gfx
{
	class Material;

	class Drawable
	{
	public:
		virtual ~Drawable() = default;

		void Draw(const IGraphics& gfx , glm::mat4 transform);
		void Submit(const IGraphics& gfx, glm::mat4 transform);
		void Submit(const IGraphics& gfx, std::span<const glm::mat4> transforms, class InstancedModelParent& parent);
		void BindBase() const;
		void BindBase(const IContext& context) const;
		void BindExtraBinds();
		void BindExtraBinds(const IContext& context);
		void AddExtraBind(IBindable* pBindable);
		void BindTransformCBuf() const;
		void BindTransformCBuf(const IContext& context) const;
		void InitTransformCBuf(const IGraphics& gfx);
		[[nodiscard]] IVertexBuffer& GetVertexBuffer() const;
		[[nodiscard]] uint32_t GetIndexCount() const;
		[[nodiscard]] glm::mat4 GetTransformMatrix() const;
		[[nodiscard]] std::uint16_t GetID() const;

	protected:
		Material& GetSelectedMaterial() const;
		virtual void inline ExtraSubmitBehavior() {}
		std::array<IBindable*, 10> extraBindPtrs;
		std::shared_ptr<IVertexBuffer> pVertexBuffer;
		std::shared_ptr<IIndexBuffer> pIndexBuffer;
		std::shared_ptr<IPrimitiveTopology> pTopology;
		std::unique_ptr<ITransformCBuf> pTransformCBuf;
		uint32_t indexCount = 0;
		std::uint16_t ID = 0;
		std::vector<std::unique_ptr<class Material>> pMaterials;
		unsigned int selectedMaterial = 0;
		glm::mat4 transform = {};
	};
}