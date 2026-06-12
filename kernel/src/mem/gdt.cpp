#include "mem/gdt.hpp"
#include "cpu/cpu.h"
#include "libbad/memory.hpp"


#include "limine.h"

__attribute__((used, section(".limine_requests")))
static volatile limine_kernel_address_request kmapping = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};


namespace os::mem::gdt
{
    static struct gdt gdt_table;

    void init()
    {
        bad::memset(&gdt_table, 0, sizeof(struct gdt));

        gdt_table.code_segment.type = code_eoe;
        gdt_table.code_segment.s = 1;
        gdt_table.code_segment.dpl = 0;
        gdt_table.code_segment.p = 1;
        gdt_table.code_segment.l = 1;
        gdt_table.code_segment.db = 0;

        gdt_table.data_segment.type = data_rwa;
        gdt_table.data_segment.s = 1;
        gdt_table.data_segment.p = 1;


        pseudo_descriptor pseudo_descriptor
        {
            .limit = sizeof(gdt_table) - 1,
            .base_ptr = &gdt_table
        };

        os::cpu::load_gdt_from(&pseudo_descriptor);
    }
}