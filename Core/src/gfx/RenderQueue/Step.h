#pragma once
#include <Core/src/gfx/IGraphics.h>


namespace tryn::gfx
{
	class Step
	{
	public:
		void Bind(IGraphics& gfx)
		{
			for (auto& [name,vtxCbuff] : vtxCbuffers)
			{
				vtxCbuff->Bind();
			}
			for (auto& [name, pxCbuffers] : pxCbuffers)
			{
				pxCbuffers->Bind();
			}

		}
		std::vector<std::pair<std::string, std::shared_ptr<gfx::IVtxConstantBuffer>>> vtxCbuffers;
		std::vector<std::pair<std::string, std::shared_ptr<gfx::IPxConstantBuffer>>> pxCbuffers;
		std::shared_ptr<IVertexShader> pVertexShader;
		std::shared_ptr<IPixelShader> pPixelShader;
	};
}