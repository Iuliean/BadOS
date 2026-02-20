add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/font.obj
    COMMAND ld -r -b binary HUGE-VGA.F32 -o ${CMAKE_CURRENT_BINARY_DIR}/font.obj
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS HUGE-VGA.F32
)

add_library(fonts STATIC font.obj)
set_target_properties(fonts PROPERTIES LINKER_LANGUAGE CXX)
