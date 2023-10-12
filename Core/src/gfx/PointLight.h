#pragma once
#include "IGraphics.h"
#include "Model/Model.h"

namespace tryn::gfx
{
	class PointLight
	{
	public:
		PointLight(IGraphics& gfx, float radius = 0.5f, glm::vec3 color = { 1.0f,1.0f,1.0f });
		void ShowControls();
		void Reset();
		void Draw() const;
		void Bind(const glm::mat4 view) const;
		Model& GetModel();
	public:
		struct Parameters
		{
			glm::vec3 viewLightPos;
			glm::vec3 ambient;
			glm::vec3 diffuseColor;
			float diffuseIntensity;
			float constantAtt;
			float linearAtt;
			float quadraticAtt;
		};
	private:
		Parameters parameters = {};
		glm::vec3 position = { 0.0f,0.0f,0.0f };
		std::unique_ptr<Model> pModel;
		static std::shared_ptr<IPxConstantBuffer> pCBuf;
	};
}