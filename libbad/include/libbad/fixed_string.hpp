#ifndef LIBBAD_STRING_HPP
#define LIBBAD_STRING_HPP
#include <iterator>
#include <limits>
#include <utility>

#include "debugger.hpp"
#include "memory.hpp"
#include "error.hpp"
#include "algorithm.hpp"
#include "string_view.hpp"

namespace bad
{
    template<std::size_t Capacity>
    class fixed_string
    {
    public:
        inline static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

        using ref = char&;
        using iterator = char*;
        using const_ref = const char&;
        using const_iterator = const char*;
        using reverse_const_iter = std::reverse_iterator<const_iterator>;
        using SelfT = fixed_string<Capacity>;

        constexpr fixed_string()noexcept
            : m_length(0)
        {
            bad::memset(m_data, '\0', capacity());
        }

        constexpr fixed_string(const char* string, std::size_t size) noexcept
        {
            assign(string, size);
        }

        constexpr fixed_string(const char* string) noexcept 
            : fixed_string(string, strlen(string)) { }

        constexpr fixed_string(string_view string) noexcept
            : fixed_string(string.data(), string.size()) { }

        /************
        * ITERATORS *
        ************/


        constexpr auto begin(this auto&& self) noexcept { return &self.m_data[0]; }
        constexpr auto end(this auto&& self) noexcept { return &self.m_data[self.m_length + 1]; }
        constexpr auto rbegin(this auto&& self) noexcept { return reverse_const_iter{self.end()}; }
        constexpr auto rend(this auto&& self) noexcept { return reverse_const_iter{self.begin()}; }


        /*****************
        * ELEMENT ACCESS *
        *****************/

        constexpr auto&& operator[](this auto&& self, std::size_t index) noexcept
        { return std::forward<decltype(self)>(self).at(index); }

        constexpr auto&& at (this auto&& self, std::size_t index) noexcept
        {
            debugger::assert(index <= self.m_length);
            debugger::assert(index <= self.capacity());
            return std::forward<decltype(self)>(self).m_data[index];
        }

        constexpr auto&& front(this auto&& self) noexcept
        {
            debugger::assert(!self.empty());
            return std::forward<decltype(self)>(self).m_data[0];
        }

        constexpr auto&& back(this auto&& self) noexcept
        {
            debugger::assert(!self.empty());
            return std::forward<decltype(self)>(self).m_data[self.m_length];
        }

        constexpr auto data(this auto&& self) noexcept
        { return std::forward<decltype(self)>(self).m_data; }

        constexpr const char* c_str() const noexcept { return m_data; }
        constexpr operator string_view() { return string_view{m_data, m_length}; }

        /***********
        * CAPACITY *
        ***********/

        constexpr std::size_t size() const noexcept { return m_length; }
        constexpr std::size_t length() const noexcept { return m_length; }
        consteval std::size_t capacity() const noexcept { return Capacity; }
        consteval std::size_t max_size() const noexcept { return capacity() - 1; }
        [[nodiscard]]
        constexpr bool empty() const noexcept { return m_length == 0; }

        /*************
        * ASSIGNMENT *
        *************/

        constexpr void assign(const char* string, std::size_t size) noexcept
        {
            m_length = min(size, max_size());
            bad::memcpy(m_data, string, m_length);
            m_data[min(m_length + 1, capacity())] = '\0';
        }

        constexpr void assign(const char* string) noexcept { assign(string, strlen(string)); }
        constexpr void assign(string_view string) noexcept { assign(string.data(), string.size()); }

        /************
        * MODIFIERS *
        ************/

        constexpr result<SelfT&> erase(std::size_t index = 0, std::size_t count = npos)
        {
            if ( index > size() || index > capacity())
                return make_unexpected_error(
                        generic_errors::out_of_bounds_access,
                        "Index out of bounds"
                    );
            const std::size_t final_position = index + min(count, size());
            memcpy(
                m_data + index,
                m_data + final_position,
                count
            );
            m_length -= count;

            return result<fixed_string&>{ *this };
        }

        constexpr result<iterator> erase(const_iterator position)
        {
            const std::size_t index = position - m_data;
            erase(index, 1);
            return length() == 0 ? end() : position;
        }

        constexpr result<iterator> erase(iterator position)
        {
            return erase(static_cast<const_iterator>(position));
        }

        constexpr result<iterator> erase(const_iterator first, const_iterator last)
        {
            const std::size_t index = first - m_data;
            const std::size_t count = last - first;
            erase(index, count);
            return length() == 0 ? end() : last;
        }

        constexpr result<iterator> erase(iterator first, iterator last)
        {
            return erase(static_cast<const_iterator>(first), static_cast<const_iterator>(last));
        }

        constexpr void clear() { erase(begin(), end()); }

        constexpr result<SelfT&> insert(std::size_t index, std::size_t count, char c)
        {
            RETURN_IF_UNEXPECTED(shift_index_right(index, count));

            memset(m_data + index, c, count);
            return result<SelfT&>{ *this };
        }

        constexpr result<SelfT&> insert(std::size_t index, string_view string, std::size_t count)
        {
            RETURN_IF_UNEXPECTED(shift_index_right(count));
            memcpy(m_data + index, string.data(), count);
            return result<SelfT&> { *this };
        }

        constexpr result<SelfT&> insert(std::size_t index, string_view string) { return insert(index, string, string.length()); }

        template<std::size_t N>
        constexpr result<SelfT&> insert(std::size_t index, fixed_string<N> string, std::size_t s_index, std::size_t count = npos)
        {
            insert(index, string_view{string}.substr(index, count));
        }
    private:

        constexpr result<void> shift_index_right(std::size_t index, std::size_t count = 0)
        {
            if (count + size() > max_size())
                return make_unexpected_error(
                    generic_errors::length_error,
                    "Size + count goes exceeds max_size()"
                );

            memmove(
                m_data + index + count,
                m_data + index,
                count
            );
            m_length += count;

            return result<void>{}; 
        }
        char m_data[Capacity];
        std::size_t m_length;
    };
}

#endif //LIBBAD_STRING_HPP