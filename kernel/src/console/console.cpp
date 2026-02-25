#include "console/console.hpp"

#include "console/font/font.hpp"
#include "framebuffer.hpp"
#include "libbad/string_view.hpp"
#include <memory>

namespace os
{
    static console instance;
    console& console::get_console()
    {
        return instance;
    }

    console::console()
        : m_width(framebuffer::get_width() / 8),
          m_height( framebuffer::get_height() / font::get_glyph_data('s').size()),
          m_cursor()
    {
    }

    void console::println(bad::string_view string)
    {
        print(string);
        new_line();
    }

    void console::println(const char* string)
    {
        println(bad::string_view{string});
    }

    void console::println(char c)
    {
        println(bad::string_view{std::addressof(c), 1});
    }

    void console::print(bad::string_view string)
    {
        for (const char c : string)
        {
            auto glyph = font::get_glyph_data(c);
            [[maybe_unused]]auto r = framebuffer::render_glyph_at(m_cursor.x * glyph.size(), m_cursor.y * 8, glyph);
            advance();
        }
    }

    void console::print(const char* string) { print(bad::string_view{string}); }
    void console::print(char c) { print(bad::string_view{std::addressof(c), 1}); }


    /***********
    * PRIVATE *
    **********/

    void console::advance(std::size_t count)
    {
        if ((m_cursor.y + count) > m_width)
        {
            m_cursor.y = 0;
            if (m_cursor.x + 1 < m_height)
                ++m_cursor.x;
        }
        else
            m_cursor.y += count;
    }

    void console::retreat(std::size_t count)
    {
        if (m_cursor.y < count)
        {
            m_cursor.y = 0;

            if (m_cursor.x < 2)
                m_cursor.x = 0;
            else
                --m_cursor.x;
        }
        else
            m_cursor.y -= count;
    }

    void console::reset() { m_cursor.x = 0; m_cursor.y = 0; }

    void console::new_line()
    {
        m_cursor.x = 0;

        if (m_cursor.y != m_height)
            ++m_cursor.y;
    }

}
