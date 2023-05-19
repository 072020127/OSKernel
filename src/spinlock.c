#include "spinlock.h"
#include "definitions.h"
#include "spinlock.h"
#include "riscv.h"
#include "proc.h"

void acquire_lock(spinlock* lock){
    push_off();

    if(holding(lock)){
        panic("acquire");
    }

    //使用swap原子地交换地址和1
    //直到接收了0
    while(__sync_lock_test_and_set(&(lock->locked),1)!=0);

    __sync_synchronize();       //禁用编译器优化

    lock->cpu=mycpu();          
}

void release_lock(spinlock* lock){

    if(!holding(lock)){
        panic("realse");
    }

    lock->cpu=0;

    __sync_synchronize();

    __sync_lock_release(&lock->locked);

    pop_off();
}

void push_off(){
    int old = intr_get();

    //关中断
    intr_off();

    CPU* my_cpu = mycpu();
    
    //首次push_off
    if(my_cpu->depth == 0){
        my_cpu->inter_enable = old;
    }

    my_cpu->depth++;
}

void pop_off(){
    CPU* my_cpu = mycpu();

    if(intr_get())
    panic("pop_off - interruptible");

    if(my_cpu->depth < 1){
        panic("pop_off");
    }

    my_cpu->depth--;

    //若退出最后一个push_off, 并且cpu中断允许则开中断
    if(my_cpu->depth = 0 && my_cpu->inter_enable){
        intr_on();
    }
}

int holding(spinlock* lock){
    return lock->locked && lock->cpu == mycpu();
}


void init_lock(spinlock* lock, char* name){
    lock->spinlock_name=name;
    lock->cpu=0;
    lock->locked=0;
}