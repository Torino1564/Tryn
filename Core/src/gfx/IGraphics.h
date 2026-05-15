#pragma once
#include <Core/src/utl/Assert.h>
#include <Core/src/spa/Dimensions.h>
#include <vector>
#include <concepts>
#include <memory>
#include <thread>
#include <semaphore>
#include <Core/src/ccr/GenericTaskQueue.h>
#include "Render/RenderGraph.h"
#include <Core/src/gfx/IContext.h>
#include <Core/src/gfx/GraphicAPI.h>
#include <Core/src/win/WindowHandle.h>
#include <Core/src/gfx/TextureFormat.h>
#include <Core/src/utl/Tuple.h>
#include <ranges>
#include <tuple>
#include <any>
#include <array>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include "Bindables/Bindable.h"
#include "Bindables/IBufferBase.h"
#include "Bindables/TransformCBuf.h"
#include "Render/IRenderWorker.h"
#include "Render/Step.h"
#include "Vertex.h"
#include <Core/src/log/Log.h>
#include <Core/src/utl/String.h>
#include <Core/src/utl/StringHasher.h>
#include <Core/src/utl/TypeName.h>
#include <Core/third/glm/fwd.hpp>

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
		IGraphics();
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
		glm::vec3 GetAmbientColor() const;
		void SetAmbientColor(glm::vec3 color);
		const spa::DimensionsI& GetDimensions() const;
		virtual constexpr GraphicAPI GetType() const = 0;
		virtual void Resize() = 0;
		virtual std::shared_ptr<IRenderTargetView> GetRenderTargetView() const = 0;
		virtual std::shared_ptr<IDepthStencil> GetDepthStencilView() const = 0;
		static constexpr uint32_t MapTextureFormatStride(TextureFormat format);
		static const std::vector<std::string>& GetApiArray();
		virtual constexpr void AssertContextCoherence(const IContext& context) const;
		IContext& GetContextInterface() const;
		constexpr virtual const char* GetAPIString() const = 0;
		static const char* GetShaderRootPath();
		const float* GetBackgroundColor() const;
		void SetBackgroundColor(const float r, const float g, const float b, const float a);

		template<std::invocable F>
		auto Dispatch(F&& f) const;
		static constexpr auto MAX_CONSTRUCTORS_ALLOWED = 5;

		template <typename Bindable, typename... ParameterTuples>
		struct Register
		{
			using Bindable_t = Bindable;
			using ParameterTuplesTuple_t = std::tuple<ParameterTuples...>;
		};

		// Bindable Registration
		using SupportedBindables = std::tuple<
			Register<IVertexBuffer>,
			Register<class ISOAVertexBuffer>,
			Register<IIndexBuffer>,
			Register<class IVertexShader>,
			Register<class IPixelShader>,
			Register<class IInputLayout,
					std::tuple<IVertexBuffer&, class IVertexShader&>,
					std::tuple<class VertexLayout&, class IVertexShader&>>,
			Register<class IPrimitiveTopology>,
			Register<IVtxConstantBuffer>,
			Register<IVtxConstantBufferNCach>,
			Register<IPxConstantBuffer>,
			Register<IPxConstantBufferNCach>,
			Register<IInstanceBuffer>,
			Register<class ITexture,
					std::tuple<const std::shared_ptr<class Texture>&, uint8_t, TextureUsage>,
					std::tuple<spa::DimensionsI, TextureFormat, uint8_t, TextureUsage>>,
			Register<class IRasterizer>,
			Register<class ISampler>,
			Register<IRenderTargetView>,
			Register<IDepthStencil>,
			Register<class ITransformCBuf>
		>;

		using BindableVTable = std::array<std::array<std::any, MAX_CONSTRUCTORS_ALLOWED>, std::tuple_size_v<SupportedBindables>>;
		const BindableVTable& GetBindableVTable() const;

		// Bindable Creation
		template <typename T, typename ... Args> requires std::is_convertible_v<std::add_pointer_t<T>, IBindable*>
		std::shared_ptr<T> CreateBindable(Args&&... args) const
		{
			//static_assert(utl::tuple_contains_type_v<T, SupportedBindables>, "Attempting to create unsupported bindable type");
			const auto& vtable = GetBindableVTable();
			static constexpr auto indexInTupleOfBindables = utl::GetTypeIndexFromTupleOfRegister<T, SupportedBindables>();
			trylog.debug(std::to_wstring(indexInTupleOfBindables));
			auto& funcArray = vtable[indexInTupleOfBindables];
			using ConstRefTuple = utl::ConstRefTuple_t<std::tuple<Args...>>;
			using Register = std::tuple_element_t<utl::GetTypeIndexFromTupleOfRegister<T, SupportedBindables>(), SupportedBindables>;
			static constexpr auto paramTupleUUID = ZT_TYPE_UUID(ConstRefTuple);
			trylog.info(utl::ToWide(ZT_TYPE_OF(ConstRefTuple).data()));
			for (const std::any& any : funcArray)
			{
				for (auto [index, rtti] : std::views::enumerate(bindableConstructorsRTTI[indexInTupleOfBindables]))
				{
					if (rtti.has_value() && any.has_value() && rtti.type() == any.type())
					{
						if (TestConvertible<Register, 0, Args...>(index))
						{
							return CastAndCallToNth<Register>(any, index, std::forward<Args>(args)...);
						}
					}
				}
			}
			std::unreachable();
		}

		template <typename Register, unsigned N = 0, typename... Args>
		bool TestConvertible(unsigned n) const;

		template <typename Register, unsigned N = 0, typename... Args>
		std::shared_ptr<typename Register::Bindable_t> CastAndCallToNth(const std::any& pFunc, unsigned n, Args&&... args) const;

		// Other resource creation
		virtual std::unique_ptr<IRenderWorker> CreateRenderWorker(ccr::Master*) const = 0;

	protected:
		void InitDefaults();
		virtual void InitDefaultRenderGraph();
		spa::DimensionsI dimensions = spa::DimensionsI(0, 0);
		glm::vec3 ambientColor = { 0.01f, 0.01f, 0.01f };
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
		auto Dispatch_(F&& f) const;
		std::unique_ptr<IContext> pContext = nullptr;
		BindableVTable bindableVtable = {};
		std::array<std::array<std::any, MAX_CONSTRUCTORS_ALLOWED>, std::tuple_size_v<SupportedBindables>> bindableConstructorsRTTI = {};
		bool vsync = false;

		// Instaced Parents:
		std::unordered_map<std::string, std::shared_ptr<class InstancedModelParent>> rogueInstancedModelParentMap;
	};

	template <std::invocable F>
	auto IGraphics::Dispatch(F&& f) const
	{
		return Dispatch_(std::forward<F>(f));
	}

	template <typename Register, unsigned N, typename ... Args>
	bool IGraphics::TestConvertible(const unsigned n) const
	{
		using Interface = typename Register::Bindable_t;
		using ParameterTuplesTuple = typename Register::ParameterTuplesTuple_t;

		if constexpr (N == 0 && std::tuple_size_v<ParameterTuplesTuple> == 0)
		{
			// The case where no tuple parameters are specified means that it should get the static Resolve Method
			return true;
		}
		else if constexpr (N < std::tuple_size_v<ParameterTuplesTuple>)
		{
			if (N == n)
			{
				using FunctionArgTuple = std::tuple_element_t<N, ParameterTuplesTuple>;
				return std::is_convertible_v<std::tuple<Args...>, FunctionArgTuple>;
			}
			return TestConvertible<Register, N + 1, Args...>(n);
		}
		std::unreachable();
	}

	template <typename Register, unsigned N, typename ... Args>
	std::shared_ptr<typename Register::Bindable_t> IGraphics::CastAndCallToNth(const std::any& pFunc, unsigned n,
		Args&&... args) const
	{
		using Interface = typename Register::Bindable_t;
		using ParameterTuplesTuple = typename Register::ParameterTuplesTuple_t;

		if constexpr (N == 0 && std::tuple_size_v<ParameterTuplesTuple> == 0)
		{
			// The case where no tuple parameters are specified means that it should get the static Resolve Method
			auto pCastedFunc = std::any_cast<std::shared_ptr<Interface>(*)(const IGraphics&, typename utl::MethodArgTupleMinusFirst<decltype(&Interface::Resolve)>::t)>(pFunc);
			return pCastedFunc(*this, std::forward_as_tuple(std::forward<Args>(args)...));
		}
		else if constexpr (N < std::tuple_size_v<ParameterTuplesTuple>)
		{
			using FunctionArgTuple = std::tuple_element_t<N, ParameterTuplesTuple>;
			trylog.debug(L"From tuple:" + utl::ToWide(ZT_TYPE_OF(ParameterTuplesTuple).data()) + L" selecting: " + std::to_wstring(N));
			trylog.debug(L"From: " + utl::ToWide(ZT_TYPE_OF(std::tuple<Args...>).data()));
			trylog.debug(L"To: " + utl::ToWide(ZT_TYPE_OF(FunctionArgTuple).data()));
			if constexpr (std::is_convertible_v<std::tuple<Args...>, FunctionArgTuple>)
			{
				if (N == n)
				{
					auto pCastedFunc = std::any_cast<std::shared_ptr<Interface>(*)(const IGraphics&, FunctionArgTuple)>(pFunc);
					return pCastedFunc(*this, std::forward_as_tuple(std::forward<Args>(args)...));
				}
			}
			return CastAndCallToNth<Register, N + 1>(pFunc, n, std::forward<Args>(args)...);
		}
		std::unreachable();
	}

	template <std::invocable F>
	auto IGraphics::Dispatch_(F&& f) const
	{
		std::lock_guard lk{ mtx };
		auto future = tasks_.Push(std::forward<F>(f));
		cv.notify_one();
		return future;
	}

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

	constexpr void IGraphics::AssertContextCoherence(const IContext& context) const
	{
		trynass(context.GetApi() == GetType());
	}
}
