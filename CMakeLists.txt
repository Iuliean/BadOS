cmake_minimum_required(VERSION 3.30.8)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)
set(ISO_SYSROOT ${CMAKE_INSTALL_PREFIX}/sysroot)
include(cmake/Limine.cmake)

project(BadOS)

add_subdirectory(font)
add_subdirectory(kernel)


add_compile_options(-Wall -Wextra -Werror -Wpedantic)

set(ISO_NAME "${CMAKE_PROJECT_NAME}.iso")

add_custom_target(sysroot SOURCES sysroot/)

install(DIRECTORY sysroot DESTINATION ${CMAKE_INSTALL_PREFIX})
install(CODE "execute_process(COMMAND xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
        -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        ${ISO_SYSROOT} -o ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}.iso)")
install(CODE "execute_process(COMMAND ${limine_SOURCE_DIR}/limine bios-install ${CMAKE_INSTALL_PREFIX}/${CMAKE_PROJECT_NAME}.iso)")
