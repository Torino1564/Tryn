#pragma once

namespace tryn::gbl
{
	struct Configs
	{
		bool singleThreadedRendeder = false;
		int numRenderWorkers = 3;
	};
	static Configs configs;
}