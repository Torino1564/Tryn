#pragma once
#include "Model.h"

namespace tryn::gfx
{
	class MainInstancedModel : public Model
	{
	public:
		void SubmitInstances();
	private:
		std::string instancedGroup;
		std::vector<std::unique_ptr<InstancedModel>> pInstances;
	};

	class InstancedModel
	{
	public:
		Settings settings;
		int instanceID;
		MainInstancedModel& pMainModel;
	};
}