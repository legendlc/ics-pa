#include "proc.h"
#include <elf.h>
#include <fs.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  assert(fd >= 0);

  Elf_Ehdr ehdr = {0};
  fs_read(fd, &ehdr, sizeof(ehdr));
  uintptr_t entry_point = (uintptr_t)ehdr.e_entry;
  printk("ELF entry point: %x\n", ehdr.e_entry);
  printk("ELF program header offset: %d, entsize: %d, number: %d\n", 
    ehdr.e_phoff, ehdr.e_phentsize, ehdr.e_phnum);
  
  fs_lseek(fd, ehdr.e_phoff, SEEK_SET);
  for (int i = 0; i < ehdr.e_phnum; i++) {
    Elf_Phdr phdr = {0};
    assert(sizeof(phdr) == ehdr.e_phentsize);
    
    fs_read(fd, &phdr, sizeof(phdr));
    if (phdr.p_type != PT_LOAD) {
      continue;
    }
    size_t read_offset = fs_lseek(fd, 0, SEEK_CUR);
    printk("Program header [%d]: type %x, file offset %x, filesize %x, mem %x, memsize %x\n",
      i, phdr.p_type, phdr.p_offset, phdr.p_filesz, phdr.p_vaddr, phdr.p_memsz);
    
    fs_lseek(fd, phdr.p_offset, SEEK_SET);
    fs_read(fd, (void*)(phdr.p_vaddr), phdr.p_filesz);
    memset((void*)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);

    fs_lseek(fd, read_offset, SEEK_SET);
  }

  fs_close(fd);
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
