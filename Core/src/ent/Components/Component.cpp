#include "Component.h"

std::string_view tryn::ent::Component::GetName()
{
	static constexpr const char* invalid = "?";
	return name.value_or(invalid);
}
