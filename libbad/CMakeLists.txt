project (libbad CXX)

add_library(libbad STATIC
    src/memory.cpp
    include/libbad/debugger.hpp
    include/libbad/span.hpp
    include/libbad/color.hpp
    include/libbad/result.hpp
    include/libbad/memory.hpp
)

target_include_directories(libbad
    PRIVATE include/libbad
    PUBLIC include
)
