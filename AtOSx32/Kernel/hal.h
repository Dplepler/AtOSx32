#ifndef HAL_H
#define HAL_H

#include <stddef.h>
#include <stdint.h>

uint8_t inportb(uint16_t port);
void outportb(uint16_t port, uint8_t value);

static inline void flush_tlb_single(void* addr) {
  __asm__ __volatile__ ("invlpg (%0)" ::"r" (addr) : "memory");
}

static inline void cpu_load_idt(void* addr) {
  __asm__ ("lidt (%0)\n" :: "r" (addr));
}

static inline void cpu_load_gdt(void* addr) {
  __asm__ ("lgdt (%0)\n" :: "r" (addr));
}

static inline void cpu_load_tss(void* addr) {
  __asm__ __volatile__ ("ltr %%ax"::"a"(addr));
}

/* Set interrupts */
static inline void sti() {
  __asm__ ("sti");
}

/* Clear interrupts (disable them) */
static inline void cli() {
  __asm__ ("cli");
}

static inline void enable_nmi() {
  outportb(0x70, inportb(0x70) & 0x7F); // Enable the non maskable interrupt
  inportb(0x71);                        // Clear port

}

static inline void disable_nmi() {
  outportb(0x70, inportb(0x70) | 0x80); // Disable the non maskable interrupt
  inportb(0x71);                        // Clear port
}

#endif
