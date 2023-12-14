#include <iostream>

#define COMPONENT(x) class x : public ComponentImpl<x>
#define INIT_COMPONENT 	private: static inline const int ID = ComponentManager::Get().ResolveID


namespace tryn::ent::prot
{
	class IDResolver
	{
		static inline int componentCount = 0;
	public:
		int ResolveID()
		{
			return componentCount++;
		}
	};

	class ComponentManager
	{
	public:
		static ComponentManager& Get()
		{
			ComponentManager singleton;
			return singleton;
		}
		int ResolveID()
		{
			return idResolver.ResolveID();
		}
	private:
		ComponentManager() = default;
		IDResolver idResolver;
	};

	class Component
	{
	public:
		virtual ~Component() = default;
		virtual void OnUpdate() = 0;
	};

	template <typename ComponentType>
	class ComponentImpl : public Component
	{
	public:
		virtual ~ComponentImpl() = default;
		void OnUpdate() override
		{
			ComponentType::OnUpdateImpl();
		}

	};

	COMPONENT(TestComponent1)
	{
		INIT_COMPONENT();
	public:
		static void OnUpdateImpl()
		{
			std::cout << "Test Component 1 Update Implementation. ID = " << ID << "\n";
		}
	};

	COMPONENT(TestComponent2)
	{
		INIT_COMPONENT();
	public:
		static void OnUpdateImpl()
		{
			std::cout << "Test Component 2 Update Implementation. ID = " << ID << "\n";
		}
	};
}