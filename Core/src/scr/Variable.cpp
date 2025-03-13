#include "TrynPCH.h"
#include "Variable.h"
#include <Core/src/ser/StreamIO.h>

#include "TypeRegister.h"

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
		scr::TypeRegister* pTypeRegister = nullptr;
		pExtraData->Get("pTypeRegister").Get((void*&)pTypeRegister);
		sr.ReadSerialized(data.name, binary, pExtraData);
		sr.ReadSerialized(data.typeName, binary, pExtraData);
		sr.ReadSerialized(data.uuid, binary, pExtraData);

		pTypeRegister->ConstructAny(data.var, data.uuid);
	}
}
