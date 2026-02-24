#ifndef LIBBAD_SPAN_HPP
#define LIBBAD_SPAN_HPP
#include <limits>
#include <type_traits>
#include <iterator>
#include <array>
#include <ranges>

#include "debugger.hpp"

namespace bad
{
    template<typename T, std::size_t Extent>
    class span;

    namespace detail
    {
        template<typename T, typename ElmType>
        concept is_compatible_array =
            std::is_convertible_v<std::remove_reference_t<std::iter_reference_t<T>>(*)[], ElmType(*)[]>;

            template<typename T>
            inline constexpr bool is_span = false;

            template<typename T, std::size_t S>
            inline constexpr bool is_span<span<T,S>> = true;

            template<typename T>
            inline constexpr bool is_std_array = false;

            template<typename T, std::size_t S>
            inline constexpr bool is_std_array<std::array<T,S>> = true;
    }

    constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

    template<typename T, std::size_t Extent = dynamic_extent>
    class span
    {
    public:
        using element_type = T;
        using value_type = std::remove_cv_t<T>;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = element_type*;
        using const_pointer = const element_type*;
        using reference = element_type&;
        using const_reference = const element_type&;
        using iterator = pointer;
        using const_iterator = std::const_iterator<iterator>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::const_iterator<reverse_iterator>;
        static constexpr size_type extent = Extent;


        /***************
        * CONSTRUCTORS *
        ***************/

        constexpr span() noexcept
            : m_data(nullptr), m_size(0) {}

        template<std::contiguous_iterator It>
            requires detail::is_compatible_array<It, element_type>
        constexpr explicit(extent != dynamic_extent) span(It first, std::size_t count)
            : m_data(std::to_address(first)), m_size(count)
        {
            if constexpr (Extent != dynamic_extent)
                debugger::assert(count == Extent);
        }

        template<std::contiguous_iterator It, std::sized_sentinel_for<T> End>
            requires (detail::is_compatible_array<It, element_type> && !std::is_convertible_v<End, size_type>)
        constexpr explicit(extent != dynamic_extent) span(It first, End last)
            : m_data(first), m_size(static_cast<size_type>(last - first))
        {
            if constexpr (Extent != dynamic_extent)
                debugger::assert(m_size == Extent);
        }

        template<std::size_t N>
            requires (extent == dynamic_extent || N == extent)
        constexpr span(std::type_identity_t<element_type> (&arr)[N]) noexcept
            : m_data(arr), m_size(N) {}

        template<class U, std::size_t ArrayExtent>
            requires detail::is_compatible_array<U, element_type> &&
                (Extent == dynamic_extent || ArrayExtent == Extent)
        constexpr span(std::array<U, ArrayExtent>& arr) noexcept
            : m_data(arr.data()), m_size(ArrayExtent) {}

        template<class U, std::size_t ArrayExtent>
            requires detail::is_compatible_array<U, element_type> &&
                (Extent == dynamic_extent || ArrayExtent == Extent)
        constexpr span(const std::array<U, ArrayExtent>& arr) noexcept
            : m_data(arr.data()), m_size(ArrayExtent) {}

        template<class R>
            requires
                std::ranges::contiguous_range<R> && std::ranges::sized_range<R> &&
                (std::ranges::borrowed_range<R> || std::is_const_v<element_type>) &&
                (!std::is_array_v<std::remove_cvref_t<R>>) &&
                (!detail::is_std_array<R>) &&
                (!detail::is_span<R>) &&
                std::is_convertible_v<std::remove_reference_t<std::ranges::range_reference_t<R>>(*)[], element_type(*)[]>
        constexpr explicit(extent != dynamic_extent) span(R&& r)
            : m_data(std::ranges::data(r)), m_size(std::ranges::size(r))
        {
            if constexpr (Extent != dynamic_extent)
                    debugger::assert(m_size == Extent);
        }

        template<typename OtherType, std::size_t OtherExtent>
            requires
                (Extent == dynamic_extent || OtherExtent == dynamic_extent || Extent == OtherExtent) &&
                std::is_convertible_v<OtherType(*)[], element_type(*)[]>
        constexpr explicit (Extent != dynamic_extent && OtherExtent == dynamic_extent) span(const span<OtherType, OtherExtent>& other) noexcept
            : m_data(other.m_data, other.m_size)
        {
            if constexpr (Extent != dynamic_extent)
                debugger::assert(other.size() == extent);
        }

        constexpr span(const span& other) noexcept = default;
        constexpr span& operator=(const span& other) noexcept = default;

        /************
        * ITERATORS *
        ************/

        constexpr iterator begin() const noexcept { return m_data; }
        constexpr reverse_iterator rbegin() const noexcept { return std::reverse_iterator(end()); }
        constexpr iterator end() const noexcept { return m_data + m_size; }
        constexpr reverse_iterator rend() const noexcept { return std::reverse_iterator(begin()); }

        /************
        * OBSERVERS *
        ************/

        constexpr size_type size() const noexcept { return m_size; }
        constexpr size_type size_bytes() const noexcept { return m_size * sizeof(element_type); }

        [[nodiscard("Checks are important :)")]]
        constexpr bool empty()const noexcept { return size() == 0;}

        /*****************
        * ELEMENT ACCESS *
        *****************/

        constexpr pointer data()const noexcept { return m_data; }

        constexpr reference operator[](std::size_t idx) const
        {
            debugger::assert(idx < m_size);
            return *(data() + idx);
        }

        constexpr reference front() const
        {
            debugger::assert(!empty());
            return *m_data;
        }

        constexpr reference back() const
        {
            debugger::assert(!empty());
            return *(data() + (size() - 1));
        }


    private:
        pointer m_data;
        size_type m_size;

    };

}

#endif //LIBBAD_SPAN_HPP
