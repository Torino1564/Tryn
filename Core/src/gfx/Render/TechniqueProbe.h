#pragma once
#include <Core/src/utl/Probe.h>

namespace tryn::gfx
{
	class TechniqueBase;
	class Step;

	class TechniqueProbe : public utl::IProbe
	{
	public:
		virtual ~TechniqueProbe() = default;
		void SetTechnique(TechniqueBase* technique)
		{
			pTechnique = technique;
			OnSetTechnique();
		}
		void SetStep(Step* step)
		{
			pStep = step;
			OnSetStep();
		}
		virtual bool VisitBuffer(class ConstantBuffer&) = 0;
	protected:
		virtual void OnSetTechnique() {}
		virtual void OnSetStep() {}

		TechniqueBase* pTechnique;
		Step* pStep;
	};
}