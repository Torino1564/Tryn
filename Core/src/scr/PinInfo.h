#pragma once

namespace tryn
{
	namespace scr
	{
		enum class PinKind
		{
			Input,
			Output
		};

		struct PinInfo
		{
			unsigned long long parentId;
			unsigned long long linkedId;
			std::string name;
			PinKind kind;
			unsigned long long id;
			bool linked = false;
		};
	}

	namespace ser
	{
	    void SerializeWrite(const class StreamWriter& sw, const scr::PinInfo& data, const bool binary, const std::string& name);

	    void SerializeRead(const class StreamReader& sr, scr::PinInfo& data, const bool binary, class ExtraDataPack* pExtraData = nullptr);
	}
}