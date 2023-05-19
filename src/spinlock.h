#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "types.h"

typedef struct spinlock{

    uint32 locked;                //是否被锁住
    
    /* data */
    char* spinlock_name;      //锁的名字
    struct CPU* cpu;                    //锁所占用的CPU
}spinlock;


#endif