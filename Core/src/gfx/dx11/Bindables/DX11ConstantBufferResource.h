#include <Core/src/gfx/Bindables/ConstantBufferResource.h>
#include <wrl/client.h>

struct ID3D11Buffer;
struct ID3D11DeviceContext;

namespace tryn::gfx::dx11
{
	class Graphics;

	class DX11ConstantBufferResource : public IConstantBufferResource
	{
	public:
		DX11ConstantBufferResource(const Graphics& gfx, const ConstantBufferLayout& cbl, IConstantBufferResource::Type type, int slot, const std::string& tag = "?");
		void Bind() override;
		void Bind(const IContext& context) override;
		ID3D11Buffer* Data() const;
		void Update();
		void Update(ID3D11DeviceContext& context);
		void InitDynamicCBufferOnGPU();
		void Resize(std::size_t newSize) override;
	private:
		void Bind_(ID3D11DeviceContext& context);
	private:
		const Graphics& gfx;
		int slot;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
	};
}