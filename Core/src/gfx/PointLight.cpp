
#include "PointLight.h"
#include <Core/src/gfx/Bindables/IBufferBase.h>
#include <Core/src/gfx/Render/Techniques/Flat.h>
#include "Model/Model.h"
#include <Core/src/gfx/ImguiManager.h>

namespace tryn::gfx
{
	PointLight::PointLight(const IGraphics& gfx, float radius, glm::vec3 color)
		:
		gfx(gfx)
	{
		pModel = Model::Make<Flat>(gfx, "Game/Resources/Models/sphere.obj", glm::vec3{ 0.1f,0.1f,0.1f });

		parameters.viewLightPos = glm::vec3(0.0f, 3.0f, 0.0f);
		parameters.diffuseColor = glm::normalize(color);
		parameters.constantAtt = 1.0f;
		parameters.linearAtt = 0.045f;
		parameters.quadraticAtt = 0.0075f;
		parameters.diffuseIntensity = 1.0f;

		if (pCBuf == nullptr)
		{
			ConstantBufferLayout cblayout;
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float3,	"viewLightPos"));
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float3,	"diffuseColor"));
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float,	"diffuseIntensity"));
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float,	"constantAtt"));
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float,	"linearAtt")); 
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float,	"quadraticAtt"));
			cblayout.Solidify();

			pCBuf = IPxConstantBuffer::Resolve(gfx, std::move(cblayout), 0);
		}
	}
	void PointLight::ShowControls()
	{
		if(ImGui::Begin("PointLight Parameters"))
		{
			ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&position), -20.0f, 20.0f);
			ImGui::ColorEdit3("Diffuse Color", reinterpret_cast<float*>(&parameters.diffuseColor));
			ImGui::SliderFloat("Diffuse Intensity", &parameters.diffuseIntensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Constant Attenuation", &parameters.constantAtt, 0.0f, 1.0f);
			ImGui::SliderFloat("Linear Attenuation", &parameters.linearAtt, 0.0f, 1.0f);
			ImGui::SliderFloat("Quadratic Attenuation", &parameters.quadraticAtt, 0.0f, 1.0f);
		}
		ImGui::End();
	}
	void PointLight::Reset()
	{
		// Do something here
		position = { 0.0f,0.0f,0.0f };
		parameters.viewLightPos = glm::vec3(0.0f, 0.0f, 0.0f);
		parameters.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
		parameters.diffuseIntensity = 1.0f;
		parameters.constantAtt = 1.0f;
		parameters.linearAtt = 0.045f;
		parameters.quadraticAtt = 0.0075f;
	}

	void PointLight::Bind() const
	{
		pCBuf->Bind();
	}
	void PointLight::Bind(IContext& context) const
	{
		pCBuf->Bind(context);
	}
	void PointLight::SubmitLight(const IGraphics& gfx)
	{
		gfx.GetRenderGraph().AddPointLight(this);
	}
	void PointLight::Submit(const IGraphics& gfx, const glm::mat4 view)
	{
		(*pCBuf)["viewLightPos"].Get<glm::vec3>() = view * glm::vec4(position + GetModel().GetPosition(), 1.0f);
		(*pCBuf)["diffuseColor"].Get<glm::vec3>() = parameters.diffuseColor;
		(*pCBuf)["diffuseIntensity"].Get<float>() = parameters.diffuseIntensity;
		(*pCBuf)["constantAtt"].Get<float>() = parameters.constantAtt;
		(*pCBuf)["linearAtt"].Get<float>() = parameters.linearAtt;
		(*pCBuf)["quadraticAtt"].Get<float>() = parameters.quadraticAtt;

		transformation = glm::translate(glm::mat4(1.0f), position);
		pModel->Submit(transformation);
	}
	Model& PointLight::GetModel()
	{
		return *pModel;
	}

	void BindPointLightTask::operator()()
	{
		params.pLight->Bind(*params.pContext);
	}

	std::shared_ptr<IPxConstantBuffer> PointLight::pCBuf;
}