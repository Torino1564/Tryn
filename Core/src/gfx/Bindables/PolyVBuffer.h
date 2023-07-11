#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <vector>
#include <memory>
#include "VertexBuffer.h"
#include "InputLayout.h"

namespace tryn::gfx
{
	class IPolyVBuffer : public IBindable
	{
	public:
		virtual ~IPolyVBuffer() {};
		size_t Size() const
		{
			return slots.size();
		}
		virtual void Append(std::shared_ptr<VertexBuffer>) = 0;

		std::vector<std::shared_ptr<IVertexBuffer>> slots;
	};
}