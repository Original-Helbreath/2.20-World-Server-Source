#pragma once

#include <string>
#include <string_view>

// Text conversion between the legacy game's CP949 (Korean) byte strings and the UTF-8 world.db stores (WORLD-008).
// The game logic keeps CP949 bytes in its fixed char buffers, so names go to UTF-8 on import and back on load.
// Platform code: text_codec_win32.cpp is the Windows implementation.
namespace hb::server::world::text
{
	// False when the input is not valid in the source encoding; out is left empty then.
	bool cp949_to_utf8(std::string_view cp949, std::string& out);
	bool utf8_to_cp949(std::string_view utf8, std::string& out);
}
