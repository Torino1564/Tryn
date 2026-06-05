#include <Core/src/gfc/Bindables/ConstantBufferResource.h>

namespace tryn::gfx::dx11
{
	class DX11ConstantBufferResource : public IConstantBufferResource
	{
	public:
		DX11ConstantBufferResource(const Graphics& gfx, const ConstantBufferLayout& cbl, Type type, int slot, const std::string& tag = "?");
		void Bind() override;
		void Bind(const IContext& context) override;
		ID3D11Buffer* Data() const;
		void Update();
		void Update(ID3D11DeviceContext& context);
		void InitDynamicCBufferOnGPU();
		void Resize(std::size_t newSize) override;
	private:
		void Bind_(ID3D11DeviceContext& context) override;
	private:
		const Graphics& gfx;
		Type type;
		int slot;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
	};
}