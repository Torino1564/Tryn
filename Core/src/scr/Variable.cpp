#include "TrynPCH.h"
#include "Variable.h"
#include <Core/src/ser/StreamIO.h>

#include <Core/src/gph/TTypeRegister.h>

namespace tryn::ser
{
	void Serialize(StreamIO& io, scr::Variable* data, const bool binary, const std::string& name)
	{
		gph::TTypeRegister* pTypeRegister ;
		io.GetAndFill("typeRegister", reinterpret_cast<void*&>(pTypeRegister));
		
		io.Field(&data->name, binary);
		io.Field(&data->typeName, binary);
		io.Field(&data->uuid, binary);
		
		io.Exclusive<StreamIO::Reader>(
			[&]()
			{
				pTypeRegister->ConstructAt(data->var, data->uuid);
			}
		);
	}
}
