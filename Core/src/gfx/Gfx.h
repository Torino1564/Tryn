#pragma once
#include "IGraphics.h"
#include "Bindables/BindableBase.h"
#include "Texture.h"

#ifndef TRYN_NOT_INCLUDE_DX11
#include "dx11/Dx11Graphics.h"
#endif

#include "Bindables/BindableBase.h"
#include "Render/Techniques/Flat.h"
#include "Render/Techniques/ForwardPhong.h"
#include "Render/Jobs/IJob.h"
#include "Render/Passes/PointLightBindPass.h"
#include "Render/Passes/ForwardLambertianPass.h"
#include "Core/src/gfx/Render/Passes/ClearTargetPass.h"
#include "Core/src/gfx/Render/Passes/FullScreenRenderPass.h"
#include "Render/Technique.h"
#include "Render/RenderGraph.h"
#include "Render/DefaultRenderGraphs/DefaultRenderGraph.h"
#include "Render/TechniqueProbe.h"

#include "Model/Model.h"
#include "Model/InstancedModel.h"
#include "Model/StaticMesh.h"

#include "Animation/Animation.h"
#include "Animation/BonedMesh.h"

#include "Camera.h"
#include "Assimp.h"
#include "ImguiManager.h"
#include "PointLight.h"

#include "Profiler.h"

namespace tryn::gfx
{
	void Boot();
}