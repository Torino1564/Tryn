#pragma once
#include "PinInfo.h"

namespace tryn::scr
{
	struct ScriptGraph;
	struct Link
    {
        unsigned long long Id;
        unsigned long long InputId;
        unsigned long long OutputId;
        ScriptGraph* pGraph = nullptr;

        Link(unsigned long long id, PinInfo& pin1, PinInfo& pin2, ScriptGraph* pGraph);
        Link& operator=(Link&& rhs) noexcept;
        Link(Link&& rhs) noexcept;
        ~Link();

        Link() = default;
    };
}

namespace tryn::ser
{
    void SerializeWrite(const class StreamWriter& sw, const scr::Link& data, const bool binary, const std::string& name);

    void SerializeRead(const class StreamReader& sr, scr::Link& data_, const bool binary, class ExtraDataPack* pExtraData = nullptr);
}
