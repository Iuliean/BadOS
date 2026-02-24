#ifndef LIBBAD_COLOR_HPP
#define LIBBAD_COLOR_HPP

#include <cstdint>
namespace bad
{
    struct color
    {
        std::uint8_t b;
        std::uint8_t g;
        std::uint8_t r;

    };

    static inline constexpr color white = { 255, 255 ,255 };
    static inline constexpr color black = { 0, 0, 0 };
    static inline constexpr color red   = { 0, 0, 255 };
    static inline constexpr color green = { 0, 255, 0 };
    static inline constexpr color blue  = { 255, 0, 0 };

}

#endif //LIBBAD_COLOR_HPP
