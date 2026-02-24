#ifndef FONT_HPP
#define FONT_HPP
#include "libbad/span.hpp"

namespace font
{

    bad::span<const std::byte> get_font_bin_data();

    bad::span<const std::byte> get_glyph_data(char c);
}

#endif //FONT_HPP
