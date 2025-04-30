#pragma once

namespace tryn::fsx
{
	class IBoundingBody
	{
	public:
		virtual ~IBoundingBody() = default;
		virtual bool Intersects(const IBoundingBody& rhs) = 0;
	};
}