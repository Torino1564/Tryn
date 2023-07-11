#pragma once
#include <Core/src/win/TrynWin.h>
#include <optional>
#include <typeinfo>
#include <Core/src/utl/Assert.h>
#include <Core/src/spa/Dimensions.h>
#include <vector>
#include <concepts>

#define GRAPHIC_APIS \
		X( DX11 ) \
		X( DX12 ) \
		X( Vulkan ) \
		X( Unknown )

namespace tryn::ent
{
	class Model;
}

namespace tryn::gfx
{
	enum class GraphicAPI
	{
#define X(el) el,
		GRAPHIC_APIS
#undef X
	};

	class VertexBuffer;
	class IVertexBuffer;
	class IVertexShader;
	class IPixelShader;
	class IPolyVBuffer;
	class IPolyInputLayout;
	class IInputLayout;
	class IBindable;

	class IGraphics
	{
	public:
		struct IocParams
		{
			std::optional<int> width;
			std::optional<int> height;
			HWND hWnd;
		};

		virtual ~IGraphics() {}
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void ClearBuffer(float r, float g, float b) = 0;
		virtual void DrawTriangle() = 0;
		virtual void DrawIndexed(int count) = 0;
		virtual void MakeBindablesForModel(ent::Model&) = 0;
		template<typename T>
		auto& QueryInterface()
		{
			auto ptr = static_cast<T*>(this);
#ifdef _DEBUG
			trynass_msg(typeid(T) == typeid(this), L"Attempt to cast query IGraphics interface to an invalid type");
#endif
			return *ptr;
		}
		virtual GraphicAPI GetType() = 0;

		// Resurce Creation
		virtual void CreateVertexBuffer(std::shared_ptr<VertexBuffer> cpuBuffer, IVertexBuffer** ppBindable) = 0;
		virtual void CreatePolyVertexBuffer(std::vector<std::shared_ptr<VertexBuffer>>& buffers, IPolyVBuffer** ppBindable) = 0;
		virtual void CreateVertexShader(std::wstring path, IVertexShader** ppBindable) = 0;
		virtual void CreatePixelShader(std::wstring path, IPixelShader** ppBindable) = 0;
		virtual void CreatePolyInputLayout(IPolyVBuffer& PVBuf, IVertexShader& vs, IPolyInputLayout** ppBindable) = 0;
		virtual void CreateInputLayout(IVertexBuffer& vb, IVertexShader& vs , IInputLayout** ppBindable) = 0;
		virtual void CreateInputLayout(IPolyVBuffer& vb, IVertexShader& vs, IInputLayout** ppBindable) = 0;
		//virtual void CreatePixelShader() = 0;
		//virtual void CreateConstantBuffer() = 0;
		//virtual void CreatePrimitiveTopology() = 0;

		spa::DimensionsI dimensions = spa::DimensionsI(0, 0);
	};
}