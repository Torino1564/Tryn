//#include "DX11PolyVBuffer.h"
//#include <Core/src/gfx/BindablePool.h>
//#include <Core/src/gfx/dx11/Bindables/DX11InputLayout.h>
//#include <Core/src/gfx/dx11/Bindables/DX11VertexBuffer.h>
//#include <Core/src/gfx/dx11/GraphicsError.h>
//
//namespace tryn::gfx::dx11
//{
//	DX11PolyVBuffer::DX11PolyVBuffer(Graphics& gfx, BufferArray& CpuVBs, std::string tag)
//		:
//		gfx(gfx)
//	{
//		this->tag = tag;
//		type = GraphicAPI::DX11;
//		for (auto& buffer : CpuVBs)
//		{
//			if (std::holds_alternative<std::shared_ptr<IVertexBuffer>>(buffer))
//			{
//				decltype(auto) ptr = std::get<std::shared_ptr<IVertexBuffer>>(buffer);
//				Append(ptr);
//			}
//			else if (std::holds_alternative<std::pair<std::string, std::shared_ptr<VertexBuffer>>>(buffer))
//			{
//				auto [tag, vb] = std::get<std::pair<std::string, std::shared_ptr<VertexBuffer>>>(buffer);
//				Append(tag, vb);
//			}
//			else
//			{
//				auto& ptr = std::get<std::shared_ptr<gfx::IPolyVBuffer>>(buffer);
//				for (auto& buffer : ptr->slots)
//				{
//					Append(buffer);
//				}
//			}
//		}
//	}
//
//	DX11PolyVBuffer::DX11PolyVBuffer(Graphics& gfx, std::shared_ptr<IPolyVBuffer> pvb, std::string tag)
//		:
//		gfx(gfx)
//	{
//		this->tag = tag;
//		type = GraphicAPI::DX11;
//		for (auto& vb : pvb->slots)
//		{
//			Append(vb);
//		}
//	}
//
//	void DX11PolyVBuffer::Append(std::string tag, std::shared_ptr<VertexBuffer> vb)
//	{
//		std::shared_ptr<IVertexBuffer> vertexBuffer = BindablePool::Resolve<IVertexBuffer>(gfx, vb, tag);
//		slots.push_back(vertexBuffer);
//	}
//
//	void DX11PolyVBuffer::Append(std::shared_ptr<IVertexBuffer> pVB)
//	{
//		slots.push_back(pVB);
//	}
//
//	void DX11PolyVBuffer::Bind()
//	{
//		if (!initialized) [[unlikely]]
//		{
//			for (auto& vBuffer : slots)
//			{
//				if (vBuffer->Get().Dirty())
//				{
//					vBuffer->Init();
//				}
//				strides.push_back((UINT)vBuffer->Get().Stride());
//				offsets.push_back((UINT)0);
//				auto dx11vb = std::dynamic_pointer_cast<DX11VertexBuffer>(vBuffer);
//				buffArray.push_back(dx11vb->GetPtr());
//			}
//			initialized = true;
//			gfx.GetContext().IASetVertexBuffers((UINT)0, (UINT)buffArray.size(), buffArray.data(), strides.data(), offsets.data());
//		}
//		else [[likely]] {
//			gfx.GetContext().IASetVertexBuffers((UINT)0, (UINT)buffArray.size(), buffArray.data(), strides.data(), offsets.data());
//		}
//	}
//	void DX11PolyVBuffer::Bind(IContext& context)
//	{
//		gfx.AssertContextCoherence(context);
//		auto& dx11context = static_cast<DX11Context*>(&context)->GetContext();
//		if (!initialized) [[unlikely]]
//			{
//				for (auto& vBuffer : slots)
//				{
//					if (vBuffer->Get().Dirty())
//					{
//						vBuffer->Init();
//					}
//					strides.push_back((UINT)vBuffer->Get().Stride());
//					offsets.push_back((UINT)0);
//					auto dx11vb = std::dynamic_pointer_cast<DX11VertexBuffer>(vBuffer);
//					buffArray.push_back(dx11vb->GetPtr());
//				}
//				initialized = true;
//				dx11context.IASetVertexBuffers((UINT)0, (UINT)buffArray.size(), buffArray.data(), strides.data(), offsets.data());
//			}
//		else [[likely]] {
//			dx11context.IASetVertexBuffers((UINT)0, (UINT)buffArray.size(), buffArray.data(), strides.data(), offsets.data());
//			}
//	}
//}