#include "PointLight.h"
#include <Core/src/gfx/Model/StaticMeshPool.h>
#include "Bindables/ConstantBuffer.h"

namespace tryn::gfx
{
	PointLight::PointLight(IGraphics& gfx, float radius, glm::vec3 color)
	{
		// Model
		pModel = std::make_unique<Model>(gfx, "resources/models/sphere.fbx", glm::vec3{0.5f,0.5f,0.5f});

		parameters.ambient = glm::vec3(0.01f, 0.01f, 0.01f);
		parameters.viewLightPos = glm::vec3(0.0f, 0.0f, 0.0f);
		parameters.diffuseColor = glm::normalize(color);
		parameters.constantAtt = 1.0f;
		parameters.linearAtt = 0.045f;
		parameters.quadraticAtt = 0.0075f;
		parameters.diffuseIntensity = 1.0f;

		if (pCBuf == nullptr)
		{
			ConstantBufferLayout cblayout;
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float3,	"viewLightPos"));
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Float3,	"ambient"));
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
		ImGui::Begin("PointLight Parameters");
		ImGui::ColorEdit3("Diffuse Color", reinterpret_cast<float*>(&parameters.diffuseColor));
		ImGui::ColorEdit3("Ambient Color", reinterpret_cast<float*>(&parameters.ambient));
		ImGui::SliderFloat("Diffuse Intensity", &parameters.diffuseIntensity, 0.0f, 1.0f);
		ImGui::SliderFloat("Constant Attenuation", &parameters.constantAtt, 0.0f, 1.0f);
		ImGui::SliderFloat("Linear Attenuation", &parameters.linearAtt, 0.0f, 1.0f);
		ImGui::SliderFloat("Quadratic Attenuation", &parameters.quadraticAtt, 0.0f, 1.0f);
		ImGui::End();
	}
	void PointLight::Reset()
	{
		parameters.ambient = glm::vec3(0.01f, 0.01f, 0.01f);
		parameters.viewLightPos = glm::vec3(0.0f, 0.0f, 0.0f);
		parameters.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
		parameters.diffuseIntensity = 1.0f;
		parameters.constantAtt = 1.0f;
		parameters.linearAtt = 0.045f;
		parameters.quadraticAtt = 0.0075f;
	}
	void PointLight::Draw() const
	{
		pModel->Draw();
	}
	void PointLight::Bind() const
	{
		(*pCBuf)["viewLightPos"].Get<glm::vec3>() = pModel->GetPosition();
		(*pCBuf)["ambient"].Get<glm::vec3>() = parameters.ambient;
		(*pCBuf)["diffuseColor"].Get<glm::vec3>() = parameters.diffuseColor;
		(*pCBuf)["diffuseIntensity"].Get<float>() = parameters.diffuseIntensity;
		(*pCBuf)["constantAtt"].Get<float>() = parameters.constantAtt;
		(*pCBuf)["linearAtt"].Get<float>() = parameters.linearAtt;
		(*pCBuf)["quadraticAtt"].Get<float>() = parameters.quadraticAtt;

		pCBuf->Bind();
	}
	Model& PointLight::GetModel()
	{
		return *pModel;
	}

	std::shared_ptr<IPxConstantBuffer> PointLight::pCBuf;
}