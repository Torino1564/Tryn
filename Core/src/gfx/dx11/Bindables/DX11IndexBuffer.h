#include <Core/src/gfx/Bindables/IndexBuffer.h>

namespace tryn::gfx::dx11
{
	class DX11IndexBuffer : public IIndexBuffer
	{
	public:
		DX11IndexBuffer(const Graphics& gfx, std::shared_ptr<IndexBuffer> indices, const std::string& tag = "?");
		void Bind() override;
		void Bind(const IContext& context) override;
		ID3D11Buffer* Data() const;
		void Update();
		void Update(ID3D11DeviceContext& context);
		void Resize(const std::size_t newSize) override;
	private:
		void Bind_(ID3D11DeviceContext& context) override;
	private:
		const Graphics& gfx;
		UINT stride = 0;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
	};
}