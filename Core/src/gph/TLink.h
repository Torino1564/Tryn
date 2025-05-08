#pragma once
#include "PinInfo.h"

namespace tryn::gph
{
	class TGraph;
	struct TLink
    {
        unsigned long long Id;
        unsigned long long InputId;
        unsigned long long OutputId;
        TGraph* pGraph = nullptr;

        TLink(unsigned long long id, PinInfo& pin1, PinInfo& pin2, TGraph* pGraph);
        TLink& operator=(TLink&& rhs) noexcept;
        TLink(TLink&& rhs) noexcept;
        ~TLink();

        TLink() = default;
    };
}

namespace tryn::ser
{
    void Serialize(class StreamIO& io, gph::TLink* data, bool binary, const std::string& name);
}
