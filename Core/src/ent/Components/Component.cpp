#include "Component.h"

void tryn::ent::Component::SetEntityID(int newID)
{
	this->entityUID = newID;
}

std::string_view tryn::ent::Component::GetName()
{
	static constexpr const char* invalid = "?";
	return name.value_or(invalid);
}
