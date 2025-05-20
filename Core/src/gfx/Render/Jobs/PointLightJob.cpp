#include "TrynPCH.h"
#include "PointLightJob.h"
#include <Core/src/gfx/Render/RenderGraph.h>
#include <Core/src/gfx/Bindables/IBufferBase.h>
#include <Core/src/gfx/Camera.h>

namespace tryn::gfx
{
	PointLightJob::PointLightJob(const PointLightParameters& parameters, const glm::vec3& position, IRenderGraph& renderGraph, std::uint16_t jobID)
		:
		IJob(jobID), pParams(&parameters), pRenderGraph(&renderGraph), pPosition(&position) {}

	void PointLightJob::Execute(const IGraphics& gfx)
	{
		ExecuteImpl_();
	}
	void PointLightJob::Execute(const IContext& ctx)
	{
		ExecuteImpl_();
	}
	void PointLightJob::ExecuteImpl_() const
	{
		// Use JobID as the number in the buffer
		auto& cbuff = pRenderGraph->pPointLightCBuf->GetConstantBuffer();
		auto params = cbuff["pointLightArray"][jobID];
		auto& viewMatrix = pRenderGraph->pCameras[pRenderGraph->selectedCamera]->GetViewOnViewMatrix();

		params["position"].Get<glm::vec4>() = viewMatrix * glm::vec4(*pPosition, 1.0f);
		params["ambient"].Get<glm::vec3>() = pParams->ambient;
		params["diffuseColor"].Get<glm::vec3>() = pParams->diffuseColor;
		params["diffuseIntensity"].Get<float>() = pParams->diffuseIntensity;
		params["constantAtt"].Get<float>() = pParams->constantAtt;
		params["linearAtt"].Get<float>() = pParams->linearAtt;
		params["quadraticAtt"].Get<float>() = pParams->quadraticAtt;
	}
}


