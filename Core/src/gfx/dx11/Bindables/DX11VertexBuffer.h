#include <Core/src/gfx/Bindables/VertexBuffer.h>

namespace tryn::gfx::dx11
{
	class DX11VertexBuffer : public IVertexBuffer
	{
	public:
		DX11VertexBuffer(const Graphics& gfx, const std::shared_ptr<VertexBuffer>& cpuBuffer, const std::string& tag = "?");
		void Bind() override;
		void Bind(const IContext& context) override;
		std::vector<std::any> GetLayoutFromVB() const override;
		std::vector<std::any> GetSlottedLayoutFromVB(int slot) const override;
		void Resize(const std::size_t newSize) override;
		ID3D11Buffer* Data() const;
		void Update();
		void Update(ID3D11DeviceContext& context);

	private:
		void Bind_(ID3D11DeviceContext& context) override;

	private:
		Graphics& gfx;
		UINT stride = 0;
		UINT offset = 0;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
	}
}