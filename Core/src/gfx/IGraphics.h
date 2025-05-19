#pragma once
#include <Core/src/utl/Assert.h>
#include <Core/src/spa/Dimensions.h>
#include <vector>
#include <concepts>
#include <memory>
#include <Core/third/glm/glm.hpp>
#include <thread>
#include <semaphore>
#include <Core/src/ccr/GenericTaskQueue.h>
#include "Render/RenderGraph.h"
#include <Core/src/gfx/IContext.h>
#include <Core/src/gfx/GraphicAPI.h>
#include <Core/src/gfx/IBufferFwd.h>
#include <Core/src/gfx/ComparissonMode.h>
#include <Core/src/gfx/RTVDSFwd.h>
#include <Core/src/win/WindowHandle.h>


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
	class RenderWorker;
	class IGenericRenderTargetView;

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
		virtual std::shared_ptr<IGenericRenderTargetView> GetRenderTargetView() const = 0;
		virtual std::shared_ptr<IGenericDepthStencil> GetDepthStencilView() const = 0;
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

		// Resource Creation
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
		virtual std::unique_ptr<IInstanceBuffer>					CreateInstanceBuffer(ConstantBufferLayout::Node node, std::size_t size, int slot = 2) const = 0;
		virtual std::shared_ptr<ITexture>							CreateTexture(std::filesystem::path path, int slot = 0) const = 0;
		virtual std::shared_ptr<ITexture>							CreateTexture(const aiTexture& tex, int slot = 0) const = 0;
		virtual std::shared_ptr<ITexture>							CreateTexture(std::shared_ptr<Texture> pTexture, int slot = 0) const = 0;
		virtual std::shared_ptr<IRasterizer>						CreateRasterizer(const bool twoSided = true) const = 0;
		virtual std::shared_ptr<ISampler>							CreateSampler(SamplerType type, bool reflect, int slot) const = 0;
		virtual std::shared_ptr<IOutputOnlyRenderTargetView>		CreateOutputOnlyRenderTargetView(const spa::DimensionsI dimensions) const = 0;
		virtual std::shared_ptr<IShaderResourceRenderTargetView>	CreateShaderResourceRenderTargetView(const spa::DimensionsI, const uint16_t slot) const = 0;
		virtual std::shared_ptr<IOutputOnlyDepthStencil>			CreateOutputOnlyDepthStencil(const spa::DimensionsI, ComparissonMode mode = ComparissonMode::Less) const = 0;
		virtual std::shared_ptr<IShaderResourceDepthStencil>		CreateShaderResourceDepthStencil(const spa::DimensionsI, const uint16_t slot, ComparissonMode mode = ComparissonMode::Less) const = 0;
		virtual std::unique_ptr<ITransformCBuf>						CreateTransformCBuf() const = 0;
		virtual std::unique_ptr<RenderWorker>						CreateRenderWorker(ccr::Master*) const = 0;

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

		bool vsync = false;
	protected:
		// Instaced Parents:
		std::unordered_map<std::string, std::shared_ptr<class InstancedModelParent>> rogueInstancedModelParentMap;
	};
}