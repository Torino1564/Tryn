#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include "Gfx.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <concepts>

#define BINDABLE_LIST \
		X(VertexBuffer)\
		X(PolyVBuffer)\
		X(IndexBuffer)\
		X(VertexShader)\
		X(PixelShader)\
		X(InputLayout)\
		X(PrimitiveTopology)\
		X(ConstantBuffer)

namespace tryn::gfx
{

	class BindablePool
	{
		template<typename>
		struct ResolveHelper;
        template <typename T>
        struct ResolveHelper;

        template <> struct ResolveHelper<IVertexBuffer>
        {
            template <class ... Args>
            std::shared_ptr<IVertexBuffer> operator()(IGraphics& gfx, Args&&...args)
            {
                return gfx.CreateVertexBuffer(std::forward<Args>(args)...);
            }
        };
        template <> struct ResolveHelper<IPolyVBuffer>
        {
            template <class ... Args>
            std::shared_ptr<IPolyVBuffer> operator()(IGraphics& gfx, Args&&...args)
            {
                return gfx.CreatePolyVertexBuffer(std::forward<Args>(args)...);
            }
        };
        template <> struct ResolveHelper<IVertexShader>
        {
            template <class ... Args>
            std::shared_ptr<IVertexShader> operator()(IGraphics& gfx, Args&&...args)
            {
                return gfx.CreateVertexShader(std::forward<Args>(args)...);
            }
        };
        template <> struct ResolveHelper<IPixelShader>
        {
            template <class ... Args>
            std::shared_ptr<IPixelShader> operator()(IGraphics& gfx, Args&&...args)
            {
                return gfx.CreatePixelShader(std::forward<Args>(args)...);
            }
        };
        template <> struct ResolveHelper<IIndexBuffer>
        {
            template <class ... Args>
            std::shared_ptr<IIndexBuffer> operator()(IGraphics& gfx, Args&&...args)
            {
                return gfx.CreateIndexBuffer(std::forward<Args>(args)...);
            }
        };
        template <> struct ResolveHelper<IInputLayout>
        {
            template <class ... Args>
            std::shared_ptr<IInputLayout> operator()(IGraphics& gfx, Args&&...args)
            {
                return gfx.CreateInputLayout(std::forward<Args>(args)...);
            }
        };
        template <> struct ResolveHelper<IPrimitiveTopology>
        {
            template <class ... Args>
            std::shared_ptr<IPrimitiveTopology> operator()(IGraphics& gfx, Args&&...args)
            {
                return gfx.CreatePrimitiveTopology(std::forward<Args>(args)...);
            }
        };
        template <> struct ResolveHelper<IConstantBuffer>
        {
            template <class ... Args>
            std::shared_ptr<IConstantBuffer> operator()(IGraphics& gfx, Args&&...args)
            {
                return gfx.CreateConstantBuffer(std::forward<Args>(args)...);
            }
        };

	public:
		template <class T, class... Args>
		std::shared_ptr<T> static Resolve(IGraphics& gfx, Args&& ... args)
		{
            //Gets the key that would be generated from the queried resource
            const auto key = T::GenerateID(gfx,args...);
            const auto it = Get().pool.find(key);
            if (key == "?")
            {
                ResolveHelper<T> functor;
                std::shared_ptr<T> bind = functor(gfx, std::forward<Args>(args)...);
                return bind;
            }
            else
            {
                if (it == Get().pool.end())
                {
                    // The key doesnt exist in the map, have to add a new element
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
                    std::shared_ptr<IBindable> bind = it->second.lock();
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