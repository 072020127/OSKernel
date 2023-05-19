#ifndef PROC_H
#define PROC_H

#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "ptregs.h"


typedef struct {
  uint64 ra;
  uint64 sp;

  //调用时保存
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
}context;


//进程状态
//RUNNING 进程准备好运行，但尚未分配CPU
//READY 进程准备好运行，但尚未分配CPU
//ZOMBIE 进程已经运行完毕，但尚未被父进程回收。在此状态下的进程已经不再占用CPU资源
//FREE 该进程为空闲状态，可被分配
//EMBRYO 处于创建装态，未初始化
//BLOCKED 由于某种请求进入的状态，在等待进程发生 
enum procState{RUNNING, BLOCKED, SLEEPING, READY, ZOMBIE, FREE, EMBRYO};


//进程控制块
typedef struct PCB{


    // 在使用下列信息时要获取p->lock
    int pid;                            //进程描述信息
    enum procState proc_state;          //进程状态
    int priority;                       //进程优先级
    int killed;                         //不为0，进程被kill
    void* blocked_queue;                //阻塞队列，不为0表示在队列上


    struct PCB* parent;                 //父进程
    uint64 proc_memory_size;            //进程内存空间大小
    uint64 kernel_stack;                //内核栈


    char name[32];                      //进程名
    context proc_context;               //进程调度的上下文切换
    struct pt_regs* pt_regs;                //中断发生时的上下文切换保存
    struct spinlock lock;               //自旋锁

    //  MLFQ
    uint64 entry_time;                  //进程进入时间
    uint64 chosen_queue;                //在四个队列中被选中的优先级队列
}PCB;

typedef struct CPU{
    PCB *cur_proc;           //当前运行在cpu上的进程
    context context;         //上下文切换
    int depth;               //push_off的深度
    int inter_enable;        //是否在push_off前可中断 0表示不可中断
}CPU;

extern CPU cpus[NCPU];

#endif