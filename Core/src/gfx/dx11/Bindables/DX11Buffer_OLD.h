#pragma once
#include <Core/src/gfx/Bindables/IBufferBase.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/ConstantBuffer.h>


namespace tryn::gfx::dx11
{
	using cbType = ConstantBufferLayout::Type;
	template <BufferType Type>
	constexpr int GetBindFlag() { return 0;  }

	template <>
	constexpr int GetBindFlag<BufferType::Vertex>() { return D3D11_BIND_VERTEX_BUFFER; }

	template <>
	constexpr int GetBindFlag<BufferType::Index>() { return D3D11_BIND_INDEX_BUFFER; }

	template <BufferType Type, CachingPolicy Policy = CachingPolicy::Caching>
	class DX11Buffer : public IBufferBase<Type, Policy>
	{
	public:
		DX11Buffer(const Graphics& gfx, const std::shared_ptr<CPUBuffer>& pCpuBuffer, const std::string& tag = "?")
			requires (Type == BufferType::Vertex || Type == BufferType::Index);

		// TODO: Add NonCaching variant
		DX11Buffer(const Graphics& gfx, const ConstantBufferLayout& cbl, int slot, const std::string& tag = "?")
			requires (Type == BufferType::PxConstant || Type == BufferType::VtxConstant);
		DX11Buffer(const Graphics& gfx, const ConstantBufferLayout::Node& arrayElement, int slot, std::size_t numInstances = 50)
			requires (Type == BufferType::Instance);
		void Resize(std::size_t newSize) override;
		void Resize_(std::size_t newSize);
		void InitDynamicCBufferOnGPU();
		void Bind() override;
		void Bind(const IContext& context) override;
		ID3D11Buffer* Data() const;
		void Update();
		void Update(ID3D11DeviceContext& context);
		std::vector<std::any> GetSlottedLayoutFromVB(int slot) const override;

	private:
		std::string_view Test() const override;
		void GPUSizeChanges() requires (Type == BufferType::Instance);
		void GPUSizeChanges() requires (Type != BufferType::Instance);
		void Bind_(ID3D11DeviceContext& context)
			requires (Type == BufferType::Vertex);
		void Bind_(ID3D11DeviceContext& context)
			requires (Type == BufferType::Index);
		void Bind_(ID3D11DeviceContext& context)
			requires (Type == BufferType::PxConstant || Type == BufferType::VtxConstant || Type == BufferType::Instance);
		std::vector<std::any> GetLayoutFromVB() const override;
		std::vector<std::any> GetSlottedLayoutFromVB_(int slot) const
			requires (Type == BufferType::Vertex);
		static std::vector<std::any> GetSlottedLayoutFromVB_(int slot);

		const Graphics& gfx;

		//Memory
		UINT stride = 0;
		UINT offset = 0;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
	};
}
