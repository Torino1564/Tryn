#pragma once
#include <vector>
#include <string>
#include <unordered_map>

namespace tryn::gfx
{
	enum class VertexElement
	{
		Position3D,
		Position2D,
		Normal,
		UV,
		Tangent,
		Bitangent,
		Unknown,
	};

	class VertexLayout
	{
		template<typename... Args>
		VertexLayout( Args ... args)
		{
			std::unordered_map<VertexElement, int> CountPerElement;
			AppendElement(args ...)
		}

		template<typename T ,typename... Args>
		void AppendElement(T type , Args ... args , std::unordered_map<VertexElement, int>& map)
		{
			map[type]++;
			Elements.emplace_back(std::make_pair(type, std::string(map[type]));

			AppendElement(args, map);
		}
		void AppendElement()
		{
			// Base case for recursive function
		}
		std::vector<std::pair<VertexElement,std::string>> Elements;
	};

	class VertexBuffer
	{
		VertexLayout layout;
	private:        
		char* buffer;
		int bufferByteSize
	};
}