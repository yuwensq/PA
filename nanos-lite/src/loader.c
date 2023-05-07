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
  //TODO();
  int fd=fs_open(filename,0,0);
  printf("%s\n",filename);
  assert(fd!=-1);
  Elf_Ehdr elf;
  fs_read(fd,(void*)&elf,sizeof(Elf_Ehdr));
  Elf_Phdr phdr[elf.e_phnum];
  for(size_t i=0;i<elf.e_phnum;i++){
    fs_lseek(fd,elf.e_phoff+i*elf.e_phentsize,SEEK_SET);
    fs_read(fd,(void*)&phdr[i],elf.e_phentsize);
    if(phdr[i].p_type==PT_LOAD){
      fs_lseek(fd,phdr[i].p_offset,SEEK_SET);
      fs_read(fd,(void*)phdr[i].p_vaddr,phdr[i].p_filesz);
      memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz),0,phdr[i].p_memsz-phdr[i].p_filesz);
    }
  }
  fs_close(fd);
  return elf.e_entry;
  // TODO();
  // int fd = fs_open(filename, 0, 0);
  // unsigned char buf[2048];
  // Elf_Ehdr elf_head;
  // fs_read(fd, &elf_head, sizeof(Elf_Ehdr));
  // // ramdisk_read(&elf_head, 0, sizeof(Elf_Ehdr));
  // Elf_Phdr elf_phentry;
  // for (int i = 0; i < elf_head.e_phnum; i++)
  // {
  //   fs_lseek(fd, elf_head.e_phoff + i * elf_head.e_phentsize, SEEK_SET);
  //   fs_read(fd, &elf_phentry, elf_head.e_phentsize);
  //   // ramdisk_read(&elf_phentry, i, sizeof(Elf_Phdr));
  //   if (elf_phentry.p_type == PT_LOAD)
  //   {
  //     int len = 0;
  //     size_t file_off = elf_phentry.p_offset;
  //     unsigned char *mem_addr = (unsigned char *)elf_phentry.p_vaddr;
  //     while (len < elf_phentry.p_filesz)
  //     {
  //       int mov_size = (elf_phentry.p_filesz - len > 2048 ? 2048 : elf_phentry.p_filesz - len);
  //       fs_lseek(fd, file_off, SEEK_SET);
  //       fs_read(fd, buf, mov_size);
  //       // ramdisk_read(buf, file_off, mov_size);
  //       memcpy(mem_addr, buf, mov_size);
  //       file_off += mov_size;
  //       mem_addr += mov_size;
  //       len += mov_size;
  //     }
  //     memset(mem_addr, 0, elf_phentry.p_memsz - len);
  //   }
  // }
  // fs_close(fd);
  // return elf_head.e_entry;
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = 0x%x", entry);
  ((void (*)())entry)();
}

void context_kload(PCB *pcb, void *entry)
{
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
