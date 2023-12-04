#pragma once
#include <Core/src/gfx/Bindables/VertexBuffer.h>
#include <Core/src/gfx/Bindables/IBuffer.h>
#include <Core/src/gfx/dx11/Dx11Graphics.h>
#include <Core/src/gfx/dx11/TrynWLR.h>
#include <memory>

namespace tryn::gfx::dx11
{
	//class DX11VertexBuffer : public IVertexBuffer
	//{
	//public:

	//	DX11VertexBuffer(Graphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer, std::string tag = "?");
	//	void Bind() override;
	//	void Bind(IContext& context) override;
	//	void Init() override;
	//	std::vector<std::any> GetLayoutFromVB() const override;
	//	std::vector<std::any> GetSlottedLayoutFromVB(int slot = 0) const override;
	//	ID3D11Buffer* GetPtr();
	//private:
	//	Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
	//	Graphics& gfx;
	//};
}