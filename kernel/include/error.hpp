#ifndef ERROR_HPP
#define ERROR_HPP
#include <cstddef>
#include <expected>
#include <string_view>
#include <type_traits>
#include <source_location>

namespace os
{
    enum class generic_errors
    {
        out_of_bounds_access
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

        constexpr std::string_view message() const { return {m_message};}
        constexpr std::string_view file() const { return {m_source.file_name()}; }
        constexpr std::string_view function() const { return {m_source.function_name()}; }
        constexpr std::size_t line() const { return static_cast<std::size_t>(m_source.line()); }

    private:
        ErrNamespace m_type;
        const char* m_message;
        std::source_location m_source;
    };

    using kernel_error = error<generic_errors>;

    template<typename T>
    using result = std::expected<T, kernel_error>;

    template<typename N>
        requires std::is_enum_v<N>
    inline error<N> make_error(N type, const char* message, std::source_location source = {})
    {
        return error<N>{type, message, std::move(source)};
    }

    template<typename N>
    inline std::unexpected<error<N>> make_unexpected_error(N type, const char* message, std::source_location source = {})
    {
        return std::unexpected<error<N>>{make_error(type, message, std::move(source)) };
    }
}

#endif //ERROR_HPP
