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
		trynass_msg(!(type != Type::Struct && type != Type::Array), L"Attempted to append an element to a non array/struct node");

		for (auto& existingChild : children)
		{
			trynass_msg(existingChild.id != child.id, L"Attempted to append an element with a duplicate ID");
		}
		//child.parent = this;
		children.push_back(std::move(child));
	}
	void ConstantBufferLayout::Node::Append(ConstantBufferLayout::Type type, std::string name)
	{
		Append(ConstantBufferLayout::Node(type, std::move(name)));
	}
	/*bool ConstantBufferLayout::Node::IsRoot() const
	{
		return !parent.has_value();
	}*/
	bool ConstantBufferLayout::Node::IsLeaf() const
	{
		return children.empty();
	}
	ConstantBufferLayout::Node& ConstantBufferLayout::Node::GetEmpty()
	{
		static Node empty(Type::Empty, "Empty");
		return empty;
	}
	ConstantBufferLayout::Node& ConstantBufferLayout::Node::operator[](std::string id)
	{
		return IndexByName(id);
	}
	ConstantBufferLayout::Node& ConstantBufferLayout::Node::operator[](std::size_t index)
	{
		return IndexByKey(index);
	}
	ConstantBufferLayout::Node& ConstantBufferLayout::Node::IndexByName(std::string id)
	{
		trynass_msg(type == Type::Struct || type == Type::Array, L"Attempted to index by name into a non struct node");
		for (auto& child : children)
		{
			if (child.id == id)
			{
				return child;
			}
		}
		return GetEmpty();
	}
	ConstantBufferLayout::Node& ConstantBufferLayout::Node::IndexByKey(std::size_t key)
	{
		trynass_msg(type == Type::Array, L"Attempted to index by key into a non array node");
		trynass_msg( key < children.size(), L"Out of bounds access into constant buffer array node");
		return children[key];
	}
	void ConstantBufferLayout::Node::Resize(std::size_t newSize, ConstantBuffer& buffer)
	{
		trynass_msg(GetType() == ConstantBufferLayout::Type::Array, L"Tried to resize a non array element");
		children.resize(newSize, children[0]);
		buffer.layout.Solidify();
		buffer.Rebase();
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

	void ConstantBufferLayout::Node::Set(Node node, std::size_t numElements)
	{
		children.reserve(numElements);
		for (auto i = 0; i < numElements; i++)
		{
			children.push_back(node);
		}
	}

	std::size_t ConstantBufferLayout::Node::Size() const
	{
		trynass_msg(GetType() == ConstantBufferLayout::Type::Array, L"Tried to resize a non array element");
		return children.size();
	}

	template<ConstantBufferLayout::Type type>
	struct TrueTypeSizeLookup
	{
		static constexpr auto Exec() noexcept
		{
			return ConstantBufferLayout::TypeAttr<type>::TrueTypeSize;
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
	void ConstantBufferLayout::Append(ConstantBufferLayout::Type type, std::string name)
	{
		Append(ConstantBufferLayout::Node(type, std::move(name)));
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
			// ReSharper disable once CppTooWideScope
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
				if (size > remainingSpace)
				{
					if (size < 16)
					{
						accumulatedOffset += remainingSpace;
					}
				}
				current->offset = accumulatedOffset;
				accumulatedOffset += size;
				break;
			}

			current->solid = true;

			for (auto& child : current->children)
			{
				q.push(&child);
			}
		}

		// Add offset to structs
		q.push(root.get());
		while (!q.empty())
		{
			current = q.front();
			q.pop();

			for (auto& child : current->children)
			{
				q.push(&child);
			}

			if (current->type == ConstantBufferLayout::Struct && !current->children.empty())
			{
				current->offset = current->children[0].offset;
			}
		}

		solid = true;
		size = accumulatedOffset + (16 - accumulatedOffset % 16);
	}
	ConstantBufferLayout::Node& ConstantBufferLayout::operator[](const std::string& id)
	{
		return (*root)[id];
	}

	ConstantBufferLayout::Node& ConstantBufferLayout::GetRoot()
	{
		return *root;
	}

	ElementView::ElementView(ConstantBufferLayout::Node& node, char* pBytes, ConstantBuffer* pBuffer)
		:
		node(node), 
		pBytes(pBytes),
		pBuffer(pBuffer)
	{}
	bool ElementView::Exists() const
	{
		if (node.GetType() == ConstantBufferLayout::Type::Empty)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}