#ifndef LIBBAD_STRING_VIEW_HPP
#define LIBBAD_STRING_VIEW_HPP
#include "debugger.hpp"
#include "memory.hpp"
#include <algorithm>
#include <compare>
#include <string_view>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>

namespace bad
{
    class string_view
    {
    public:

        using const_ref = const char&;
        using const_iter = const char*;
        using reverse_const_iter = std::reverse_iterator<const_iter>;

        inline static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

        constexpr string_view() noexcept
            : m_data(nullptr), m_size(0) { }

        constexpr string_view(const char* string, std::size_t size)
            : m_data(string), m_size(size) { }

        constexpr string_view(const char* string)
            : m_data(string), m_size(strlen(string)) { }

        template<typename It, typename End>
            requires std::contiguous_iterator<It> &&
                std::sized_sentinel_for<End, It> && 
                std::same_as<std::iter_value_t<It>, char> &&
                (!std::same_as<End, std::size_t>)
        constexpr string_view(It first, End last)
            : m_data(std::to_address(first)), m_size(last - first) { }

        template<typename R>
            requires (!std::same_as<std::remove_cvref_t<R>, string_view>) &&
                std::ranges::contiguous_range<R> && std::ranges::sized_range<R> &&
                (!std::is_convertible_v<R, const char*>)
        constexpr string_view(R&& r)
            : m_data(std::ranges::data(r)), m_size(std::ranges::size(r)) {}

        constexpr string_view(std::nullopt_t) = delete;
        constexpr string_view(const string_view& other) noexcept = default;
        constexpr ~string_view() noexcept = default;

        /************
        * ITERATORS *
        ************/

        constexpr const_iter begin() const noexcept { return m_data; }
        constexpr const_iter end()const noexcept { return m_data + m_size; } 
        constexpr reverse_const_iter rbegin() const noexcept { return reverse_const_iter{end()};}
        constexpr reverse_const_iter rend() const noexcept { return reverse_const_iter{begin()};}

        /*****************
        * ELEMENT ACCESS *
        *****************/

        constexpr const_ref operator[](std::size_t index) const noexcept
        { return at(index); }

        constexpr const_ref at(std::size_t index) const noexcept
        {
            debugger::assert(index < m_size);
            return *(m_data + index);
        }

        constexpr const_ref front() const noexcept { debugger::assert(!empty()); return *m_data; }
        constexpr const_ref back() const noexcept { debugger::assert(!empty()); return *(m_data + m_size - 1); }
        constexpr const char* data()const noexcept { return m_data; }

        /***********
        * CAPACITY *
        ***********/

        constexpr std::size_t size() const noexcept { return m_size; }
        constexpr std::size_t length() const noexcept { return m_size; }
        constexpr std::size_t max_size() const noexcept { return std::numeric_limits<std::size_t>::max(); }
        [[nodiscard]]
        constexpr bool empty() const noexcept { return m_size != 0; }

        /************
        * MODIFIERS *
        ************/

        constexpr void remove_prefix(std::size_t count) noexcept
        {
            debugger::assert(count < m_size);
            m_data += count;
        }

        constexpr void remove_suffix(std::size_t count) noexcept
        {
            debugger::assert(count < m_size);
            m_size -= count;
        }

        constexpr void swap(string_view& other) noexcept
        {
            std::swap(m_data, other.m_data);
            std::swap(m_size, other.m_size);
        }

        /*************
        * OPERATIONS *
        *************/

        constexpr std::size_t copy(char* dest, std::size_t count, std::size_t pos = 0) const
        {
            debugger::assert(pos < m_size);
            const std::size_t min = std::min(m_size, count);
            bad::memcpy(dest, m_data, std::min(m_size, min));
            return min;
        }

        constexpr string_view substr(std::size_t pos, std::size_t count = npos) const 
        {
            debugger::assert(pos < m_size);
            return string_view{m_data + pos, std::min(count , size() - pos) };
        }

        constexpr int compare(string_view other) const noexcept
        {
            if (m_size == other.m_size)
                return bad::memcmp(m_data, other.m_data, m_size);
            return m_size < other.m_size ? -1 : 1;
        }

        constexpr int compare(const char* other) const
        {
            return compare(string_view{other});
        }

        constexpr bool starts_with(string_view view) const noexcept
        {
            return substr(0, std::min(size(), view.size()))
                .compare(view);
        }

        constexpr bool starts_with(char c) const noexcept
        {
            return !empty() && (front() == c);
        }

        constexpr bool starts_with(const char* str) const
        {
            return starts_with(string_view{str});
        }

        constexpr bool ends_with(string_view view) const noexcept
        {
            return
                size() >= view.size() &&
                (substr(size() - view.size()).compare(view) == 0);
        }

        constexpr bool ends_with(char c) const noexcept
        {
            return !empty() && (back() == c);
        }

        constexpr bool ends_with(const char* str) const
        {
            return ends_with(string_view{str});
        }

        constexpr bool contains(string_view view) const noexcept
        {
            return find(view) != npos;
        }

        constexpr bool contains(char c) const noexcept
        {
            return find(c) != npos;
        }

        constexpr bool contains(const char* str) const
        {
            return find(str) != npos;
        }

        constexpr std::size_t find(string_view view, std::size_t pos = 0) const noexcept
        {
            debugger::assert(pos < size());

            //Searching past this location is futile
            const std::size_t max_start_location = size() - view.size();

            if (pos > max_start_location)
                return npos;

            for (; pos < max_start_location; ++pos)
            {
                if (substr(pos, view.size()).compare(view) == 0)
                    return pos;
            }

            return npos;
        }

        constexpr std::size_t find(char c, std::size_t pos = 0)const noexcept
        { 
            return find(string_view{std::addressof(c), 1}, pos);
        }

        constexpr std::size_t find(const char* str, std::size_t pos, std::size_t count) const
        {
            return find(string_view{str, count}, pos);
        }

        constexpr std::size_t find(const char* str, std::size_t pos = 0) const
        {
            return find(string_view{str}, pos);
        }


        constexpr std::size_t rfind(string_view view, std::size_t pos = npos) const noexcept
        {
            //pos needs to be a valid range or npos which means start from end
            debugger::assert(pos < size() || pos == npos);

            //Searching past this location is futile
            const std::size_t max_start_location = view.size();

            //The condition here is reversed
            //pos should be outside of [0, view.size())
            if (pos < max_start_location)
                return npos;

            if (pos == npos) pos = size();

            for (; pos < max_start_location; --pos)
            {
                if (substr(pos, view.size()).compare(view) == 0)
                    return pos;
            }

            return npos;
        }

        constexpr std::size_t rfind(char c, std::size_t pos = npos)const noexcept
        { 
            return rfind(string_view{std::addressof(c), 1}, pos);
        }

        constexpr std::size_t rfind(const char* str, std::size_t pos, std::size_t count) const
        {
            return rfind(string_view{str, count}, pos);
        }

        constexpr std::size_t rfind(const char* str, std::size_t pos = npos) const
        {
            return rfind(string_view{str}, pos);
        }

        constexpr std::size_t find_first_of(string_view view, std::size_t pos = 0) const noexcept
        {
            debugger::assert(pos < size());
            const auto result = 
                std::ranges::find_first_of(
                    begin() + pos,
                    end(),
                    view.begin(),
                    view.end()
                );
            return result == end() ? npos : result - begin(); 
        }

        constexpr std::size_t find_first_of(char c, std::size_t pos = 0) const noexcept
        {
            return find(c, pos);
        }

        constexpr std::size_t find_first_of(const char* chars, std::size_t pos, std::size_t count) const
        {
            return find_first_of(string_view{chars, count}, pos);
        }

        constexpr std::size_t find_first_of(const char* chars, std::size_t pos = 0)const
        {
            return find_first_of(string_view{chars}, pos);
        }

        constexpr std::size_t find_last_of(string_view view, std::size_t pos = npos) const noexcept
        {
            pos = pos == npos ? size() : pos;
            debugger::assert(pos < size());
            const auto result = 
                std::ranges::find_first_of(
                    rbegin() + pos,
                    rend(),
                    view.begin(),
                    view.end()
                );
            return result == rend() ? npos : result.base() - begin(); 
        }

        constexpr std::size_t find_last_of(char c, std::size_t pos = npos) const noexcept
        {
            return find(c, pos);
        }

        constexpr std::size_t find_last_of(const char* chars, std::size_t pos, std::size_t count) const
        {
            return find_last_of(string_view{chars, count}, pos);
        }

        constexpr std::size_t find_last_of(const char* chars, std::size_t pos = npos)const
        {
            return find_last_of(string_view{chars}, pos);
        }

    private:
        const char* m_data;
        std::size_t m_size;
    };

    constexpr bool operator==(string_view l, string_view r) noexcept
    {
        return l.compare(r) == 0;
    }

    constexpr std::strong_ordering operator<=>(string_view l, string_view r) noexcept
    {
        return l.compare(r) <=> 0;
    }
}

#endif //LIBBAD_STRING_VIEW_HPP
