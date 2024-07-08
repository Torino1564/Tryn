#pragma once
#include <string>
#include "TechniqueProbe.h"
#include "Step.h"

namespace tryn::gfx
{
	template <typename OriginalInstanciation, bool NewParam1, bool NewParam2> struct ReplaceTemplateParam;

	template <template<bool, bool> class Tech, bool OldParam1, bool OldParam2, bool NewParam1, bool NewParam2>
	struct ReplaceTemplateParam<Tech<OldParam1, OldParam2>, NewParam1, NewParam2> {
		using type = Tech<NewParam1, NewParam2>;
	};

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
		void Draw(class IGraphics& gfx, Drawable* parent);
		void Submit(class IGraphics& gfx, Drawable* parent);
		void Submit(class IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, class InstancedModelParent& instancedParent);
		void Accept(class TechniqueProbe& probe);

	protected:
		class VertexLayout& ExtractLayoutFromMaterial(class Material& mat);
		std::string name;
		std::vector<Step> steps;
	};
}
