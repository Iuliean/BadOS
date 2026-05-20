project (kernel CXX ASM)

add_executable(kernel
    src/kmain.cpp
    src/framebuffer.cpp

    src/cpu/cpuid.s
    src/cpu/memory.cpp
    src/cpu/gdt.cpp

    src/console/console.cpp
    src/console/font/font.cpp

    include/utils.hpp
    include/error.hpp
    include/framebuffer.hpp

    include/cpu/cpu.h
    include/cpu/gdt.hpp
    include/cpu/memory.hpp
    
    include/console/console.hpp
    include/console/font/font.hpp
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
    "-mabi=sysv"
)

target_link_options(kernel PRIVATE
    "-ffreestanding"
    "-nostdlib"
    "-nostartfiles"
    "-nodefaultlibs"
    -T ${CMAKE_SOURCE_DIR}/toolchain/linker.ld
)

target_include_directories(kernel PRIVATE ${limine_INCLUDE_DIR} include)
target_link_libraries(kernel
    PRIVATE fonts
    libbad
)

install(TARGETS kernel DESTINATION ${ISO_SYSROOT}/boot)
