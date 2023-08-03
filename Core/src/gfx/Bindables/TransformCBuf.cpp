#include "TransformCBuf.h"
#include <Core/src/gfx/Drawable.h>

namespace tryn::gfx
{
	void ITransformCBuf::BindTransformCBuf(Drawable* parent)
	{
		BindParent(parent);
		Bind();
	}
	void ITransformCBuf::BindParent(Drawable* parent)
	{
		this->parent = parent;
	}
	void ITransformCBuf::Bind()
	{
		Update(parent);
		pVCB->Bind();
	}
	void ITransformCBuf::Update(Drawable* parent) const
	{
		const auto model = parent->GetTransformMatrix();
		const auto modelView = gfx.GetCameraMatrix() * model;
		const auto modelViewProj = gfx.GetProjectionMatrix() * modelView;

		(*pVCB)["model"].Get<glm::mat4>() = transpose(model);
		(*pVCB)["modelView"].Get<glm::mat4>() = transpose(modelView);
		(*pVCB)["modelViewProj"].Get<glm::mat4>() = transpose(modelViewProj);
	}
}