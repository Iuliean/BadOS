#ifndef CONSOLE_HPP
#define CONSOLE_HPP
#include <cstddef>

#include "libbad/string_view.hpp"

namespace os
{
    class console
    {
    public:
        static console& get_console();

        explicit console();
        ~console() = default;

        void println(bad::string_view string);
        void println(const char* string);
        void println(char c);

        void print(bad::string_view string);
        void print(const char* string);
        void print(char c);

    private:

        inline void advance() { advance(1); }
        void advance(std::size_t count);

        inline void retreat() { retreat(1); }
        void retreat(std::size_t count);

        void reset();
        void new_line();

        struct cursor
        {
            std::size_t x = 0;
            std::size_t y = 0;
        };

        std::size_t m_width;
        std::size_t m_height;

        cursor m_cursor;
    };
}

#endif //CONSOLE_HPP
