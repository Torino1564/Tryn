#pragma once

namespace tryn::utl
{
	[[noreturn]]
	void NoReturnImpl_();
}

#define no_return tryn::utl::NoReturnImpl_()