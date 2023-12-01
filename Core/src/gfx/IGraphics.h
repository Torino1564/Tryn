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
#include <thread>
#include <atomic>
#include <semaphore>
#include <Core/src/ccr/GenericTaskQueue.h>
#include "RenderGraph.h"
#include <Core/src/gfx/IContext.h>
#include <Core/src/gfx/GraphicAPI.h>
#include <Core/src/utl/LocalGenericTaskQueue.h>

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

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
	class RenderWorker;

	class IGraphics
	{
		friend class app::App;
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
		virtual void DrawInstancedIndexed(int indexCount, int instanceCount, int startIndexLocation, int baseVertexLocation, int startInstanceLocation) = 0;
		void SetRenderGraph(std::unique_ptr<IRenderGraph>&&);
		IRenderGraph& GetRenderGraph();
		void ExecuteFrame();
		glm::mat4& GetCameraMatrix();
		void SetCamera(glm::mat4 camera);
		glm::mat4& GetProjectionMatrix();
		void SetProjection(glm::mat4 projection);
		virtual constexpr GraphicAPI GetType() const = 0;
		static const std::vector<std::string>& GetApiArray()
		{
			static std::vector<std::string> graphicApiString = {
	#define X(el) GENERATE_STRING(el)
			GRAPHIC_APIS
	#undef X
			};
			return graphicApiString;
		}
		virtual constexpr void AssertContextCoherence(IContext& context) const
		{
			trynass(context.GetApi() == GetType());
		}
		IContext& GetContext()
		{
			return *pContext;
		}
		virtual std::string_view GetAPIString() const = 0;

		template<std::invocable F>
		auto Dispatch(F&& f) const
		{
			return Dispatch_(std::forward<F>(f));
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
		
		// Worker Thread Creation
		virtual std::unique_ptr<RenderWorker>		CreateRenderWorker(ccr::Master*) = 0;

		spa::DimensionsI dimensions = spa::DimensionsI(0, 0);
	protected:
		glm::mat4 camera = {};
		glm::mat4 projection = {};
		std::unique_ptr<IRenderGraph> renderGraph;

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
	};
}