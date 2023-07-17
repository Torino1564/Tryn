#pragma once
#include <Core/src/win/TrynWin.h>
#include <optional>
#include <typeinfo>
#include <Core/src/utl/Assert.h>
#include <Core/src/spa/Dimensions.h>
#include <Core/src/utl/String.h>
#include <vector>
#include <concepts>
#include <memory>
#include <variant>
#include "ImguiManager.h"

#define GRAPHIC_APIS \
		X( DX11 ) \
		X( DX12 ) \
		X( Vulkan ) \
		X( Unknown )

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

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
	class IIndexBuffer;
	class IPrimitiveTopology;
	class ConstantBufferLayout;
	class IConstantBuffer;

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
		static const std::vector<std::string>& GetAPIArray()
		{
			static std::vector<std::string> GRAPHIC_APISTR = {
	#define X(el) GENERATE_STRING(el)
			GRAPHIC_APIS
	#undef X
			};
			return GRAPHIC_APISTR;
		}
		// Resurce Creation
		virtual std::shared_ptr<IVertexBuffer>		CreateVertexBuffer(VertexBuffer, std::string tag = "?") = 0;
		virtual std::shared_ptr<IIndexBuffer>		CreateIndexBuffer(std::shared_ptr<std::vector<int>> indices, std::string tag = "?") = 0;
		virtual std::shared_ptr<IPolyVBuffer>		CreatePolyVertexBuffer(std::vector<std::variant<std::pair<std::string, VertexBuffer>, std::shared_ptr<IVertexBuffer>>>, std::string tag = "?") = 0;
		virtual std::shared_ptr<IVertexShader>		CreateVertexShader(std::string path) = 0;
		virtual std::shared_ptr<IPixelShader>		CreatePixelShader(std::string path) = 0;
		virtual std::shared_ptr<IInputLayout>		CreateInputLayout(IVertexBuffer& vb, IVertexShader& vs) = 0;
		virtual std::shared_ptr<IInputLayout>		CreateInputLayout(IPolyVBuffer& vb, IVertexShader& vs) = 0;
		virtual std::shared_ptr<IPrimitiveTopology> CreatePrimitiveTopology() = 0;
		virtual std::shared_ptr<IConstantBuffer>	CreateConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") = 0;

		spa::DimensionsI dimensions = spa::DimensionsI(0, 0);
	};
}