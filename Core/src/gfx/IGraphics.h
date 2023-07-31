#pragma once
#include <Core/src/win/TrynWin.h>
#include <optional>
#include <typeinfo>
#include <Core/src/utl/Assert.h>
#include <Core/src/spa/Dimensions.h>
#include <Core/src/utl/String.h>
#include <vector>
#include <concepts>
#include <filesystem>
#include <memory>
#include <variant>
#include "ImguiManager.h"
#include <Core/third/glm/glm.hpp>

#define GRAPHIC_APIS \
		X( DX11 ) \
		X( DX12 ) \
		X( Vulkan ) \
		X( Unknown )

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

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
	class IVtxConstantBuffer;
	class IPxConstantBuffer;
	class ITransformCBuf;
	class ITexture;
	class ISampler;
	enum class SamplerType;
	class IRasterizer;
	class StaticMesh;
	class VertexLayout;

	class IGraphics
	{
	public:
		struct IocParams
		{
			std::optional<int> width;
			std::optional<int> height;
			HWND hWnd{};
		};

		virtual ~IGraphics() = default;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void ClearBuffer(float r, float g, float b) = 0;
		virtual void DrawIndexed(int count) = 0;
		glm::mat4& GetCameraMatrix()
		{
			return camera;
		}
		void SetCamera(glm::mat4 camera)
		{
			this->camera = std::move(camera);
		}
		glm::mat4& GetProjectionMatrix()
		{
			return projection;
		}
		void SetProjection(glm::mat4 projection)
		{
			this->projection = std::move(projection);
		}
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
		static const std::vector<std::string>& GetApiArray()
		{
			static std::vector<std::string> graphicApiString = {
	#define X(el) GENERATE_STRING(el)
			GRAPHIC_APIS
	#undef X
			};
			return graphicApiString;
		}
		// Resource Creation
		virtual std::shared_ptr<IVertexBuffer>		CreateVertexBuffer(std::shared_ptr<VertexBuffer>, std::string tag = "?") = 0;
		virtual std::shared_ptr<IIndexBuffer>		CreateIndexBuffer(std::shared_ptr<const std::vector<int>> indices, std::string tag = "?") = 0;
		virtual std::shared_ptr<IPolyVBuffer>		CreatePolyVertexBuffer(std::vector<std::variant<std::pair<std::string, std::shared_ptr<tryn::gfx::VertexBuffer>>, std::shared_ptr<tryn::gfx::IVertexBuffer>, std::shared_ptr<tryn::gfx::IPolyVBuffer>>>&, std::string tag = "?") = 0;
		virtual std::shared_ptr<IVertexShader>		CreateVertexShader(std::string path) = 0;
		virtual std::shared_ptr<IPixelShader>		CreatePixelShader(std::string path) = 0;
		virtual std::shared_ptr<IInputLayout>		CreateInputLayout(IVertexBuffer& vb, IVertexShader& vs) = 0;
		virtual std::shared_ptr<IInputLayout>		CreateInputLayout(IPolyVBuffer& vb, IVertexShader& vs) = 0;
		virtual std::shared_ptr<IInputLayout>		CreateInputLayout(VertexLayout& vLayout, IVertexShader& vs) = 0;
		virtual std::shared_ptr<IPrimitiveTopology> CreatePrimitiveTopology() = 0;
		virtual std::shared_ptr<IVtxConstantBuffer>	CreateVtxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") = 0;
		virtual std::shared_ptr<IPxConstantBuffer>	CreatePxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") = 0;
		virtual std::shared_ptr<ITexture>			CreateTexture(std::filesystem::path path, int slot = 0) = 0;
		virtual std::shared_ptr<IRasterizer>		CreateRasterizer(const bool twoSided = true) = 0;
		virtual std::shared_ptr<ISampler>			CreateSampler(SamplerType type, bool reflect, int slot) = 0;
		virtual std::unique_ptr<ITransformCBuf>		CreateTransformCBuf() = 0;

		spa::DimensionsI dimensions = spa::DimensionsI(0, 0);
	private:
		glm::mat4 camera = {};
		glm::mat4 projection = {};
	};
}