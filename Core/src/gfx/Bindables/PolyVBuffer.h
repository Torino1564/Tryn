#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <vector>
#include <memory>
#include "VertexBuffer.h"
#include <string>
#include "InputLayout.h"
#include <sstream>
#include <variant>

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
		virtual void Append(std::string , VertexBuffer&&) = 0;
		virtual void Append(std::shared_ptr<IVertexBuffer>) = 0;

		static std::string GenerateID(IGraphics& gfx, std::string tag,std::vector<std::variant<std::pair<std::string , VertexBuffer>, std::shared_ptr<IVertexBuffer>>>& CpuVBs)
		{
			decltype(auto) typeStr = IGraphics::GetAPIArray()[static_cast<int>(gfx.GetType())];
			std::stringstream ss;
			ss << typeStr << "#PolyVBuffer#" << std::to_string(CpuVBs.size()) << '#' << tag;

			for (auto& buffer : CpuVBs)
			{
				if (std::holds_alternative<std::shared_ptr<IVertexBuffer>>(buffer))
				{
					decltype(auto) ptr = std::get<std::shared_ptr<IVertexBuffer>>(buffer);
					ss << '$' << ptr->GetTag() << ptr->Get().Size();
					for (auto& [element,index] : ptr->Get().GetLayout().Elements)
					{
						ss << '#' << element.GetName() << std::to_string(index);
					}
				}
				else
				{
					const auto& [tag,vb] = std::get<std::pair<std::string, VertexBuffer>>(buffer);
					ss << '$' << tag << vb.Size();
					for (auto& [element, index] : vb.GetLayout().Elements)
					{
						ss << '#' << element.GetName() << std::to_string(index);
					}
				}
			}
		}
		std::string tag;
		std::vector<std::shared_ptr<IVertexBuffer>> slots;
	};
}