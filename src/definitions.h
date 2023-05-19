//proc.c
void yield(); 
void scheduler();
void sched();
void sleep(void* block_ornot, struct spinlock lock);
void wakeup(void* block);
struct CPU* mycpu();
struct PCB* myproc();
void proc_init();


//spinlock.c
void proc_init();
void init_lock(struct spinlock* lock, char* name);
void acquire_lock(struct spinlock* lock);
void release_lock(struct spinlock* lock);
void push_off();
void pop_off();
int holding(struct spinlock* lock);
void scheduler();  
struct PCB* allocproc();


//switch_to.S
void switch_to(struct context* old, struct context* new);


//type.c
extern unsigned int ticks;
void timer_irq_handle();
void soft_irq_handle();
void ex_irq_handle();
void trap_init(void);

//plic.c
void plic_init();
void plic_inithart();
int plic_claim();
void plic_complete();


//uart.c
void uart_init();
void uart_handler();
char uart_getchar();
void uart_putchar(char c);
void uart_start();





