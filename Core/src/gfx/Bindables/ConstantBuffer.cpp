#include "ConstantBuffer.h"
#include <Core/src/utl/Assert.h>
#include <algorithm>
#include <ranges>
#include <queue>
#include <Core/src/log/Log.h>

namespace tryn::gfx
{
	ConstantBufferLayout::Node::Node(Type type, std::string id)
		:
		type(type),
		id(id)
	{}

	void ConstantBufferLayout::Node::Append(Node child)
	{
		trynass_msg( !(type != Type::Struct && type != Type::Array), L"Attempted to append an element to a non array/struct node");
		
		for (auto& existingChild : children)
		{
			trynass_msg(existingChild.id != child.id, L"Attempted to append an element with a duplicate ID");
		}
		child.parent = this;
		children.push_back(std::move(child));
	}
	bool ConstantBufferLayout::Node::IsRoot() const
	{
		return !parent.has_value();
	}
	bool ConstantBufferLayout::Node::IsLeaf() const
	{
		return children.empty();
	}
	ConstantBufferLayout::Node& ConstantBufferLayout::Node::GetEmpty() const
	{
		static Node empty(Type::Empty , "Empty");
		return empty;
	}
	ConstantBufferLayout::Node& ConstantBufferLayout::Node::operator[](std::string id)
	{
		return IndexByName(id);
	}
	ConstantBufferLayout::Node& ConstantBufferLayout::Node::IndexByName(std::string id)
	{
		trynass_msg(type == Type::Struct || type == Type::Array, L"Attempted to index an element to a non array/struct node");
		for (auto& child : children)
		{
			if (child.id == id)
			{
				return child;
			}
		}
		return GetEmpty();
	}
	bool ConstantBufferLayout::Node::Validate() const
	{
		return type != Type::Empty;
	}
	ConstantBufferLayout::Type ConstantBufferLayout::Node::GetType() const
	{
		return type;
	}
	size_t ConstantBufferLayout::Node::GetOffset() const
	{
#ifdef _DEBUG
		if (!solid)
			trylog.warn(L"Called member GetOffset in a non solid Node. The result may be subject to future changes");
#endif
		return offset;
	}

	template<ConstantBufferLayout::Type type>
	struct TrueTypeSizeLookup
	{
		static constexpr auto Exec() noexcept
		{
			return sizeof(ConstantBufferLayout::TypeAttr<type>::TrueType);
		}
	};
	constexpr size_t ConstantBufferLayout::SizeOf(ConstantBufferLayout::Type type)
	{
		return Bridge<TrueTypeSizeLookup>(type);
	}

	ConstantBufferLayout::ConstantBufferLayout()
	{
		root = std::make_unique<Node>(Type::Struct, "Root");
	}
	void ConstantBufferLayout::Append(Node child)
	{
		root->Append(std::move(child));
	}
	bool ConstantBufferLayout::IsSolid() const
	{
		return solid;
	}
	size_t ConstantBufferLayout::Size() const
	{
#ifdef _DEBUG
		if (!solid)
			trylog.warn(L"Called member GetOffset in a non solid Layout. The result may be subject to future changes");
#endif
		return size;
	}
	void ConstantBufferLayout::Solidify()
	{
		std::queue<Node*> q;
		q.push(root.get());
		Node* current = nullptr;
		size_t accumulatedOffset = 0;
		size_t remainingSpace = 0;
		while (!q.empty())
		{
			current = q.front();
			q.pop();

			remainingSpace = 16 - (accumulatedOffset % 16);
			const auto type = current->GetType();
			switch (type)
			{
			case Struct:
				break;
			case Array:
				break;
			case Empty:
				break;
			default:
				const auto size = SizeOf(type);
				current->offset = accumulatedOffset;
				if (size > remainingSpace)
				{
					if (size < 16)
					{
						accumulatedOffset += remainingSpace;
					}
				}
				accumulatedOffset += size;
				break;
			}

			current->solid = true;

			for (auto& child : current->children)
			{
				q.push(&child);
			}
		}
		solid = true;
		size = accumulatedOffset;
	}
	ConstantBufferLayout::Node& ConstantBufferLayout::operator[](std::string id)
	{
		return (*root.get())[id];
	}

	ElementView::ElementView(ConstantBufferLayout::Node& node, char* pBytes)
		:
		node(node),
		pBytes(pBytes)
	{}
}

