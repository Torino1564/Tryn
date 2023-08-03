#pragma once
#include <string>
#include <sstream>
#include <optional>
#include "StackTrace.h"
#include <format>
#include "Macro.h"
#include "Exception.h"
#include "NoReturn.h"

namespace tryn::utl
{
	ZT_EX_DEF(FailedAssertion);

	class Assertion
	{
	public:
		// types
		enum class Consequence
		{
			Log,
			Terminate,
			Exception,
		};
		// functions
		Assertion(std::wstring expression, const wchar_t* file, const wchar_t* function, int line, Consequence consequence = Consequence::Terminate, std::wstring msg = L"");
		~Assertion();
		Assertion& msg(const std::wstring& message);
		template<typename T>
		Assertion& watch(T&& val, const wchar_t* name)
		{
			stream_ << L"   " << name << L" => " << std::forward<T>(val) << L"\n";
			return *this;
		}
		[[noreturn]]
		void ex();
	private:
		const wchar_t* file_;
		const wchar_t* function_;
		int line_ = -1;
		int skip_depth_ = 0;
		Consequence consequence_;
		std::wostringstream stream_;
	};
}

#ifndef ZT_TRYNASS_ACTIVE
#ifdef NDEBUG
#define ZT_TRYNASS_ACTIVE false
#else
#define ZT_TRYNASS_ACTIVE true
#endif
#endif

#define trynass(expr) (!ZT_TRYNASS_ACTIVE || bool(expr)) ? void(0) : (void)tryn::utl::Assertion{ ZT_WSTR(expr), __FILEW__, __FUNCTIONW__, __LINE__ }
#define trynass_msg(expr, msg) (!ZT_TRYNASS_ACTIVE || bool(expr)) ? void(0) : (void)tryn::utl::Assertion{ ZT_WSTR(expr), __FILEW__, __FUNCTIONW__, __LINE__ , tryn::utl::Assertion::Consequence::Terminate , msg}

#define trynchk(expr) bool(expr) ? void(0) : (void)tryn::utl::Assertion{ ZT_WSTR(expr), __FILEW__, __FUNCTIONW__, __LINE__, ZT_TRYNASS_ACTIVE ? tryn::utl::Assertion::Consequence::Terminate : tryn::utl::Assertion::Consequence::Log ,L""}

#define trynchk_fail (void)tryn::utl::Assertion{ L"[Always Fail]", __FILEW__, __FUNCTIONW__, __LINE__, ZT_TRYNASS_ACTIVE ? tryn::utl::Assertion::Consequence::Terminate : tryn::utl::Assertion::Consequence::Log }

#define ass_watch(...) ZT_DISPATCH_VA(ZZ_AW_, __VA_ARGS__)
#define ZZ_AW_(expr) watch((expr), ZT_WSTR(expr))
#define ZZ_AW_1_(z) ZZ_AW_(z)
#define ZZ_AW_2_(z, a) ZZ_AW_(z).ZZ_AW_(a)
#define ZZ_AW_3_(z, a, b) ZZ_AW_(z).ZZ_AW_(a).ZZ_AW_(b)
#define ZZ_AW_4_(z, a, b, c) ZZ_AW_(z).ZZ_AW_(a).ZZ_AW_(b).ZZ_AW_(c)
#define ZZ_AW_5_(z, a, b, c, d) ZZ_AW_(z).ZZ_AW_(a).ZZ_AW_(b).ZZ_AW_(c).ZZ_AW_(d)
#define ZZ_AW_6_(z, a, b, c, d, e) ZZ_AW_(z).ZZ_AW_(a).ZZ_AW_(b).ZZ_AW_(c).ZZ_AW_(d).ZZ_AW_(e)
#define ZZ_AW_7_(z, a, b, c, d, e, f) ZZ_AW_(z).ZZ_AW_(a).ZZ_AW_(b).ZZ_AW_(c).ZZ_AW_(d).ZZ_AW_(e).ZZ_AW_(f)
#define ZZ_AW_8_(z, a, b, c, d, e, f, g) ZZ_AW_(z).ZZ_AW_(a).ZZ_AW_(b).ZZ_AW_(c).ZZ_AW_(d).ZZ_AW_(e).ZZ_AW_(f).ZZ_AW_(g)
#define ZZ_AW_9_(z, a, b, c, d, e, f, g, h) ZZ_AW_(z).ZZ_AW_(a).ZZ_AW_(b).ZZ_AW_(c).ZZ_AW_(d).ZZ_AW_(e).ZZ_AW_(f).ZZ_AW_(g).ZZ_AW_(h)