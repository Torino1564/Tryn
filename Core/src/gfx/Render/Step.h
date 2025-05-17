#pragma once
#include <Core/src/gfx/ConstantBuffer.h>
#include <span>

namespace tryn::gfx
{
	class VertexLayout;
	class ISOAVertexBuffer;
	class Drawable;
	class Mesh;
	class IGraphics;
	class IContext;

	class Step
	{
	public:
		Step(const std::string& renderQueueID);
		Step(Step&& rhs) noexcept;
		~Step();
		void AddBindable(std::shared_ptr<class IBindable> bindable);
		void Bind() const;
		void Bind(const IContext& context) const;
		static void Draw(const IGraphics& gfx, const Drawable* parent);
		void Submit(const IGraphics& gfx, Drawable* parent);
		void Submit(const IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, class InstancedModelParent& instanceParent);
		void OfferBindable(const std::string& identifier, const std::shared_ptr<IBindable>& pBindable);
		void FillSOAVertexBuffer(const ISOAVertexBuffer& pSOAVertexBuffer) const;
		const VertexLayout& GetVertexLayout() const;
		VertexLayout& GetVertexLayout() ;
		void Accept(class TechniqueProbe& probe);
		std::vector<std::shared_ptr<IBindable>> bindables;
		std::vector<std::shared_ptr<IBindable>> acceptedBindables;
		const std::string& RenderQueueID() const;

	private:
		std::unique_ptr<VertexLayout> pVertexLayout;
		std::shared_ptr<ISOAVertexBuffer> pSOAVertexBuffer;
		std::vector<std::string> bindablesToAccept;
		std::string renderQueueID;
	};
}
