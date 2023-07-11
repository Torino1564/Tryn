#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include "Gfx.h"
#include <unordered_map>
#include <memory>
#include "Bindables/VertexBuffer.h"
#include <string>
#include <concepts>

namespace tryn::gfx
{

	class BindablePool
	{
	public:
		static std::shared_ptr<IVertexBuffer>			ResolveVertexBuffer(IGraphics& gfx, std::shared_ptr<VertexBuffer> cpuBuffer);
		static std::shared_ptr<IPolyVBuffer>			ResolvePolyVertexBuffer(IGraphics& gfx, std::vector<std::shared_ptr<VertexBuffer>>& buffers);
		static std::shared_ptr<IIndexBuffer>			ResolveIndexBuffer(IGraphics& gfx, std::shared_ptr<std::vector<int>> indices);
		static std::shared_ptr<IVertexShader>			ResolveVertexShader(IGraphics& gfx, std::string path);
		static std::shared_ptr<IPixelShader>			ResolvePixelShader(IGraphics& gfx, std::string path);
		static std::shared_ptr<IInputLayout>			ResolveInputLayout(IGraphics& gfx, IVertexBuffer& vb, IVertexShader& vs);
		static std::shared_ptr<IInputLayout>			ResolveInputLayout(IGraphics& gfx, IPolyVBuffer& vb, IVertexShader& vs);
		static std::shared_ptr<IPrimitiveTopology>		ResolvePrimitiveTopology(IGraphics& gfx);
		static std::shared_ptr<IConstantBuffer>			ResolveConstantBuffer(IGraphics& gfx, ConstantBufferLayout&&);
		
		template <class T, class... Args>
		std::shared_ptr<T> static Resolve(IGraphics& gfx, Args&& ... args)
		{
			ResolveHelper<T> functor;
			return functor(gfx, std::forward<Args>(args)...);
		}
		template<typename>
		struct ResolveHelper;
		template<> struct ResolveHelper<IVertexBuffer>
		{
			template <class ... Args>
			std::shared_ptr<IVertexBuffer> operator()(IGraphics& gfx, Args&&...args)
			{
				return gfx.CreateVertexBuffer(std::forward<Args>(args)...);
			}
		};

	private:
		BindablePool() = default;
		static BindablePool& Get()
		{
			static BindablePool pool;
			return pool;
		}
		std::unordered_map<std::string, std::weak_ptr<IBindable>> pool;
	};
}