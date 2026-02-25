#include "console/font/font.hpp"

extern const std::byte _binary_HUGE_VGA_F32_start[];
extern const std::byte _binary_HUGE_VGA_F32_end[];

namespace font
{
    bad::span<const std::byte> get_font_bin_data()
    {
        const std::size_t size = _binary_HUGE_VGA_F32_end - _binary_HUGE_VGA_F32_start;
        return bad::span<const std::byte>(reinterpret_cast<const std::byte*>(&_binary_HUGE_VGA_F32_start[32]), size);
    }

    bad::span<const std::byte> get_glyph_data(char c)
    {
        return bad::span<const std::byte>(reinterpret_cast<const std::byte*>(_binary_HUGE_VGA_F32_start + (c * 32)), 32);
    }
}
