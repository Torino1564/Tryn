#include "TrynPCH.h"
#include "PinInfo.h"
#include <Core/src/ser/StreamIO.h>

namespace tryn::ser
{
	void Serialize(StreamIO& sw, gph::PinInfo* data, const bool binary, const std::string& name)
	{
		sw.Field(&data->parentId, binary);
		sw.Field(&data->linkedId, binary);
		sw.Field(&data->name, binary);
		sw.Field(&data->kind, binary);
		sw.Field(&data->id, binary);
		sw.Field(&data->linked, binary);
	}
}
