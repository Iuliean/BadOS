include(ExternalProject)

set(limine_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/limine)
set(limine_INCLUDE_DIR ${limine_SOURCE_DIR})

ExternalProject_Add(
    limine
    GIT_REPOSITORY https://github.com/limine-bootloader/limine.git
    GIT_TAG origin/v9.x-binary
    GIT_SHALLOW true
    SOURCE_DIR ${limine_SOURCE_DIR}
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE true
    BUILD_COMMAND make all
    INSTALL_COMMAND ""
)

install(FILES ${limine_SOURCE_DIR}/limine-bios.sys
              ${limine_SOURCE_DIR}/limine-bios-cd.bin
              ${limine_SOURCE_DIR}/limine-uefi-cd.bin
              DESTINATION ${ISO_SYSROOT}/boot/limine)


