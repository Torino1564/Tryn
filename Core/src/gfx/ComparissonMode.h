#pragma once

namespace tryn::gfx
{
	enum class ComparissonMode
	{
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always
	};

	constexpr const char* to_string(ComparissonMode e)
	{
		switch (e)
		{
		case ComparissonMode::Never: return "Never";
		case ComparissonMode::Less: return "Less";
		case ComparissonMode::Equal: return "Equal";
		case ComparissonMode::LessEqual: return "LessEqual";
		case ComparissonMode::Greater: return "Greater";
		case ComparissonMode::NotEqual: return "NotEqual";
		case ComparissonMode::GreaterEqual: return "GreaterEqual";
		case ComparissonMode::Always: return "Always";
		default: return "unknown";
		}
	}
}