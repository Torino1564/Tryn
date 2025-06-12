#pragma once
#include <Core/src/utl/Assert.h>
#include <Core/src/spa/Dimensions.h>
#include <vector>
#include <concepts>
#include <memory>
#include <Core/third/glm/glm.hpp>
#include <thread>
#include <semaphore>
#include <filesystem>
#include <Core/src/ccr/GenericTaskQueue.h>
#include "Render/RenderGraph.h"
#include <Core/src/gfx/IContext.h>
#include <Core/src/gfx/GraphicAPI.h>
#include <Core/src/gfx/IBufferFwd.h>
#include <Core/src/gfx/ComparissonMode.h>
#include <Core/src/win/WindowHandle.h>
#include <Core/src/gfx/TextureFormat.h>
#include <Core/src/utl/Tuple.h>

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

struct aiTexture;

namespace tryn::app
{
	class App;
}

namespace tryn::ccr
{
	class Master;
}

namespace tryn::gfx
{
	class Texture;
	class VertexBuffer;
	class ISOAVertexBuffer;
	class IndexBuffer;
	class IVertexShader;
	class IPixelShader;
	class IPolyVBuffer;
	class IPolyInputLayout;
	class IInputLayout;
	class IBindable;
	class IPrimitiveTopology;
	class ITransformCBuf;
	class ITexture;
	class ISampler;
	enum class SamplerType;
	class IRasterizer;
	class StaticMesh;
	class VertexLayout;
	class IRenderWorker;
	class IRenderTargetView;
	class IDepthStencil;

	class IGraphics
	{
		friend class app::App;
	public:
		struct IocParams
		{
			std::optional<int> width = std::nullopt;
			std::optional<int> height = std::nullopt;
			win::WindowHandle hWnd = nullptr;
		};
		virtual ~IGraphics() = default;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void ClearBuffer(float r, float g, float b) const = 0;
		virtual void DrawIndexed(int count) const = 0;
		virtual void DrawIndexedInstanced(int indexCount, int instanceCount, int startIndexLocation, int baseVertexLocation, int startInstanceLocation) const = 0;
		void SetRenderGraph(std::unique_ptr<IRenderGraph>&&);
		IRenderGraph& GetRenderGraph() const;
		void ExecuteFrame();
		const glm::mat4& GetCameraMatrix() const;
		glm::mat4& GetCameraMatrix();
		void SetCamera(glm::mat4 camera);
		glm::mat4& GetProjectionMatrix();
		const glm::mat4& GetProjectionMatrix() const;
		void SetProjection(glm::mat4 projection);
		bool GetVsyncFlag() const;
		void SetVsyncFlag(bool vsync);
		const spa::DimensionsI& GetDimensions() const;
		virtual constexpr GraphicAPI GetType() const = 0;
		virtual void Resize() = 0;
		virtual std::shared_ptr<IRenderTargetView> GetRenderTargetView() const = 0;
		virtual std::shared_ptr<IDepthStencil> GetDepthStencilView() const = 0;
		static constexpr uint32_t MapTextureFormatStride(TextureFormat format);
		static const std::vector<std::string>& GetApiArray()
		{
			static std::vector<std::string> graphicApiString = {
	#define X(el) GENERATE_STRING(el)
			GRAPHIC_APIS
	#undef X
			};
			return graphicApiString;
		}
		virtual constexpr void AssertContextCoherence(const IContext& context) const
		{
			trynass(context.GetApi() == GetType());
		}
		IContext& GetContextInterface() const;
		constexpr virtual const char* GetAPIString() const = 0;

		static const char* GetShaderRootPath();
		const float* GetBackgroundColor() const
		{
			return bgcolor;
		}
		void SetBackgroundColor(const float r, const float g, const float b, const float a)
		{
			bgcolor[0] = r;
			bgcolor[1] = g;
			bgcolor[2] = b;
			bgcolor[3] = a;
		}
		template<std::invocable F>
		auto Dispatch(F&& f) const
		{
			return Dispatch_(std::forward<F>(f));
		}
		static constexpr auto MAX_CONSTRUCTORS_ALLOWED = 5;

		using SupportedBindables = std::tuple<
			IVertexBuffer,
			ISOAVertexBuffer,
			IIndexBuffer,
			IVertexShader,
			IPixelShader,
			IInputLayout,
			IPrimitiveTopology,
			IVtxConstantBuffer,
			IVtxConstantBufferNCach,
			IPxConstantBuffer,
			IPxConstantBufferNCach,
			IInstanceBuffer,
			ITexture,
			IRasterizer,
			ISampler,
			IRenderTargetView,
			IDepthStencil
		>;

		using BindableVTable = std::array<std::array<std::pair<void*, utl::UUID_t>, MAX_CONSTRUCTORS_ALLOWED>, std::tuple_size_v<SupportedBindables>>;
		const BindableVTable& GetBindableVTable() const;

		// Bindable Creation
		template <typename T, typename... Args>
			requires std::is_convertible_v<std::add_pointer_t<T>, std::add_pointer_t<IBindable>>
		std::shared_ptr<T> CreateBindable(Args&&... args) const
		{ 
			static_assert(utl::tuple_contains_type_v<T, SupportedBindables>, "Attempting to create unsupported bindable type");
			const auto& vtable = GetBindableVTable();
			auto& funcArray = vtable[utl::GetTypeIndex<T, SupportedBindables>()];
			for (auto [pFunc, uuid] : funcArray)
			{
				trylog.info(utl::ToWide(ZT_TYPE_OF(std::tuple<Args...>).data()));
				if (uuid != ZT_TYPE_UUID(std::tuple<Args...>))
					continue;
				auto func = static_cast<std::shared_ptr<T>(*)(const IGraphics&, std::tuple<Args...>)>(pFunc);
				auto pBindable = func(*this, std::forward_as_tuple(std::forward<Args>(args)...));
				return std::static_pointer_cast<T>(pBindable);
			}
			std::runtime_error{"Attempting to create a bindable with incorrect parameters. See the Resolve declarations"};
			std::unreachable();
		}

		virtual std::shared_ptr<IVertexBuffer>						CreateVertexBuffer(const std::shared_ptr<VertexBuffer>&, std::string tag = "?") const = 0;
		virtual std::shared_ptr<ISOAVertexBuffer>					CreateSOAVertexBuffer() const = 0;
		virtual std::shared_ptr<IIndexBuffer>						CreateIndexBuffer(std::shared_ptr<IndexBuffer> indices, std::string tag = "?") const = 0;
		virtual std::shared_ptr<IVertexShader>						CreateVertexShader(std::string path) const = 0;
		virtual std::shared_ptr<IPixelShader>						CreatePixelShader(std::string path) const = 0;
		virtual std::shared_ptr<IInputLayout>						CreateInputLayout(IVertexBuffer& vb, IVertexShader& vs) const = 0;
		virtual std::shared_ptr<IInputLayout>						CreateInputLayout(VertexLayout& vLayout, IVertexShader& vs) const = 0;
		virtual std::shared_ptr<IPrimitiveTopology>					CreatePrimitiveTopology() const = 0;
		virtual std::shared_ptr<IVtxConstantBuffer>					CreateVtxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") const = 0;
		virtual std::shared_ptr<IVtxConstantBufferNCach>			CreateNonCachVtxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") const= 0;
		virtual std::shared_ptr<IPxConstantBuffer>					CreatePxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") const = 0;
		virtual std::shared_ptr<IPxConstantBufferNCach>				CreateNonCachPxConstantBuffer(ConstantBufferLayout&&, int slot = 0, std::string tag = "?") const = 0;
		virtual std::shared_ptr<IInstanceBuffer>					CreateInstanceBuffer(ConstantBufferLayout::Node node, std::size_t size, int slot = 2) const = 0;
		virtual std::shared_ptr<ITexture>							CreateTexture(std::filesystem::path path, int slot = 0) const = 0;
		virtual std::shared_ptr<ITexture>							CreateTexture(const aiTexture& tex, int slot = 0) const = 0;
		virtual std::shared_ptr<ITexture>							CreateTexture(std::shared_ptr<Texture> pTexture, int slot = 0) const = 0;
		virtual std::shared_ptr<IRasterizer>						CreateRasterizer(bool twoSided = true) const = 0;
		virtual std::shared_ptr<ISampler>							CreateSampler(SamplerType type, bool reflect, int slot) const = 0;
		virtual std::shared_ptr<IRenderTargetView>					CreateRenderTargetView(spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot, TextureFormat format = TextureFormat::B8G8R8A8_UNORM) const = 0;
		virtual std::shared_ptr<IDepthStencil>						CreateDepthStencil(spa::DimensionsI dimensions, bool shaderResource, std::optional<uint16_t> slot = std::nullopt, ComparissonMode mode = ComparissonMode::Less) const = 0;
		virtual std::shared_ptr<ITransformCBuf>						CreateTransformCBuf() const = 0;
\
		// Other resource creation
		virtual std::unique_ptr<IRenderWorker>						CreateRenderWorker(ccr::Master*) const = 0;

	protected:
		void InitDefaults();
		virtual void InitDefaultRenderGraph();
		spa::DimensionsI dimensions = spa::DimensionsI(0, 0);
		glm::mat4 camera = {};
		glm::mat4 projection = {};
		std::unique_ptr<IRenderGraph> pRenderGraph;

		// Bg color
		float bgcolor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

		// Multithreading stuff
		mutable std::mutex mtx;
		mutable std::condition_variable cv;
		std::binary_semaphore startSignal_{ 0 };
		mutable ccr::GenericTaskQueue tasks_;
		std::jthread kernelThread_;
		bool closing_ = false;

		void InitThread();
		virtual void KernelLoop_();

		template<std::invocable F>
		auto Dispatch_(F&& f) const
		{
			std::lock_guard lk{ mtx };
			auto future = tasks_.Push(std::forward<F>(f));
			cv.notify_one();
			return future;
		}
		std::unique_ptr<IContext> pContext;
		BindableVTable bindableVtable;
		bool vsync = false;

		// Instaced Parents:
		std::unordered_map<std::string, std::shared_ptr<class InstancedModelParent>> rogueInstancedModelParentMap;
	};

	constexpr uint32_t IGraphics::MapTextureFormatStride(const TextureFormat format)
	{
		using TF = TextureFormat;
		switch (format)
		{
		case TF::R8G8B8A8_UNORM:
		case TF::R8G8B8A8_UNORM_SRGB:
		case TF::B8G8R8A8_UNORM:
		case TF::B8G8R8A8_UNORM_SRGB:
			return 4;

		case TF::R32_FLOAT:
		case TF::R32_UINT:
			return 4;

		case TF::R32G32_FLOAT:
		case TF::R32G32_UINT:
			return 8;

		case TF::R32G32B32_FLOAT:
		case TF::R32G32B32_UINT:
			return 12;

		case TF::R32G32B32A32_FLOAT:
		case TF::R32G32B32A32_UINT:
			return 16;

		case TF::R16_FLOAT:
			return 2;

		case TF::R16G16_FLOAT:
			return 4;

		case TF::R16G16B16A16_FLOAT:
			return 8;

		case TF::D32_FLOAT:
			return 4;

		case TF::D24_UNORM_S8_UINT:
			return 4;

			// Compressed formats use block-based compression (typically 4x4 blocks)
		case TF::BC1_UNORM:
			return 8; // 8 bytes per 4x4 block

		case TF::BC3_UNORM:
		case TF::BC7_UNORM:
			return 16; // 16 bytes per 4x4 block

		case TF::UNKNOWN:
		default:
			return 0;
		}
	}
}