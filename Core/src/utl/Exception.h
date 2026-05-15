#pragma once
#include <exception>
#include <string>
#include <typeinfo>

namespace tryn::utl
{
	class IException : public std::exception {};

	class BufferedException : public IException
	{
	public:
		BufferedException() = default;
		BufferedException(const std::string& msg);
		BufferedException(const std::wstring& msg);
		const char* what() const override;
		virtual std::string_view name() const = 0;
	private:
		std::string message_;
		mutable std::string buffer_;
	};

	template <typename T>
	class BufferedExceptionBase : public BufferedException
	{
	public:
		BufferedExceptionBase() = default;
		BufferedExceptionBase(const std::string& msg) : BufferedException(msg) {}
		BufferedExceptionBase(const std::wstring& msg) : BufferedException(msg) {}
		std::string_view name() const override
		{
			return typeid(T).name();
		}
	};

}

#define ZT_EX_DEF_FROM(NewType, BaseType) class NewType : public BaseType {using Base = BaseType; public: using Base::Base;}
#define ZT_EX_DEF(NewType) ZT_EX_DEF_FROM(NewType, tryn::utl::BufferedExceptionBase<NewType>)