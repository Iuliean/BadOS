#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <bitset>
#include "console/console.hpp"
#include "console/font/font.hpp"
#include "cpu/gdt.hpp"
#include "framebuffer.hpp"
#include "libbad/color.hpp"
#include "libbad/string_view.hpp"
#include "limine.h"
#include "cpu/cpu.h"
#include <span>


// Set the base revision to 3, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.
__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request limine_map = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};



static bad::string_view map_type_to_string(std::uint64_t v)
{
    switch(v)
    {
        case 0: return bad::string_view{"USABLE"};
        case 1: return bad::string_view{"RESERVED"};
        case 2: return bad::string_view{"ACPI RECLAIMABLE"};
        case 3: return bad::string_view{"ACPI_NVS"};
        case 4: return bad::string_view{"BAD MEMORY"};
        case 5: return bad::string_view{"BOOT RECLAIMABLE"};
        case 6: return bad::string_view{"EXE AND MODULES"};
        case 7: return bad::string_view{"FRAMEBUFFER"};
        case 8: return bad::string_view{"RESERVED MAPPED"};
    }
}

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

extern void (*__init_array[])();
extern void (*__init_array_end[])();

// Halt and catch fire function.
static void hcf(void) noexcept{
    for (;;) {
        asm ("hlt");
    }
}

// The following will be our kernel's entry point.d
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
extern "C" void kmain(void) {
    // Call global constructors.
    for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
        __init_array[i]();
    }
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    os::gdt::init();

    for(std::size_t count = 0; count < limine_map.response->entry_count; ++count)
    {
        const auto& entry = limine_map.response->entries[count];
        os::console::get_console().println(map_type_to_string(entry->type));
    }


    hcf();
}
