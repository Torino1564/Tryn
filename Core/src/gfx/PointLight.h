#pragma once
#include "IGraphics.h"
#include <Core/src/gfx/PointLightParameters.h>

namespace tryn::gfx
{
	class Model;
	class IContext;
	class PointLight
	{
	public:
		PointLight(const IGraphics& gfx, float radius = 0.5f, glm::vec3 color = { 1.0f,1.0f,1.0f });
		void ShowControls();
		void Reset();
		void Bind() const;
		void Bind(IContext&) const;
		void SubmitLight(const IGraphics& gfx);
		void Submit(const IGraphics& gfx, const glm::mat4 view);
		Model& GetModel();
	public:
		
	private:
		const IGraphics& gfx;
		PointLightParameters parameters = {};
		glm::vec3 position = { 0.0f,0.0f,0.0f };
		glm::mat4 transformation = {};
		std::unique_ptr<Model> pModel;
		static std::shared_ptr<IPxConstantBuffer> pCBuf;
	};

	class BindPointLightTask
	{
	public:
		void operator()();

		struct Data {
			PointLight* pLight;
			IContext* pContext;
		} params = {};
	};
}