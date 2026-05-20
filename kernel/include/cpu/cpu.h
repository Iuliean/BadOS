#ifndef CPU_H
#define CPU_H

extern "C" int is_pml5_supported();
extern "C" void load_gdt_from(void* gdt);

#endif //CPU_H