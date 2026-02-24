#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP
#include "error.hpp"

#include "libbad/color.hpp"
#include "libbad/span.hpp"

namespace os::framebuffer
{
    /*
     * General propeties of the framebuffer
     */
    std::size_t get_width();
    std::size_t get_height();
    std::size_t get_pitch();
    std::size_t get_bits_per_pixel();

    /*
     * @brief Renders a pixel at specified coordinates
     * @param[in] x, x coordinate in the framebuffer
     * @param[in] y, y coordinate in the framebuffer
     * @param[in] pixel_color, color of the renderd pixel
     * @return on error a kernel_error will be returned
     */
    result<void> put_pixel(std::size_t x, std::size_t y, bad::color pixel_color);

    /*
     * @brief Renders a glyp (character font) starting from x,y coordinates
     * @param[in] x, x coordinate in the framebuffer
     * @param[in] y, y coordinate in the framebuffer
     * @param[in] glyph_data, font data for the glyph to be rendered
     * @param[in] color, color of the resulting character. white by default
     * @return on error a kernel_error will be returned
     */
    result<void> render_glyph_at(std::size_t x, std::size_t y, bad::span<const std::byte> glyph_data, bad::color color = bad::white);
}


#endif //FRAMEBUFFER_HPP
