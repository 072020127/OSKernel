#include "types.h"
#include "param.h"
#include "memorylayout.h"
#include "riscv.h"
#include "definitions.h"
#include "io.h"


void plic_init(){
    *(uint32*)(PLIC_BASE + UART0_IRQ*4) = 1;
    *(uint32*)(PLIC_BASE + VIRTIO0_IRQ*4) = 1;
}

// 初始化硬件线程
void plic_inithart(){

    int hartid = r_tp();

    // set enable bits for this hart's S-mode
    // for the uart and virtio disk.
    *(uint32*)PLIC_SENABLE(hartid) = (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

    // set this hart's S-mode priority threshold to 0.
    *(uint32*)PLIC_SPRIORITY(hartid) = 0;
}

// 声明plic中断类型
int plic_claim(void)
{
  int hartid = r_tp();
  int irq = PLIC_SCLAIM(hartid);
  return irq;
}

// 中断完成
void plic_complete(int irq)
{
  int hartid = r_tp();
  *(uint32*)PLIC_SCLAIM(hartid) = irq;
}
