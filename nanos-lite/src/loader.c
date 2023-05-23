#include "proc.h"
#include "fs.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename)
{
  // TODO();
  const int page_size = 4096;
  int fd = fs_open(filename, 0, 0);
  Elf_Ehdr elf_head;
  fs_read(fd, &elf_head, sizeof(Elf_Ehdr));
  Elf_Phdr elf_phentry;
  for (int i = 0; i < elf_head.e_phnum; i++)
  {
    fs_lseek(fd, elf_head.e_phoff + i * elf_head.e_phentsize, SEEK_SET);
    fs_read(fd, &elf_phentry, elf_head.e_phentsize);
    if (elf_phentry.p_type == PT_LOAD)
    {
      int len = 0;
      size_t file_off = elf_phentry.p_offset;
      fs_lseek(fd, file_off, SEEK_SET);
      unsigned char *v_addr = (unsigned char *)elf_phentry.p_vaddr;
      while (len < elf_phentry.p_filesz)
      {
        int mov_size = (elf_phentry.p_filesz - len > page_size ? page_size
                                                               : elf_phentry.p_filesz - len);
        int gap = page_size - ((uint32_t)v_addr & 0xfff);
        if (mov_size > gap)
          mov_size = gap;
        // 原来的框架写的还挺方便
        void *p_addr = new_page(1);
        p_addr = (void *)((uint32_t)p_addr | ((uint32_t)v_addr & 0xfff));
        _map(&pcb->as, v_addr, p_addr, _PROT_EXEC);
        fs_read(fd, p_addr, mov_size);
        v_addr += mov_size;
        len += mov_size;
      }
    }
  }
  fs_close(fd);
  return elf_head.e_entry;
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = 0x%x", entry);
  ((void (*)())entry)();
}

void context_kload(PCB *pcb, void *entry, void *arg)
{
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, arg);
}

void context_uload(PCB *pcb, const char *filename, int argc, char *const argv[], char *const envp[])
{
  _protect(&pcb->as);
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, argc, argv, envp);
}
