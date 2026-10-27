#include "text_codec.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace hb::server::world::text
{
	namespace
	{
		constexpr UINT code_page_cp949 = 949;

		bool convert(std::string_view in, UINT from, UINT to, std::string& out)
		{
			out.clear();
			if (in.empty())
				return true;
			const int length = static_cast<int>(in.size());
			const int wide_length = MultiByteToWideChar(from, MB_ERR_INVALID_CHARS, in.data(), length, nullptr, 0);
			if (wide_length <= 0)
				return false;
			std::wstring wide(static_cast<std::size_t>(wide_length), L'\0');
			if (MultiByteToWideChar(from, MB_ERR_INVALID_CHARS, in.data(), length, wide.data(), wide_length) != wide_length)
				return false;
			// WC_ERR_INVALID_CHARS is only valid for UTF-8; for CP949 a character with no mapping is caught by the
			// default-character flag instead.
			BOOL used_default = FALSE;
			const DWORD flags = (to == CP_UTF8) ? WC_ERR_INVALID_CHARS : 0;
			BOOL* default_flag = (to == CP_UTF8) ? nullptr : &used_default;
			const int narrow_length = WideCharToMultiByte(to, flags, wide.data(), wide_length, nullptr, 0, nullptr, default_flag);
			if (narrow_length <= 0 || used_default)
				return false;
			std::string narrow(static_cast<std::size_t>(narrow_length), '\0');
			if (WideCharToMultiByte(to, flags, wide.data(), wide_length, narrow.data(), narrow_length, nullptr, default_flag) != narrow_length || used_default)
				return false;
			out = std::move(narrow);
			return true;
		}
	}

	bool cp949_to_utf8(std::string_view cp949, std::string& out)
	{
		return convert(cp949, code_page_cp949, CP_UTF8, out);
	}

	bool utf8_to_cp949(std::string_view utf8, std::string& out)
	{
		return convert(utf8, CP_UTF8, code_page_cp949, out);
	}
}
