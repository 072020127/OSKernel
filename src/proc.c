#include "proc.h"
#include "spinlock.h"
#include "riscv.h"
#include "definitions.h"
#include "memorylayout.h"

//进程号锁
spinlock pid_lock;

int nextPid = 1;

// 确保等待父进程的wait()函数能够正确被唤醒
// 避免访问proc->parent时丢失信号
spinlock wait_lock;

PCB procs[NPROC];

CPU cpus[NCPU];

PCB* initProc;



void proc_init(){
    init_lock(&pid_lock, "pid lock");
    init_lock(&wait_lock, "wait lock");

    PCB* p;

    for(p = procs; p < &procs[NPROC]; p++){
        init_lock(&p->lock, "proc");
        p->proc_state = FREE;
        //分配内核栈kstack
        p->kernel_stack = KSTACK((int) (p - procs));
    }
}

PCB* allocproc(){
    PCB* p;
    for(p = procs; p < &procs[NPROC]; p++){
        acquire_lock(&p->lock);
        if(p->proc_state == FREE){
            p->pid = nextPid++;
            p->proc_state = EMBRYO;

            //分配页表
        }
        else{
            release_lock(&p->lock);
        }
    }
}

int getPid(){
    acquire_lock(&pid_lock);
    int pid;
    pid = nextPid;
    nextPid++;
    release_lock(&pid_lock);
    return pid;
}

// 返回当前CPU
CPU* mycpu(){
    int cpu_id = r_tp();
    CPU *cpu = &cpus[cpu_id];
    return cpu;
}

// 返回当前运行在当前cpu上的进程，若无返回NULL
PCB* myproc(){
    push_off();
    CPU* c = mycpu();
    PCB* p = c->cur_proc;
    pop_off();
    return p;
}

// 让出当前CPU
void yield()
{
  PCB *p = myproc();
  acquire_lock(&p->lock);
  p->proc_state = READY;
  sched();
  release_lock(&p->lock);
}

// 切换当前进程到进程调度器
// 保存当前进程的intena
void sched(){
    PCB* p = myproc();

    // 当前进程的intena在切换后可能会被改变，因此保护起来
    // 在切换运行完成后，重新赋值
    int this_intena = mycpu()->inter_enable;
    switch_to(&p->proc_context, &mycpu()->context);
    mycpu()->inter_enable = this_intena;
}

// 将当前进程的状态设置为BOLCKED，修该其blocked_queue的值
// 表示在对应的阻塞队列上“睡着”了
// 阻塞后调用sched切换进程
void sleep(void* block_ornot, struct spinlock lock){
    PCB* p = myproc();

    // release传入的lock是因为已经持有了当前进程的lock
    // 那么当前进程的block_queue就不会由于其他的wakeup或sleep改变
    acquire_lock(&p->lock);
    release_lock(&lock);

    p->blocked_queue = block_ornot;
    p->proc_state = BLOCKED;

    sched();
    
    p->blocked_queue = 0;

    release_lock(&p->lock);
    acquire_lock(&lock);
}

void wakeup(void* block){
    PCB* p;
    for(p = procs; p < &procs[NPROC]; p++){
        if( p != myproc()){
            acquire_lock(&p->lock);
            if(p->proc_state == BLOCKED){
                if(p->blocked_queue == block){
                    p->proc_state = READY;
                }
            }
            release_lock(&p->lock);
        }
    }
}


//进程调度
void scheduler(){
    PCB *p;
    CPU *cpu = mycpu();

    cpu->cur_proc=0;

    while(1){

        //printf("ticks = %d\n", ticks);

        // 中断允许
        intr_on();

        PCB* cur_proc = 0;
        int priority_queue = 4;

        for(p = procs; p < &procs[NPROC]; ++p){
            if(p->proc_state != READY){
                continue;
            }

            if(p->chosen_queue > 0 && ( ticks - p->entry_time ) > WAITING_LIMIT){
                acquire_lock(&p->lock);

                p->chosen_queue--;
                p->entry_time = ticks;

                release_lock(&p->lock);
            }
        }


        // MLFQ进程调度
        for(p = procs; p < &procs[NPROC]; p++){
            // 选择进程
            if(p->proc_state != READY){
                continue;
            }

            if(cur_proc == 0){//首次进入的进程，选择对应优先级队列执行
                cur_proc = p;
                priority_queue = p->chosen_queue;
            }
            else if(p->chosen_queue == priority_queue){//优先级为当前队列
                if(p->entry_time < cur_proc->entry_time){//到达时间较早，优先执行进入进程
                    cur_proc = p;
                }
            }
            else if(p->chosen_queue < priority_queue){//进程的优先级较高，切换到进程对应的优先级队列
                cur_proc = p;
                priority_queue = p->chosen_queue;
            }


            // 调度进程
            if(cur_proc == 0){
                continue;
            }

            acquire_lock(&cur_proc->lock);
            if(cur_proc->proc_state == READY){

                cur_proc->entry_time = ticks;

                // 进程设置为RUNNING
                cur_proc->proc_state == RUNNING;
                cpu->cur_proc = cur_proc;
                switch_to(&cpu->context, &cur_proc->proc_context);
                
                cpu->cur_proc = 0;
            }
            release_lock(&cur_proc->lock);
        }   
    }
    
}