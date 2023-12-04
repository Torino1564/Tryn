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
	void ITransformCBuf::BindTransformCBuf(const Drawable* parent, IContext& context)
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
	void ITransformCBuf::Bind(IContext& context)
	{
		Update(context);
		context.GetTransfromBuffer().Bind(context);
	}
	void ITransformCBuf::Update() const
	{
		const auto model = parent->GetTransformMatrix();
		const auto modelView = gfx.GetCameraMatrix() * model;
		const auto modelViewProj = gfx.GetProjectionMatrix() * modelView;

		(*pVCB)["model"].Get<glm::mat4>() = transpose(model);
		(*pVCB)["modelView"].Get<glm::mat4>() = transpose(modelView);
		(*pVCB)["modelViewProj"].Get<glm::mat4>() = transpose(modelViewProj);
	}
	void ITransformCBuf::Update(IContext& context) const
	{
		const auto model = parent->GetTransformMatrix();
		const auto modelView = gfx.GetCameraMatrix() * model;
		const auto modelViewProj = gfx.GetProjectionMatrix() * modelView;

		context.GetTransfromBuffer()["model"].Get<glm::mat4>() = transpose(model);
		context.GetTransfromBuffer()["modelView"].Get<glm::mat4>() = transpose(modelView);
		context.GetTransfromBuffer()["modelViewProj"].Get<glm::mat4>() = transpose(modelViewProj);
	}
}