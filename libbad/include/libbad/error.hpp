#ifndef LIBBAD_ERROR_HPP
#define LIBBAD_ERROR_HPP
#include <cstddef>
#include <type_traits>
#include <source_location>

#include "expected.hpp"
#include "string_view.hpp"


#define RETURN_IF_UNEXPECTED(v) do{\
    auto __macro_result = v;\
    if(!__macro_result) return bad::unexpected {std::move(__macro_result.error())};\
}while(0)

#define RETURN_IF_EXPECTED(v) do{\
    auto __macro_result = v;\
    if(__macro_result) return __macro_result;\
}while(0)


namespace bad
{

    enum class generic_errors
    {
        out_of_bounds_access,
        length_error
    };

    /* @class error
     * @brief The class acts as a generic container for multiple types of error.
     * Each error group is defined by the error space namspace which is specified as a template parameter
     * @tparam ErrNamespace a enum that contains all possible errors
     */
    template<typename ErrNamespace>
        requires std::is_enum_v<ErrNamespace>
    class error
    {
    public:
        constexpr explicit error(ErrNamespace type, const char* message, std::source_location source = {})
            : m_type(type),
              m_message(message),
              m_source(std::move(source))
        {}
        ~error() = default;

        constexpr string_view message() const { return {m_message};}
        constexpr string_view file() const { return {m_source.file_name()}; }
        constexpr string_view function() const { return {m_source.function_name()}; }
        constexpr std::size_t line() const { return static_cast<std::size_t>(m_source.line()); }

    private:
        ErrNamespace m_type;
        const char* m_message;
        std::source_location m_source;
    };


    template<typename E>
    using result = expected<E, error<generic_errors>>;


    template<typename N>
        requires std::is_enum_v<N>
    inline error<N> make_error(N type, const char* message, std::source_location source = {})
    {
        return error<N>{type, message, std::move(source)};
    }

    template<typename N>
    inline unexpected<error<N>> make_unexpected_error(N type, const char* message, std::source_location source = {})
    {
        return unexpected<error<N>>{make_error(type, message, std::move(source)) };
    }
}

#endif //ERROR_HPP
