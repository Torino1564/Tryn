#include "Bindable.h"

tryn::gfx::IBindable::~IBindable()
{}

void tryn::gfx::IBindable::Bind( const IContext&)
{ Bind(); }

void tryn::gfx::IBindable::Accept(TechniqueProbe&)
{}

tryn::gfx::GraphicAPI tryn::gfx::IBindable::GetAPI() const
{
	return type;
}
