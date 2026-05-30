#pragma once
#include "Core/src/spa/Vec2.h"
#include <functional>
#include <vector>
#include <string>

namespace tryn
{
	namespace app
	{
		class App;
	}

	namespace gph
	{
	    class TGraph;

		class TNode
	    {
		public:
	        virtual ~TNode() = default;
	        unsigned long long uniqueId;
	        std::vector<unsigned long long> pinIds;
	        std::vector<unsigned long long> childrenIds;
	        std::string name;
	        void Submit(const std::function<void(TNode&)>& submitBehaviour);
	        bool placed = false;
	        TGraph* pGraph;
	        spa::Vec2I position = {};
	        virtual unsigned long long Execute() { return uniqueId; }

	        static void ImGuiCreate(TGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour);

	        friend class TrynEditorApp;
	        TNode() = default;
	        TNode(TNode&&) = default;
	    protected:
	        TNode(TGraph* pGraph, const std::string_view name, spa::Vec2I position = {});
	    };
	}

	namespace ser
	{
		void Serialize(class StreamIO& io, gph::TNode* data, bool binary, const std::string& name);
	}
}
