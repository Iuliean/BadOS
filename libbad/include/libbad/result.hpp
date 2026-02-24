#ifndef LIBBAD_RESULT_HPP
#define LIBBAD_RESULT_HPP
#include <expected>

namespace bad
{
    template<typename R, typename E>
    using result = std::expected<R, E>;
}

#endif //LIBBAD_RESULT_HPP
