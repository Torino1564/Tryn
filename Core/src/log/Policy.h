#pragma once
namespace tryn::log
{
	struct Entry;

	class IPolicy
	{
	public:
		virtual ~IPolicy() = default;
		virtual bool TransformFilter(Entry&) = 0;
	};
}