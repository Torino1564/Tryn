#include "ConstantBuffer.h"
#include <Core/src/utl/Assert.h>

namespace tryn::gfx
{
	ConstantBufferLayout::Node::Node(Type type, std::wstring id)
		:
		type(type),
		id(id)
	{}

	void ConstantBufferLayout::Node::Append(Node child)
	{
		trynass_msg(type == Type::Struct && type == Type::Array, L"Attempted to append an element to a non array/struct node");
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
		static Node empty(Type::Empty , L"Empty");
		return empty;
	}
	ConstantBufferLayout::Node& ConstantBufferLayout::Node::operator[](std::wstring id)
	{
		trynass_msg(type == Type::Struct && type == Type::Array, L"Attempted to index an element to a non array/struct node");
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
}

