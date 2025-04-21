project (kernel CXX)

add_executable(kernel kmain.cpp)


target_compile_options(kernel PRIVATE "-ffreestanding"
"-Wall" "-Wextra" "-fno-exceptions" "-fno-rtti")

target_link_options(kernel PRIVATE "-ffreestanding" "-nostdlib" -T ${CMAKE_SOURCE_DIR}/toolchain/linker.ld)

target_include_directories(kernel PRIVATE ${limine_INCLUDE_DIR})
target_link_libraries(kernel PRIVATE gcc)

install(TARGETS kernel DESTINATION ${ISO_SYSROOT}/boot)
