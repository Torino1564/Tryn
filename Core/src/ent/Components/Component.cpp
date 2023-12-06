#include "Component.h"

std::string_view tryn::ent::Component::GetName()
{
	return name.value_or("?");
}
