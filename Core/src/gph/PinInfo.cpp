#include "TrynPCH.h"
#include "PinInfo.h"
#include <Core/src/ser/StreamIO.h>

namespace tryn::ser
{
	void SerializeWrite(const StreamWriter& sw, const gph::PinInfo& data, const bool binary, const std::string& name)
	{
		sw.Serialize(data.parentId, binary);
		sw.Serialize(data.linkedId, binary);
		sw.Serialize(data.name, binary);
		sw.Serialize(data.kind, binary);
		sw.Serialize(data.id, binary);
		sw.Serialize(data.linked, binary);
	}

	void SerializeRead(const StreamReader& sr, gph::PinInfo& data, const bool binary, ExtraDataPack* pExtraData)
	{
		sr.ReadSerialized(data.parentId, binary, pExtraData);
		sr.ReadSerialized(data.linkedId, binary, pExtraData);
		sr.ReadSerialized(data.name, binary, pExtraData);
		sr.ReadSerialized(data.kind, binary, pExtraData);
		data.id = sr.ReadSerialized<unsigned long long>(binary, pExtraData);
		sr.ReadSerialized(data.linked, binary, pExtraData);
	}
}
