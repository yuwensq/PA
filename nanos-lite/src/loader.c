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
	int fd=fs_open(filename, 0, 0);//f_offset=get_disk_offset(fd),seg_size;
	Elf_Phdr head_pro;
	Elf_Ehdr head_elf;
	fs_read(fd,&head_elf,sizeof(Elf_Ehdr));
	void *pa;//*va=(void*)head_elf.e_entry;
	size_t siz=head_elf.e_phentsize,cnt=head_elf.e_phnum;
	uintptr_t now_v,v_addr;
	size_t page_num;
	for (int i=0;i<cnt;++i)
	{
		fs_lseek(fd,i*siz+head_elf.e_phoff,0);
		fs_read(fd,&head_pro,siz);
		if (head_pro.p_type!=PT_LOAD) continue;
		fs_lseek(fd,head_pro.p_offset,0);
		v_addr=head_pro.p_vaddr;
		//if ((head_pro.p_vaddr&0xfff)) assert(0);
		page_num=(head_pro.p_filesz-1)/PGSIZE+1;//申请的页数
		for (int j=0;j<page_num;++j)
		{
			pa=new_page(1);
			if ((uintptr_t)pa&0xfff) assert(0);
			_map(&(pcb->as),(void*)(head_pro.p_vaddr+j*PGSIZE),pa,0);
			//fs_read(fd,(void*)(head_pro.p_vaddr),head_pro.p_filesz);
			if (j<page_num-1) fs_read(fd,pa,PGSIZE);
			else fs_read(fd,pa,head_pro.p_filesz-PGSIZE*j);
		}
		//TODO 是否要清空？
		//maxn=(maxn>head_pro.p_memsz+head_pro.p_memsz?head_pro.p_memsz+head_pro.p_vaddr:maxn);
		//if (maxn&0xfff) maxn=((maxn+0xfff)&0xfffff000);
		//continue;
		v_addr+=page_num*PGSIZE;
		if (head_pro.p_filesz==head_pro.p_memsz) {pcb->max_brk=v_addr;continue;}
		
		int zero_len=head_pro.p_memsz-head_pro.p_filesz;
		if (zero_len<PGSIZE*page_num-head_pro.p_filesz)
			memset((void*)(((uintptr_t)pa)+(head_pro.p_filesz-PGSIZE*(page_num-1))),0,zero_len);
		else
		{
			memset((void*)(((uintptr_t)pa)+(head_pro.p_filesz-PGSIZE*(page_num-1))),0,PGSIZE*page_num-head_pro.p_filesz);
			zero_len-=(PGSIZE*page_num-head_pro.p_filesz);
			now_v=head_pro.p_vaddr+page_num*PGSIZE;
			page_num=(zero_len-1)/PGSIZE+1;
			for (int j=0;j<page_num;++j)
			{
				pa=new_page(1);
				_map(&(pcb->as),(void*)(now_v),pa,0);
				if ((uintptr_t)pa&0xfff) assert(0);
				if (j<page_num-1) memset(pa,0,PGSIZE);
				else memset(pa,0,zero_len);
				now_v+=PGSIZE;
				zero_len-=PGSIZE;
			}
			v_addr+=page_num*PGSIZE;
		}
		pcb->max_brk=v_addr;
		//memset((void*)(((uintptr_t)pa)+head_pro.p_filesz),0,head_pro.p_memsz-head_pro.p_filesz);
	}
	fs_close(fd);
	//current->max_brk=maxn;
	return head_elf.e_entry;
  // const int page_size = 4096;
  // int fd = fs_open(filename, 0, 0);
  // Elf_Ehdr elf_head;
  // fs_read(fd, &elf_head, sizeof(Elf_Ehdr));
  // Elf_Phdr elf_phentry;
  // for (int i = 0; i < elf_head.e_phnum; i++)
  // {
  //   fs_lseek(fd, elf_head.e_phoff + i * elf_head.e_phentsize, SEEK_SET);
  //   fs_read(fd, &elf_phentry, elf_head.e_phentsize);
  //   if (elf_phentry.p_type == PT_LOAD)
  //   {
  //     int len = 0;
  //     size_t file_off = elf_phentry.p_offset;
  //     fs_lseek(fd, file_off, SEEK_SET);
  //     unsigned char *v_addr = (unsigned char *)elf_phentry.p_vaddr;
  //     void *p_addr = 0;
  //     while (len < elf_phentry.p_filesz)
  //     {
  //       int mov_size = (elf_phentry.p_filesz - len > page_size ? page_size
  //                                                              : elf_phentry.p_filesz - len);
  //       int gap = page_size - ((uint32_t)v_addr & 0xfff);
  //       if (mov_size > gap)
  //         mov_size = gap;
  //       // 原来的框架写的还挺方便
  //       p_addr = new_page(1);
  //       p_addr = (void *)((uint32_t)p_addr | ((uint32_t)v_addr & 0xfff));
  //       _map(&pcb->as, v_addr, p_addr, _PROT_EXEC);
  //       fs_read(fd, p_addr, mov_size);
  //       v_addr += mov_size;
  //       p_addr += mov_size;
  //       len += mov_size;
  //     }
  //     int gap = page_size - ((uint32_t)v_addr & 0xfff);
  //     if (gap != page_size)
  //     {
  //       memset(p_addr, 0, gap);
  //       v_addr += gap;
  //       len += gap;
  //     }
  //     while (len < elf_phentry.p_memsz)
  //     {
  //       int mov_size = (elf_phentry.p_memsz - len > page_size ? page_size
  //                                                             : elf_phentry.p_memsz - len);
  //       p_addr = new_page(1);
  //       _map(&pcb->as, v_addr, p_addr, _PROT_EXEC);
  //       memset(p_addr, 0, mov_size);
  //       v_addr += mov_size;
  //       len += mov_size;
  //     }
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
