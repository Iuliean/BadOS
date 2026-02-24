#include "framebuffer.hpp"

#include "libbad/color.hpp"
#include "libbad/memory.hpp"

#include <cstddef>
#include <bitset>
#include <limine.h>
#include <ranges>

#include "error.hpp"
#include "utils.hpp"

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
};

namespace os::framebuffer
{
    static const limine_framebuffer& get_frame_buffer()
    {
        return *framebuffer_request.response->framebuffers[0];
    }

    static std::byte* get_buffer() { return static_cast<std::byte* >(get_frame_buffer().address);}

    std::size_t get_width() { return get_frame_buffer().width; }
    std::size_t get_height(){ return get_frame_buffer().height; }
    std::size_t get_pitch() { return get_frame_buffer().pitch; }
    std::size_t get_bits_per_pixel() { return get_frame_buffer().bpp; }



    static std::size_t coords_to_pixel_index(std::size_t x, std::size_t y)
    {
        auto framebuffer = get_frame_buffer();
        return (x * framebuffer.pitch) + (y * 4);
    }

    result<void> put_pixel(std::size_t x, std::size_t y, bad::color color)
    {
        auto framebuffer = get_frame_buffer();

        if (x >= framebuffer.width || y >= framebuffer.height)
            return make_unexpected_error(
                    generic_errors::out_of_bounds_access,
                    "Coordinates are out of bounds of the frame buffer"
                );

        bad::memcpy(
            static_cast<void*>(get_buffer() + coords_to_pixel_index(x, y)),
            &color,
            sizeof(bad::color)
        );

        return result<void>{};
    }

    result<void> render_glyph_at(std::size_t x, std::size_t y, bad::span<const std::byte> glyph_data, bad::color color)
    {
        if (x > (get_width() - 8))
            return make_unexpected_error(generic_errors::out_of_bounds_access, "Glyph will not fit horizontally on screen");
        if (y > (get_height() - glyph_data.size()))
            return make_unexpected_error(generic_errors::out_of_bounds_access, "Glyph will not fit vertiaclly on screen");


        for (auto [index, glyp_line] : std::views::enumerate(glyph_data))
        {
            std::bitset<8> line_bits(std::to_integer<std::uint8_t>(glyp_line));

            for (std::uint8_t column_index = 0 ; column_index < 8; ++column_index)
            {
                if (line_bits[column_index])
                    RETURN_IF_UNEXPECTED(put_pixel(x + index, y + 7 - column_index, color));
            }
        }

        return result<void>{};
    }

}
