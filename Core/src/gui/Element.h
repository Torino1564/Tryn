#pragma once
#include <utility>
#include <Core/src/spa/Dimensions.h>
#include <Core/src/spa/Vec2.h>
#include <vector>
#include <memory>

namespace tryn::gui
{
	class IBehaviourComponent;
	class Element
	{
	private:
		// Components
		std::vector<std::unique_ptr<IBehaviourComponent>> componentPtrs;
		spa::Vec2I position;
		spa::DimensionsF dimensions;
	};
}