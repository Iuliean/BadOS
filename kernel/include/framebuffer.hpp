#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP
#include "error.hpp"

#include "libbad/color.hpp"
#include "libbad/span.hpp"

namespace os::framebuffer
{
    std::size_t get_width();
    std::size_t get_height();
    std::size_t get_pitch();
    std::size_t get_bits_per_pixel();

    result<void> put_pixel(std::size_t x, std::size_t y, bad::color pixel_color);

    result<void> render_glyph_at(std::size_t x, std::size_t y, bad::span<const std::byte> glyph_data, bad::color color = bad::white);
}


#endif //FRAMEBUFFER_HPP
