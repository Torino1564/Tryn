#pragma once

namespace tryn
{
	namespace gph
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
		void Serialize(class StreamIO& sw, gph::PinInfo* data, const bool binary, const std::string& name);
	}
}