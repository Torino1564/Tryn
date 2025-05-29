#include "TrynPCH.h"
#include "EntityIDTechnique.h"
#include <Core/src/gfx/IGraphics.h>

#include "Core/src/gfx/Vertex.h"
#include "Core/src/gfx/Bindables/InputLayout.h"
#include "Core/src/gfx/Bindables/JITUpdateBuffer.h"
#include "Core/src/gfx/Bindables/PixelShader.h"
#include "Core/src/gfx/Bindables/Rasterizer.h"
#include "Core/src/gfx/Bindables/VertexShader.h"

namespace tryn::gfx
{
	EntityIDTechnique::EntityIDTechnique(const std::string& name)
		: Technique(name)
	{
	}

	EntityIDTechnique::EntityIDTechnique(const std::vector<std::shared_ptr<Material>>& materials,
		const IGraphics& gfx, bool instanced, bool skinned)
			: Technique("EntityIDTechnique")
	{
		static const std::string shaderRootPath = IGraphics::GetShaderRootPath();
		std::string shaderCode = "EntityID";


		// Common

		Step step(gfx, "EntityID");
		auto& vLayout = step.GetVertexLayout();
		vLayout.AppendElement(VertexLayout::Position3D);
		gfx.GetRenderGraph().AddRenderQueue("EntityID");

		step.AddBindable(std::move(IRasterizer::Resolve(gfx)));


		auto pvs = IVertexShader::Resolve(gfx, shaderRootPath + "Flat_VS.cso");
		step.AddBindable(std::move(pvs));
		step.AddBindable(IPixelShader::Resolve(gfx, shaderRootPath + shaderCode + "_PS.cso"));

		AddStep(std::move(step));
	}
}
