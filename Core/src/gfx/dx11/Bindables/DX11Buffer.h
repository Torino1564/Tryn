#pragma once
#include <Core/src/gfx/Bindables/IBuffer.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <concepts>
#include <Core/src/utl/Assert.h>
#include <Core/src/gfx/Bindables/ConstantBuffer.h>

namespace tryn::gfx::dx11
{
	template <BufferType Type>
	constexpr int GetBindFlag() {}

	template <>
	constexpr int GetBindFlag<BufferType::Vertex>() { return D3D11_BIND_VERTEX_BUFFER; }

	template <BufferType Type, CachingPolicy Policy = CachingPolicy::Caching>
	class DX11Buffer : public IBuffer<Type, Policy>
	{
	public:
		DX11Buffer(Graphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer, std::string tag = "?")
			requires (Type == BufferType::Vertex) && (Policy == CachingPolicy::Caching)
			:
			gfx(gfx)
		{
			this->tag = tag;

			trynass_msg(!cpuBuffer->Dirty(), L"Cant initialize a dirty Vertex Buffer!");

			this->pCPUBuffer = cpuBuffer;

			stride = cpuBuffer->Stride();
			offset = 0u;

			D3D11_BUFFER_DESC bd = {};
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.BindFlags = GetBindFlag<Type>();
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.StructureByteStride = (UINT)stride;
			bd.ByteWidth = (UINT)cpuBuffer->Size();

			D3D11_SUBRESOURCE_DATA srd = {};
			srd.pSysMem = cpuBuffer->Data();

			gfx.GetDevice().CreateBuffer(&bd, &srd, &pBuffer) >> chk;
		}

		DX11Buffer(Graphics& gfx, ConstantBufferLayout&& cbl, int slot, std::string tag = "?")
		requires ((Type == BufferType::PxConstant || Type == BufferType::VtxConstant) && (Policy == CachingPolicy::Caching))
		: gfx(gfx)
		{
			this->slot = slot;
			this->tag = tag;

			trynass_msg(cbl.IsSolid(), L"ConstantBuffer cannot be created with a non solidified layout!");
			this->type = GraphicAPI::DX11;
			this->layout = std::move(cbl);
			this->pCPUBuffer = std::make_shared<FlatBuffer>(this->layout.Size());

			D3D11_BUFFER_DESC cbd = {};
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.StructureByteStride = 0u;
			cbd.ByteWidth = (UINT)this->layout.Size();
			D3D11_SUBRESOURCE_DATA csrd = {};
			csrd.pSysMem = this->pCPUBuffer->Data();

			gfx.GetDevice().CreateBuffer(&cbd, &csrd, &pBuffer) >> chk;
		}

		void Bind() override
		{
			Bind(gfx.GetContextInterface());
		}

		void Bind(IContext& context) override
		{
			gfx.AssertContextCoherence(context);
			Bind_(static_cast<DX11Context*>(&context)->GetContext());
		}

		ID3D11Buffer* Data() const
		{
			return pBuffer.Get();
		}

		void Update()
		{
			Update(gfx.GetContextInterface());
		}

		void Update(ID3D11DeviceContext& context)
		{
			D3D11_MAPPED_SUBRESOURCE msr;

			context.Map(
				Data(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			) >> chk;

			memcpy(msr.pData, this->pCPUBuffer->Data(), this->pCPUBuffer->Size());

			stride = this->pCPUBuffer->Stride();

			context.Unmap(Data(), 0u);
		}
	private:
		void Bind_(ID3D11DeviceContext& context)
			requires (Type == BufferType::Vertex)
		{
			context.IASetVertexBuffers((UINT)0, (UINT)1, pBuffer.GetAddressOf(), &stride, &offset);
		}

		void Bind_(ID3D11DeviceContext& context)
			requires (Type == BufferType::PxConstant || Type == BufferType::VtxConstant)
		{
			if (this->pCPUBuffer->Dirty())
			{
				Update(context);
				this->pCPUBuffer->SetClean();
			}
			if constexpr (Type == BufferType::PxConstant)
			{
				context.PSSetConstantBuffers(this->slot, 1u, pBuffer.GetAddressOf());
			}
			else
			{
				context.VSSetConstantBuffers(this->slot, 1u, pBuffer.GetAddressOf());
			}
		}

		Graphics& gfx;
		//Memory
		UINT stride = 0;
		UINT offset = 0;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
	};

	using DX11VtxConstBufCach = DX11Buffer<BufferType::VtxConstant, CachingPolicy::Caching>;
	using DX11VtxConstBufNCach = DX11Buffer<BufferType::VtxConstant, CachingPolicy::NonCaching>;
	using DX11PxConstBufCach = DX11Buffer<BufferType::PxConstant, CachingPolicy::Caching>;
	using DX11PxConstBufNCach = DX11Buffer<BufferType::PxConstant, CachingPolicy::NonCaching>;
	using DX11VertexBuffer = DX11Buffer<BufferType::Vertex, CachingPolicy::Caching>;

}