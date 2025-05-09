#pragma once
#include "Render/Technique.h"
#include <core/src/gfx/Bindables/TransformCBuf.h>

#include "Core/third/glm/ext/matrix_transform.hpp"

struct aiMesh;

namespace tryn::gfx
{
	class Material;

	class Drawable
	{
	public:
		virtual ~Drawable() = default;

		void Draw(const IGraphics& gfx , const glm::mat4& transform);
		virtual void Submit(const IGraphics& gfx, const glm::mat4& transform);
		virtual void Submit(const IGraphics& gfx, std::span<const glm::mat4> transforms, InstancedModelParent& parent);
		void BindBase() const;
		void BindBase(const IContext& context) const;
		void BindExtraBinds();
		void BindExtraBinds(const IContext& context);
		void AddExtraBind(IBindable* pBindable);
		void AddTechniqueStepExtraBind(IBindable* pBindable, utl::UUID_t techniqueUUID, const std::string& step);
		void BindTransformCBuf() const;
		void BindTransformCBuf(const IContext& context) const;
		void InitTransformCBuf(const IGraphics& gfx);
		[[nodiscard]] IVertexBuffer& GetVertexBuffer() const;
		[[nodiscard]] uint32_t GetIndexCount() const;
		[[nodiscard]] glm::mat4 GetTransformMatrix() const;
		[[nodiscard]] std::uint16_t GetID() const;
		void DisableTechnique(utl::UUID_t techniqueUUID);
		Material& GetSelectedMaterial() const;
		virtual void ExtraSubmitBehavior() {}
		std::array<IBindable*, 10> extraBindPtrs;
		std::shared_ptr<IVertexBuffer> pVertexBuffer;
		std::shared_ptr<IIndexBuffer> pIndexBuffer;
		std::shared_ptr<IPrimitiveTopology> pTopology;
		std::unique_ptr<ITransformCBuf> pTransformCBuf;
		uint32_t indexCount = 0;
		std::uint16_t ID = 0;
		std::vector<std::shared_ptr<Material>> pMaterials;
		std::vector<std::pair<bool, std::shared_ptr<TechniqueBase>>> pTechniques;
		unsigned int selectedMaterial = 0;
		glm::mat4 transform = glm::identity<glm::mat4>();
		bool instanced = false;

		virtual void EnableOrAddTechniqueEx(const IGraphics& gfx, utl::UUID_t techniqueUUID, bool skinned, std::span<uint16_t> materialIndex = { std::array{(uint16_t)0}.begin(), 1 });
		virtual void AddTechniqueEx(const IGraphics& gfx, utl::UUID_t techniqueUUID, bool skinned, std::span<uint16_t> materialIndex = { std::array{(uint16_t)0}.begin(), 1 }, bool enabled = true);
	};
}