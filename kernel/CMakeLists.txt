project (kernel CXX)

add_executable(kernel
    src/kmain.cpp
    include/memory.hpp
)


target_compile_options(kernel PRIVATE
    "-ffreestanding"
    "-fno-asynchronous-unwind-tables"
    "-g3"
    "-Wall"
    "-Wextra"
    "-fno-exceptions"
    "-fno-rtti"
    "-mcmodel=large"
)

target_link_options(kernel PRIVATE
    "-ffreestanding"
    "-nostdlib"
    "-nostartfiles"
    "-nodefaultlibs"
    -T ${CMAKE_SOURCE_DIR}/toolchain/linker.ld
)

target_include_directories(kernel PRIVATE ${limine_INCLUDE_DIR} include)
target_link_libraries(kernel)

install(TARGETS kernel DESTINATION ${ISO_SYSROOT}/boot)
