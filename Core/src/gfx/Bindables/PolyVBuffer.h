//#pragma once
//#include <Core/src/gfx/Bindables/Bindable.h>
//#include <vector>
//#include <memory>
//#include "VertexBuffer.h"
//#include <string>
//#include <sstream>
//#include <variant>
//#include <Core/src/gfx/BindablePool.h>
//
//namespace tryn::gfx
//{
//	typedef std::vector<std::variant<std::pair<std::string, std::shared_ptr<tryn::gfx::VertexBuffer>>, std::shared_ptr<tryn::gfx::IVertexBuffer>, std::shared_ptr<tryn::gfx::IPolyVBuffer>>> BufferArray;
//
//	class IPolyVBuffer : public IBindable
//	{
//	public:
//		virtual ~IPolyVBuffer() {};
//		size_t Size() const
//		{
//			return slots.size();
//		}
//		virtual void Append(std::string, std::shared_ptr<VertexBuffer>) = 0;
//		virtual void Append(std::shared_ptr<IVertexBuffer>) = 0;
//
//		static std::shared_ptr<IPolyVBuffer> Resolve(const IGraphics& gfx, BufferArray& cpuVBs, std::string tag = "?")
//		{
//			return BindablePool::Resolve<IPolyVBuffer>(gfx, cpuVBs, tag);
//		}
//
//		static std::string GenerateID(const IGraphics& gfx, BufferArray& cpuVBs, std::string tag)
//		{
//			if (tag == "?") return tag;
//			decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
//			std::stringstream ss;
//			ss << typeStr << "#PolyVBuffer#" << std::to_string(cpuVBs.size()) << '#' << tag;
//
//			for (auto& buffer : cpuVBs)
//			{
//				if (std::holds_alternative<std::shared_ptr<IVertexBuffer>>(buffer))
//				{
//					decltype(auto) ptr = std::get<std::shared_ptr<IVertexBuffer>>(buffer);
//					ss << '$' << ptr->GetTag();
//					for (auto& [element, index] : ptr->Get().GetLayout().Elements)
//					{
//						ss << '#' << element.GetName() << std::to_string(index);
//					}
//				}
//				else if (std::holds_alternative<std::shared_ptr<IPolyVBuffer>>(buffer))
//				{
//					decltype(auto) ptr = std::get<std::shared_ptr<gfx::IPolyVBuffer>>(buffer);
//					ss << "$PVB:" << ptr->GetTag() << "{";
//					for (auto& element : ptr->slots)
//					{
//						ss << '$' << ptr->GetTag();
//						for (auto& [element, index] : element->Get().GetLayout().Elements)
//						{
//							ss << '#' << element.GetName() << std::to_string(index);
//						}
//					}
//					ss << "}";
//				}
//				else
//				{
//					const auto& [tag, vb] = std::get<std::pair<std::string, std::shared_ptr<VertexBuffer>>>(buffer);
//					ss << '$' << tag;
//					for (auto& [element, index] : vb->GetLayout().Elements)
//					{
//						ss << '#' << element.GetName() << std::to_string(index);
//					}
//				}
//			}
//
//			return ss.str();
//		}
//
//		std::string GetTag() const
//		{
//			return tag;
//		}
//
//		std::string tag;
//		std::vector<std::shared_ptr<IVertexBuffer>> slots;
//	};
//}