#include "TransformCBuf.h"
#include <Core/src/gfx/Drawable.h>
#include <Core/src/gfx/Bindables/IBuffer.h>

namespace tryn::gfx
{
	void ITransformCBuf::BindTransformCBuf(const Drawable* parent)
	{
		BindParent(parent);
		Bind();
	}
	void ITransformCBuf::BindTransformCBuf(const Drawable* parent, const IContext& context)
	{
		BindParent(parent);
		Bind(context);
	}
	void ITransformCBuf::BindParent(const Drawable* parent) const
	{
		this->parent = parent;
	}
	void ITransformCBuf::Bind()
	{
		Update();
		pVCB->Bind();
	}
	void ITransformCBuf::Bind(const IContext& context)
	{
		Update(context);
		context.GetTransfromBuffer().Bind(context);
	}
	void ITransformCBuf::Update() const
	{
		(*pVCB)["model"].Get<glm::mat4>() = transpose(parent->GetTransformMatrix());
		const auto& viewMatrix = gfx.GetCameraMatrix();
		(*pVCB)["view"].Get<glm::mat4>() = transpose(viewMatrix);
		(*pVCB)["viewProjection"].Get<glm::mat4>() = transpose(gfx.GetProjectionMatrix() * viewMatrix);
	}
	void ITransformCBuf::Update(const IContext& context) const
	{
		context.GetTransfromBuffer()["model"].Get<glm::mat4>() = transpose(parent->GetTransformMatrix());
		const auto& viewMatrix = gfx.GetCameraMatrix();
		context.GetTransfromBuffer()["view"].Get<glm::mat4>() = transpose(viewMatrix);
		context.GetTransfromBuffer()["viewProjection"].Get<glm::mat4>() = transpose(gfx.GetProjectionMatrix() * viewMatrix);
	}
}