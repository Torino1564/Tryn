#include "IContext.h"

namespace tryn::gfx
{
	IVtxConstantBuffer& IContext::GetTransfromBuffer() const
	{
		return *pTCB;
	}
	void IContext::SetDeferred(bool setting)
	{
		deferred = setting;
	}
	bool IContext::IsDeferred(void)
	{
		return deferred;
	}
}


