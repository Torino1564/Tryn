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

	public:
		template <class T, class... Args>
		std::shared_ptr<T> static Resolve(const IGraphics& gfx, Args&& ... args)
		{
			//Gets the key that would be generated from the queried resource
			const auto key = T::GenerateID(gfx, std::forward<Args>(args)...);
			const auto it = Get().pool.find(key);
			if (key == "?")
			{
				std::shared_ptr<T> bind = gfx.CreateBindable<T>(std::forward<Args>(args)...);
				return bind;
			}
			else
			{
				if (it == Get().pool.end() || it->second.expired())
				{
					// The key doesn't exist in the map or the element is expired, have to add a new element
					// Get ptr to newly created element
					std::shared_ptr<T> bind = gfx.CreateBindable<T>(std::forward<Args>(args)...);
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