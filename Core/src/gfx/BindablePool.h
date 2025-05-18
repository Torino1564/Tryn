#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <unordered_map>
#include <memory>
#include <string>
#include <Core/src/gfx/IBufferFwd.h>
#include "IGraphics.h"

namespace tryn::gfx
{
	class BindablePool
	{
		template <typename T>
		struct ResolveHelper;

		template <> struct ResolveHelper<IVertexBuffer>
		{
			template <class ... Args>
			std::shared_ptr<IVertexBuffer> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreateVertexBuffer(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IVertexShader>
		{
			template <class ... Args>
			std::shared_ptr<IVertexShader> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreateVertexShader(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IPixelShader>
		{
			template <class ... Args>
			std::shared_ptr<IPixelShader> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreatePixelShader(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IIndexBuffer>
		{
			template <class ... Args>
			std::shared_ptr<IIndexBuffer> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreateIndexBuffer(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IInputLayout>
		{
			template <class ... Args>
			std::shared_ptr<IInputLayout> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreateInputLayout(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IPrimitiveTopology>
		{
			template <class ... Args>
			std::shared_ptr<IPrimitiveTopology> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreatePrimitiveTopology(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IVtxConstantBuffer>
		{
			template <class ... Args>
			std::shared_ptr<IVtxConstantBuffer> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreateVtxConstantBuffer(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IVtxConstantBufferNCach>
		{
			template <class ... Args>
			std::shared_ptr<IVtxConstantBufferNCach> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreateNonCachVtxConstantBuffer(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IPxConstantBuffer>
		{
			template <class ... Args>
			std::shared_ptr<IPxConstantBuffer> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreatePxConstantBuffer(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IPxConstantBufferNCach>
		{
			template <class ... Args>
			std::shared_ptr<IPxConstantBufferNCach> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreateNonCachPxConstantBuffer(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<ITexture>
		{
			template <class ... Args>
			std::shared_ptr<ITexture> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreateTexture(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IRasterizer>
		{
			template <class ... Args>
			std::shared_ptr<IRasterizer> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreateRasterizer(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<ISampler>
		{
			template <class ... Args>
			std::shared_ptr<ISampler> operator()(const IGraphics& gfx, Args&&...args)
			{
				return gfx.CreateSampler(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IOutputOnlyRenderTargetView>
		{
			template <class... Args>
			std::shared_ptr<IOutputOnlyRenderTargetView> operator()(const IGraphics& gfx, Args&&... args)
			{
				return gfx.CreateOutputOnlyRenderTargetView(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IShaderResourceRenderTargetView>
		{
			template <class... Args>
			std::shared_ptr<IShaderResourceRenderTargetView> operator()(const IGraphics& gfx, Args&&... args)
			{
				return gfx.CreateShaderResourceRenderTargetView(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IOutputOnlyDepthStencil>
		{
			template <class... Args>
			std::shared_ptr<IOutputOnlyDepthStencil> operator()(const IGraphics& gfx, Args&&... args)
			{
				return gfx.CreateOutputOnlyDepthStencil(std::forward<Args>(args)...);
			}
		};
		template <> struct ResolveHelper<IShaderResourceDepthStencil>
		{
			template <class... Args>
			std::shared_ptr<IShaderResourceDepthStencil> operator()(const IGraphics& gfx, Args&&... args)
			{
				return gfx.CreateShaderResourceDepthStencil(std::forward<Args>(args)...);
			}
		};

	public:
		template <class T, class... Args>
		std::shared_ptr<T> static Resolve(const IGraphics& gfx, Args&& ... args)
		{
			//Gets the key that would be generated from the queried resource
			const auto key = T::GenerateID(gfx, args...);
			const auto it = Get().pool.find(key);
			if (key == "?")
			{
				ResolveHelper<T> functor;
				std::shared_ptr<T> bind = functor(gfx, std::forward<Args>(args)...);
				return bind;
			}
			else
			{
				if (it == Get().pool.end() || it->second.expired())
				{
					// The key doesn't exist in the map or the element is expired, have to add a new element
					// Get ptr to newly created element
					ResolveHelper<T> functor;
					std::shared_ptr<T> bind = functor(gfx, std::forward<Args>(args)...);
					// Assign a weak ptr to the map
					std::weak_ptr<T> weakPtr = bind;
					Get().pool[key] = weakPtr;

					return bind;
				}
				else
				{
					// The key exists, returns a shared ptr to the queried resource
					const std::shared_ptr<IBindable> bind = it->second.lock();
					return std::static_pointer_cast<T>(bind);
				}
			}
		}

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