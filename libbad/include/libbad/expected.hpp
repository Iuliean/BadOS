#ifndef LIBBAD_EXPECTED_HPP
#define LIBBAD_EXPECTED_HPP

#include "debugger.hpp"
#include <concepts>
#include <expected>
#include <memory>
#include <type_traits>
#include <utility>
namespace bad
{

    template<typename T>
        requires (!std::is_void_v<T> && !std::is_reference_v<T>)
    class unexpected
    {
    public:
        constexpr unexpected(const unexpected& other) = default;
        constexpr unexpected(unexpected&& other) = default;

        template<typename Err = T>
            requires (
                !std::is_same_v<std::remove_cvref_t<Err>, unexpected> &&
                !std::is_same_v<std::remove_cvref_t<Err>, std::in_place_t> &&
                std::is_constructible_v<T, Err>
            )
        constexpr explicit unexpected(Err&& error)
            : m_member(std::forward<Err>(error)) { }

        template<typename ...Args>
            requires std::is_constructible_v<T, Args...>
        constexpr explicit unexpected(std::in_place_t, Args&&... args)
            : m_member( std::forward<Args>(args)...) { }

        constexpr auto&& error(this auto&& self) noexcept
        {
            return std::forward<decltype(self)>(self).m_member;
        }

        constexpr void swap(unexpected<T>& other) noexcept(std::is_nothrow_swappable_v<T>)
            requires std::is_swappable_v<T>
        {
            std::swap(error(), other.error());
        }

        template<typename E2>
        friend constexpr bool operator==(const unexpected& lhs, const unexpected<E2> rhs);

    private:
        T m_member;
    };


    template<typename T>
    unexpected(T&&) -> unexpected<std::remove_cvref_t<T>>;

    struct unexpected_t { };

    template<typename E1, typename E2>
    constexpr bool operator==(const unexpected<E1>& lhs, const unexpected<E2> rhs)
        requires std::equality_comparable_with<E1, E2>
    {
        return lhs.m_member == rhs.m_member;
    }

    template<typename E, typename U>
    class expected
    {
    public:
        /*
        References are dealth with via pointers
        This helps with building the correct thing throughout the code
        */
        using held_type = std::conditional_t<std::is_reference_v<E>, std::remove_reference_t<E>*, E>;
        using error_t = unexpected<U>;

        constexpr expected() noexcept(std::is_nothrow_default_constructible_v<E>)
            requires (
                (
                    !std::is_reference_v<E>||
                    !std::is_pointer_v<E>
                )&&
                std::is_default_constructible_v<E>
            )
            : m_has_value(true), m_value() { }

        constexpr expected(const expected& other)
            noexcept(std::is_nothrow_copy_constructible_v<E> && std::is_nothrow_copy_constructible_v<U>)
            requires std::copy_constructible<E> && std::copy_constructible<U>
            : m_has_value(other.m_has_value)
        {
            if (m_has_value)
            {
                if constexpr (std::is_reference_v<E>)
                    new (&m_value) held_type { std::addressof(other.m_value) };
                else
                    new (&m_value) held_type { other.m_value };
            }
            else
                new (&m_unexpected) error_t { other.error() };
        }

        constexpr expected(expected&& other) 
            noexcept (std::is_nothrow_move_constructible_v<E> && std::is_nothrow_move_constructible_v<U>)
            requires std::move_constructible<E> && std::move_constructible<U>
            : m_has_value(other.m_has_value)
        {
            if(m_has_value)
            {
                if constexpr (std::is_reference_v<E>)
                    new (&m_has_value) held_type{ std::addressof(other.m_value) };
                else
                    new (&m_has_value) held_type { std::move(other.m_value) };
            }
            else
                new (&m_unexpected) error_t { other.error() };
        }

        template<typename F = std::remove_cv_t<E>>
            requires (
                std::is_convertible_v<F, E> &&
                !std::is_same_v<F, std::in_place_t> &&
                !std::is_same_v<expected, F> &&
                std::is_constructible_v<E, F> &&
                !std::is_reference_v<E>
            )
        constexpr expected(F&& value) noexcept(std::is_nothrow_constructible_v<E, F>)
            : m_has_value(true), m_value(std::forward<F>(value)) { }

        template<typename F = std::remove_cv_t<E>>
            requires (
                std::is_convertible_v<F, E> &&
                !std::is_same_v<F, std::in_place_t> &&
                !std::is_same_v<expected, F> &&
                std::is_constructible_v<E, F>
            )
        constexpr expected(F&& value) noexcept(std::is_nothrow_constructible_v<E, F>)
            : m_has_value(true), m_value(std::addressof(value)) { }

        template<typename G>
        constexpr expected(const unexpected<G>& err) noexcept (std::is_nothrow_constructible_v<U, G>)
            requires std::is_constructible_v<U, G>
            : m_has_value(false), m_unexpected(err.error()) { }

        template<typename G>
            requires std::is_constructible_v<U, G>
        constexpr expected(unexpected<G>&& err) noexcept (std::is_nothrow_constructible_v<U, G>)
            : m_has_value(false), m_unexpected(std::move(err).error()) { }

        template<typename ...Args>
        constexpr expected (std::in_place_t, Args&&... args)
            noexcept (std::is_nothrow_constructible_v<E, Args...>)
            requires (!std::is_reference_v<E>)
            : m_has_value(true), m_value(std::forward<Args>(args)...) { }

        template <typename ...Args>
        constexpr expected(unexpected_t, Args&&... args)
            noexcept (std::is_nothrow_constructible_v<U, Args...>)
            requires (!std::is_reference_v<U>)
            : m_has_value(false), m_unexpected(std::forward<Args>(args)...) { }

        /************
        * OBSERVERS *
        ************/

        constexpr bool has_value() const noexcept { return m_has_value; }
        constexpr operator bool() const noexcept { return has_value(); }

        constexpr auto operator->(this auto&& self) noexcept
        {
            debugger::assert(self.has_value());
            if constexpr (std::is_reference_v<E>) return self.m_value;
            return std::addressof(self.m_value);
        }

        constexpr auto&& operator*(this auto&& self) noexcept
        {
            return std::forward<decltype(self)>(self).value();
        }

        constexpr auto&& value(this auto&& self) noexcept
        {
            if constexpr (std::is_reference_v<E>)
                return *self.m_value;
            else
                return std::forward<decltype(self)>(self).m_value;
        }

        template<typename D = std::remove_cv_t<E>>
            requires (!std::is_reference_v<E> && std::copy_constructible<E> && std::is_convertible_v<D, E>)
        constexpr E value_or(D&& default_value) const& noexcept(std::is_nothrow_copy_constructible_v<E>)
        {
            return has_value() ? **this : static_cast<E>(std::forward<D>(default_value));
        }

        template<typename D = std::remove_cv_t<E>>
            requires (!std::is_reference_v<E> && std::move_constructible<E> && std::is_convertible_v<D, E>)
        constexpr E value_or(D&& default_value) && noexcept(std::is_nothrow_move_constructible_v<E>)
        {
            return has_value() ? std::move(**this) : static_cast<E>(std::forward<D>(default_value));
        }

        template<typename D = std::remove_cv_t<E>>
            requires (std::is_reference_v<E> && std::copy_constructible<E>)
        constexpr E value_or(D& default_value) const noexcept
        {
            return has_value() ? value() : default_value;
        }

        constexpr auto&& error (this auto&& self) noexcept
        {
            debugger::assert(!self.has_value());
            return std::forward<decltype(self)>(self).m_unexpected.error();
        }

        template<typename D = U>
        constexpr U error_or(D&& default_value) const& noexcept(std::is_nothrow_copy_constructible_v<U>)
        {
            return !has_value() ? error() : static_cast<U>(std::forward<D>(default_value));
        }

        template<typename D = U>
        constexpr U error_or(D&& default_value) && noexcept(std::is_nothrow_move_constructible_v<U>)
        {
            return !has_value() ? std::move(error()) : static_cast<U>(std::forward<D>(default_value));
        }

        /*********************
        * MONADIC OPERATIONS *
        *********************/
        template<typename FuncT>
        constexpr void and_then(FuncT&& f) { static_assert(true); }
        template<typename FuncT>
        constexpr void transform(FuncT&& f) { static_assert(true); }
        template<typename FuncT>
        constexpr void or_else(FuncT&& f) { static_assert(true); }
        template<typename FuncT>
        constexpr void transform_error(FuncT&& f) { static_assert(true); }
    private:
        bool m_has_value;
        union
        {
            held_type m_value;
            error_t m_unexpected;
        };
    };

    template<typename U>
    class expected<void, U>
    {
    public:
        using error_t = unexpected<U>;
        constexpr expected() noexcept
            : m_has_value(true) { }
        
        constexpr expected(const expected& other) noexcept(std::is_nothrow_copy_constructible_v<U>)
            requires (std::is_copy_constructible_v<U>)
            : m_has_value(other.has_value())
        {
            if (!has_value())
                std::construct_at(&m_unexpected, other);
        }

        constexpr expected(expected&& other) noexcept (std::is_nothrow_move_constructible_v<U>)
            : m_has_value(other.has_value())
        {
            if(!has_value())
                std::construct_at(&m_unexpected, std::move(other.m_unexpected));
        }


        template<typename G>
        constexpr expected(const unexpected<G>& err) noexcept (std::is_nothrow_constructible_v<U, G>)
            requires std::is_constructible_v<U, G>
            : m_has_value(false), m_unexpected(err.error()) { }

        template<typename G>
            requires std::is_constructible_v<U, G>
        constexpr expected(unexpected<G>&& err) noexcept (std::is_nothrow_constructible_v<U, G>)
            : m_has_value(false), m_unexpected(std::move(err).error()) { }

        /************
        * OBSERVERS *
        ************/
        constexpr void operator*() const noexcept
        {
            debugger::assert(has_value());
        }

        constexpr bool has_value() const noexcept { return m_has_value; }
        constexpr operator bool() const noexcept { return has_value(); }

        constexpr void value([[maybe_unused]]this auto&& self) noexcept
        {
            debugger::assert(self.has_value());
        }

        constexpr auto&& error(this auto&& self) noexcept
        {
            debugger::assert(!self.has_value());
            return std::forward_like<decltype(self)>(self.m_unexpected.error());
        }

        constexpr void value_or() = delete;

        template<typename D = U>
        constexpr U error_or(D&& default_value) const& noexcept(std::is_nothrow_copy_constructible_v<U>)
        {
            return !has_value() ? error() : static_cast<U>(std::forward<D>(default_value));
        }

        template<typename D = U>
        constexpr U error_or(D&& default_value) && noexcept(std::is_nothrow_move_constructible_v<U>)
        {
            return !has_value() ? std::move(error()) : static_cast<U>(std::forward<D>(default_value));
        }

        /*********************
        * MONADIC OPERATIONS *
        *********************/
        template<typename FuncT>
        constexpr void and_then(FuncT&& f) { static_assert(true); }
        template<typename FuncT>
        constexpr void transform(FuncT&& f) { static_assert(true); }
        template<typename FuncT>
        constexpr void or_else(FuncT&& f) { static_assert(true); }
        template<typename FuncT>
        constexpr void transform_error(FuncT&& f) { static_assert(true); }
    private:
        bool m_has_value;
        union{
            error_t m_unexpected;
        };
    };
}

#endif  //LIBBAD_EXPECTED_HPP