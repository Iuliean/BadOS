#ifndef GDT_HPP
#define GDT_HPP
#include "utils.hpp"
#include <cstddef>
#include <cstdint>


/*
    struct PACKED gdt_entry_protected
    {
        std::uint16_t segment_limit;
        std::uint16_t base_addr;
        std::uint8_t base_addr2;
        std::uint8_t type : 3;
        std::uint8_t s : 1;
        std::uint8_t dpl : 2;
        std::uint8_t p : 1;
        std::uint8_t segment_limit2 : 4;
        std::uint8_t avl : 1;
        std::uint8_t l : 1;
        std::uint8_t db : 1;
        std::uint8_t g : 1;
        std::uint8_t base_addr3;

    };
*/

enum gdt_entry_type : std::uint8_t
{
    data_ro   = 0x00, //  data segement read-only
    data_roa  = 0x01, //  data segement read-only, accessed
    data_rw   = 0x02, //  data segment read/write
    data_rwa  = 0x03, //  data segment read/write accessed
    data_roe  = 0x04, //  data segment read-only expand down
    data_roae = 0x05, //  data segment read-only accessed expand down
    data_rwe  = 0x06, //  data segment read/write expand down
    data_rwae = 0x07, //  data segment read/write accessed expand down
    
    code_eo   = 0x08, //  code segement execute-only
    code_eoa  = 0x09, //  code segement execute-only, accessed
    code_er   = 0x0a, //  code segment execute/read
    code_era  = 0x0b, //  code segment execute/read accessed
    code_eoe  = 0x0c, //  code segment execute-only expand down
    code_eoae = 0x0d, //  code segment execute-only accessed expand down
    code_ere  = 0x0e, //  code segment execute/read expand down
    code_erae = 0x0f, //  code segment execute/read accessed expand down
};

struct PACKED gdt_entry
{
    std::uint16_t segment_limit;
    std::uint16_t base_addr;
    std::uint8_t base_addr2;
    gdt_entry_type type : 4;
    std::uint8_t s : 1;
    std::uint8_t dpl : 2;
    std::uint8_t p : 1;
    std::uint8_t segment_limit2 : 4;
    std::uint8_t avl : 1;
    std::uint8_t l : 1;
    std::uint8_t db : 1;
    std::uint8_t g : 1;
    std::uint8_t base_addr3;
};

struct PACKED gdt
{
    std::uint64_t null;
    std::uint64_t code_segment;
    std::uint64_t data_segment;
};

struct PACKED gdt_pseudo_descriptor
{
    std::uint16_t limit;
    gdt* base_ptr;
};

namespace os::gdt
{
    void init();
}

#endif //GDT_HPP