#include "TransformCBuf.h"
#include <Core/src/gfx/Drawable.h>

namespace tryn::gfx
{
	void ITransformCBuf::BindParent(Drawable& parent)
	{
		this->parent = &parent;
	}
	void ITransformCBuf::Bind()
	{
		Update();
		pVCB->Bind();
	}
	void ITransformCBuf::Update() const
	{
		const auto model = parent->GetTransformMatrix();
		const auto modelView = model * gfx.GetCameraMatrix();
		const auto modelViewProj = modelView * gfx.GetProjectionMatrix();

		(*pVCB)["model"].Get<glm::mat4>() = transpose(model);
		(*pVCB)["modelView"].Get<glm::mat4>() = transpose(modelView);
		(*pVCB)["modelViewProj"].Get<glm::mat4>() = transpose(modelViewProj);
	}
}