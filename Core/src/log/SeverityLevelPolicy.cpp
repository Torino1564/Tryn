#include "SeverityLevelPolicy.h"
#include "Entry.h"

tryn::log::SeverityLevelPolicy::SeverityLevelPolicy(Level level)
	:
	level_{ level }
{}

bool tryn::log::SeverityLevelPolicy::TransformFilter(Entry& e)
{
	return e.level_ <= level_;
}