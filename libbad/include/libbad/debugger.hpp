#ifndef LIBBAD_DEBUGGER_HPP
#define LIBBAD_DEBUGGER_HPP

namespace bad::debugger
{

    namespace detail
    {
        [[noreturn]]
        inline void pseudo_break()
        {
            asm volatile ("hlt");
        }
    }

    [[noreturn]]
    inline constexpr void assert(bool condition)
    {
        if (!condition)
            detail::pseudo_break();
    }
}

#endif //LIBBAD_DEBUGGER_HPP
