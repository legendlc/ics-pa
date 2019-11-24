#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t get_ramdisk_size();
size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr ehdr = {0};
  ramdisk_read(&ehdr, 0, sizeof(ehdr));
  
  uintptr_t entry_point = (uintptr_t)ehdr.e_entry;
  printk("ELF entry point: %x\n", ehdr.e_entry);
  printk("ELF program header offset: %d, entsize: %d, number: %d\n", 
    ehdr.e_phoff, ehdr.e_phentsize, ehdr.e_phnum);
  
  for (int i = 0; i < ehdr.e_phnum; i++) {
    Elf_Phdr phdr = {0};
    assert(sizeof(phdr) == ehdr.e_phentsize);
    ramdisk_read(&phdr, ehdr.e_phoff + i * ehdr.e_phentsize, sizeof(phdr));

    if (phdr.p_type != PT_LOAD) {
      continue;
    }
    printk("Program header [%d]: type %x, file offset %d, filesize %d, mem %x, memsize %d\n",
      i, phdr.p_type, phdr.p_offset, phdr.p_filesz, phdr.p_vaddr, phdr.p_memsz);
    
    ramdisk_read((void*)(phdr.p_vaddr), phdr.p_offset, phdr.p_filesz);
    memset((void*)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
  }

  return entry_point;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
