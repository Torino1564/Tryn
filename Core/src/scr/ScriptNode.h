#pragma once
#include "Core/src/spa/Vec2.h"

namespace tryn
{
	namespace app
	{
		class App;
	}

	namespace scr
	{
		class ScriptNodeId;
	    struct ScriptGraph;

		struct ScriptNode
	    {
	        virtual ~ScriptNode() = default;
	        unsigned long long uniqueId;
	        std::vector<unsigned long long> pinIds;
	        std::vector<unsigned long long> childrenIds;
	        std::string name;
	        void Submit(const std::function<void(ScriptNode&)>& submitBehaviour);
	        bool placed = false;
	        ScriptGraph* pGraph;
	        spa::Vec2I position = {};
	        virtual unsigned long long Execute() { return uniqueId; }

	        static void ImGuiCreate(ScriptGraph* graph, spa::Vec2I screenPos, std::function<void()>& finalBehaviour);

	        friend class TrynEditorApp;
	        ScriptNode() = default;
	        ScriptNode(ScriptNode&&) = default;
	    protected:
	        ScriptNode(ScriptGraph* pGraph, const std::string_view name, spa::Vec2I position = {});
	    };
	}

	namespace ser
	{
		void SerializeWrite(const class StreamWriter& sw, const scr::ScriptNode& data, const bool binary, const std::string& name);

		void SerializeRead(const class StreamReader& sr, scr::ScriptNode& data, const bool binary, class ExtraDataPack* pExtraData = nullptr);
	}
}
