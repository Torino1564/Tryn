#include "TrynPCH.h"
#include "Variable.h"
#include <Core/src/ser/StreamIO.h>

#include <Core/src/gph/TTypeRegister.h>

namespace tryn::ser
{
	void SerializeWrite(const StreamWriter& sw, const scr::Variable& data, const bool binary, const std::string& name)
	{
		sw.Serialize(data.name, binary);
		sw.Serialize(data.typeName, binary);
		sw.Serialize(data.uuid, binary);
	}

	void SerializeRead(const StreamReader& sr, scr::Variable& data, const bool binary, ExtraDataPack* pExtraData)
	{
		gph::TTypeRegister* pTypeRegister = nullptr;
		pExtraData->Get("pTypeRegister").Get((void*&)pTypeRegister);
		sr.ReadSerialized(data.name, binary, pExtraData);
		sr.ReadSerialized(data.typeName, binary, pExtraData);
		sr.ReadSerialized(data.uuid, binary, pExtraData);

		pTypeRegister->Construct(data.var, data.uuid);
	}
}
