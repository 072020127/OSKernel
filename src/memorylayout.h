#ifndef MEMORYLAYOUT
#define MEMORYLAYOUT

//内核栈定义在.bss段
#define KERNELBASE 0X0000000080200000

#define PAGESIZE 4096

// 采用Sv39，最大的虚拟地址空间为2的39次方
// 最大虚拟地址为 2^39-1
#define MAXVA (1L << 38)

#define TRAMPOLINE (MAXVA - PAGESIZE)

#define KSTACK(p) (TRAMPOLINE - ((p)+1)*2*PAGESIZE)

// qemu puts UART registers here in physical memory.
#define UART0 0x10000000L
#define UART0_IRQ 10

// virtio mmio interface
#define VIRTIO0 0x10001000
#define VIRTIO0_IRQ 1

// CLINT
#define CLINT 0x2000000L
#define CLINT_MTIMECMP(hartid) (CLINT + 0x4000 + 8*(hartid))
#define CLINT_MTIME (CLINT + 0xBFF8)

// 访问 PLIC 设备的寄存器地址
#define PLIC_BASE 0x0c000000L                //PLIC 设备的基地址，指向 PLIC 的控制寄存器
#define PLIC_PRIORITY (PLIC_BASE + 0x0)      //PLIC 设备中的中断优先级寄存器的地址
#define PLIC_PENDING (PLIC_BASE + 0x1000)    //PLIC 设备中的中断挂起寄存器的地址
#define PLIC_MENABLE(hart) (PLIC_BASE + 0x2000 + (hart)*0x100)   //用于管理主核中断使能的寄存器
#define PLIC_SENABLE(hart) (PLIC_BASE + 0x2080 + (hart)*0x100)   //用于管理辅助核中断使能的寄存器
#define PLIC_MPRIORITY(hart) (PLIC_BASE + 0x200000 + (hart)*0x2000)      //用于管理主核中断优先级的寄存器
#define PLIC_SPRIORITY(hart) (PLIC_BASE + 0x201000 + (hart)*0x2000)      //用于管理辅助核中断优先级的寄存器
#define PLIC_MCLAIM(hart) (PLIC_BASE + 0x200004 + (hart)*0x2000)     //用于管理辅助核的中断声明寄存器，获取中断编号
#define PLIC_SCLAIM(hart) (PLIC_BASE + 0x201004 + (hart)*0x2000)     //用于管理辅助核的中断声明寄存器，获取中断编号


#endif