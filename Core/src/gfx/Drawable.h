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

		void Draw(IGraphics& gfx , glm::mat4 transform);
		void Submit(IGraphics& gfx, glm::mat4 transform);
		void Submit(IGraphics& gfx, std::span<const glm::mat4> transforms, class InstancedModelParent& parent);
		void BindBase() const;
		void BindBase(IContext& context) const;
		void BindExtraBinds();
		void BindExtraBinds(IContext& context);
		void AddExtraBind(IBindable* pBindable);
		void BindTransformCBuf() const;
		void BindTransformCBuf(IContext& context) const;
		void InitTransformCBuf(IGraphics& gfx);
		[[nodiscard]] IVertexBuffer& GetVertexBuffer() const;
		[[nodiscard]] uint32_t GetIndexCount() const;
		[[nodiscard]] glm::mat4 GetTransformMatrix() const;
		[[nodiscard]] std::uint16_t GetID() const;
		void SetMaterial(std::shared_ptr<class Material> pMat);
		class Material& GetMaterial();
		void SetTechniqueState(const std::string& name, const bool state);
		bool GetTechniqueState(const std::string& name) const;
	protected:
		virtual void inline ExtraSubmitBehavior() {}
		std::array<IBindable*, 10> extraBindPtrs;
		std::shared_ptr<IVertexBuffer> pVertexBuffer;
		std::shared_ptr<IIndexBuffer> pIndexBuffer;
		std::shared_ptr<IPrimitiveTopology> pTopology;
		std::unique_ptr<ITransformCBuf> pTransformCBuf;
		uint32_t indexCount = 0;
		std::uint16_t ID = 0;
		std::shared_ptr<class Material> pMaterial;
		std::vector<bool> techniqueState;
		glm::mat4 transform;
	};
}