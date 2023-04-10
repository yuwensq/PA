#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc)
{
  if (ref_r->eax != cpu.eax || ref_r->ecx != cpu.ecx ||
      ref_r->edx != cpu.edx || ref_r->ebx != cpu.ebx ||
      ref_r->esp != cpu.esp || ref_r->ebp != cpu.ebp ||
      ref_r->esi != cpu.esi || ref_r->edi != cpu.edi ||
      ref_r->pc != cpu.pc)
  {
    printf("at pc:0x%08x\n", pc);
    printf("ref_eax: 0x%08x\n", ref_r->eax);
    printf("ref_ecx: 0x%08x\n", ref_r->ecx);
    printf("ref_edx: 0x%08x\n", ref_r->edx);
    printf("ref_ebx: 0x%08x\n", ref_r->ebx);
    printf("ref_esp: 0x%08x\n", ref_r->esp);
    printf("ref_ebp: 0x%08x\n", ref_r->ebp);
    printf("ref_esi: 0x%08x\n", ref_r->esi);
    printf("ref_edi: 0x%08x\n", ref_r->edi);
    printf("ref_eip: 0x%08x\n", ref_r->pc);
    return false;
  }
  return true;
}

void isa_difftest_attach(void)
{
}
