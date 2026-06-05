#include <Core/src/gfx/Bindables/VertexBuffer.h>
#include <wrl/client.h>
#include <any>
#include <vector>

struct ID3D11Buffer;
struct ID3D11DeviceContext;

namespace tryn::gfx::dx11
{
	class Graphics;

	class DX11VertexBuffer : public IVertexBuffer
	{
	public:
		DX11VertexBuffer(const Graphics& gfx, const std::shared_ptr<VertexBuffer>& cpuBuffer, const std::string& tag = "?");
		void Bind() override;
		void Bind(const IContext& context) override;
		std::vector<std::any> GetLayoutFromVB() const override;
		std::vector<std::any> GetSlottedLayoutFromVB(int slot) const override;
		ID3D11Buffer* Data() const;
		/*	void Update();
			void Update(ID3D11DeviceContext& context);*/

	private:
		void Bind_(ID3D11DeviceContext& context);

	private:
		const Graphics& gfx;
		unsigned int stride = 0;
		unsigned int offset = 0;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
	};
}