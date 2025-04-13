cmake_minimum_required(VERSION 3.30.8)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)
set(ISO_SYSROOT ${CMAKE_INSTALL_PREFIX}/sysroot)

find_program(GRUB2-MKRESCUE "grub2-mkrescue")

if(NOT GRUB2-MKRESCUE)
    message(FATAL_ERROR "Failed to find grub2-mkrescue")
endif()

add_subdirectory(kernel)

project(BadOS)

set(ISO_NAME "${CMAKE_PROJECT_NAME}.iso")

add_custom_target(sysroot SOURCES sysroot/ DEPENDS kernel)

install(DIRECTORY sysroot DESTINATION ${CMAKE_INSTALL_PREFIX})
install(CODE "execute_process(COMMAND grub2-mkrescue -o ${CMAKE_INSTALL_PREFIX}/BadOS.iso ${ISO_SYSROOT})")