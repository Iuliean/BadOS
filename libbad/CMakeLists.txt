project (libbad CXX)

add_library(libbad STATIC
    src/memory.cpp

    include/libbad/debugger.hpp
    include/libbad/span.hpp
    include/libbad/string_view.hpp
    include/libbad/color.hpp
    include/libbad/memory.hpp
    include/libbad/fixed_string.hpp
    include/libbad/algorithm.hpp
    include/libbad/error.hpp
    include/libbad/expected.hpp
)

target_include_directories(libbad
    PRIVATE include/libbad
    PUBLIC include
)
