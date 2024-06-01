#pragma once
#include <Core/src/gfx/IGraphics.h>
#include <string>
#include "TechniqueProbe.h"
#include "Step.h"

namespace tryn::gfx
{
	template <typename OriginalInstanciation, bool NewParam1, bool NewParam2> struct ReplaceTemplateParam;

	template <template<bool, bool> class T, bool OldParam1, bool OldParam2, bool NewParam1, bool NewParam2>
	struct ReplaceTemplateParam<T<OldParam1, OldParam2>, NewParam1, NewParam2> {
		using type = T<NewParam1, NewParam2>;
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
		void Draw(IGraphics& gfx, Drawable* parent);
		void Submit(IGraphics& gfx, Drawable* parent);
		void Submit(IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, class InstancedModelParent& instancedParent);
		void Accept(TechniqueProbe& probe);

	protected:
		class VertexLayout& ExtractLayoutFromMaterial(class Material& mat);
		const std::string& GetShaderRootPath() const;
		std::string name;
		std::vector<Step> steps;
	};
}
