
#include "Exception.h"
#include "String.h"
#include <format>
#include <typeinfo>

namespace tryn::utl
{
	BufferedException::BufferedException(const std::string& msg)
		:
		message_{ msg }
	{}
	BufferedException::BufferedException(const std::wstring& msg)
		:
		message_{ ToNarrow(msg) }
	{}
	const char* BufferedException::what() const
	{
		using namespace std::string_literals;
		buffer_ = "["s + name().data() + "]"s;
		if (!message_.empty()) {
			buffer_ += ": ";
			buffer_ += message_;
		}
		return buffer_.c_str();
	}
}
