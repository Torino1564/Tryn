#pragma once
#include <Core/src/gfx/IGraphics.h>
#include <string>
#include "TechniqueProbe.h"
#include "Step.h"

namespace tryn::gfx
{
	enum class Techniques
	{
		Phong,
		Flat,
		Garaoud,
	};

	class Technique
	{
	public:
		Technique(std::string name);
		void AddStep(Step step);
		void Draw(IGraphics& gfx, Drawable* parent);
		void Submit(IGraphics& gfx, Drawable* parent);
		void Submit(IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, class InstancedModelParent& instancedParent);
		void Accept(TechniqueProbe& probe);
		void SetStepState(const std::string& name, bool state);
		bool GetStepState(const std::string& name) const;
		virtual bool IsSkinned() const;
		virtual bool IsInstanced() const;
		const std::string& GetName() const;
	protected:

		class VertexLayout& ExtractLayoutFromMaterial(class Material& mat);
		const std::string& GetShaderRootPath() const;
		std::string name;
		bool skinned;
		bool instanced;
		std::vector<Step> steps;
		std::vector<bool> stepActive;
	};
}
