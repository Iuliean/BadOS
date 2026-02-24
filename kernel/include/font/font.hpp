#ifndef FONT_HPP
#define FONT_HPP
#include "libbad/span.hpp"

namespace font
{
    /* Returns the entire bin data for the font */
    bad::span<const std::byte> get_font_bin_data();

    /* Returns a span with data for the given character.
     The underlying span is an offset from get_font_bin_data */
    bad::span<const std::byte> get_glyph_data(char c);
}

#endif //FONT_HPP
