#ifndef LIBBAD_ALGORITHM_HPP
#define LIBBAD_ALGORITHM_HPP

namespace bad
{
    template<typename T>
    constexpr const T& min (const T& lhs, const T& rhs)
    {
        return lhs < rhs ? lhs : rhs;
    }

    template<typename T, typename ComparatorT>
    constexpr const T& min(const T& lhs, const T& rhs, ComparatorT comp)
    {
        return comp(lhs, rhs) ? lhs : rhs;
    }

    template<typename T>
    constexpr const T& max (const T& lhs, const T& rhs)
    {
        return lhs > rhs ? lhs : rhs;
    }

    template<typename T, typename ComparatorT>
    constexpr const T& max(const T& lhs, const T& rhs, ComparatorT comp)
    {
        return comp(lhs, rhs) ? lhs : rhs;
    }
}

#endif  //LIBBAD_ALGORITHM_HPP
