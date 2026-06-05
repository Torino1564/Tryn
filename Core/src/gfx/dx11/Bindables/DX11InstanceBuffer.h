#include <Core/src/gfx/Bindables/InstanceBuffer.h>
#include <wrl/client.h>

struct ID3D11DeviceContext;
struct ID3D11Buffer;

namespace tryn::gfx::dx11
{
	class Graphics;

	class DX11InstanceBuffer : public IInstanceBuffer
	{
	public:
		DX11InstanceBuffer(const Graphics& gfx, const ConstantBufferLayout::Node& arrayElement, int slot, std::size_t numInstances = 50);
		void Bind() override;
		void Bind(const IContext& context) override;
		void Update();
		void Update(ID3D11DeviceContext& context);
		void InitDynamicCBufferOnGPU();
		void Resize(std::size_t newSize) override;
	private:
		void GPUSizeChanges();
		void Bind_(ID3D11DeviceContext& context);
	private:
		const Graphics& gfx;
		int slot;

		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
	};
}