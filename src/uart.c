#include "definitions.h"
#include "types.h"
#include "memorylayout.h"
#include "printf.h"
#include "io.h"
#include "riscv.h"

#define UART        0x10000000

/* THR:transmitter holding register */
#define UART_DAT    (UART+0x00) /* 数据寄存器*/
#define UART_IER    (UART+0x01) /* 中断使能寄存器*/
#define UART_IIR    (UART+0x02) /* 中断标识寄存器 (read only)*/
#define UART_FCR    (UART+0x02) /* FIFO控制寄存器 (write only)*/
#define UART_LCR    (UART+0x03) /* 线路控制寄存器*/
#define UART_MCR    (UART+0x04) /* MODEN控制寄存器*/
#define UART_LSR    (UART+0x05) /* 线路状态寄存器*/
#define UART_MSR    (UART+0x06) /* MODEN状态寄存器*/

#define UART_DLL (UART+0x00)  /*预分频寄存器低8位*/
#define UART_DLM (UART+0x01)  /*预分频寄存器高8位*/

#define UART_LSR_ERROR   0x80  /* 出错 */
#define UART_LSR_EMPTY    0x40  /* 传输FIFO和移位寄存器为空 */
#define UART_LSR_TFE	    0x20  /* 传输FIFO为空 */
#define UART_LSR_BI	    0x10  /* 传输被打断 */
#define UART_LSR_FE	    0x08  /* 接收到没有停止位的帧 */
#define UART_LSR_PE	    0x04  /* 奇偶校验错误位 */
#define UART_LSR_OE	    0x02  /* 数据溢出 */
#define UART_LSR_DR	    0x01  /* FIFO有数据 */

char uart_getchar(void)
{
	if (io_read_8(UART_LSR) & UART_LSR_DR){
		return io_read_8(UART_DAT);
	}
	else{
		return -1;
	}
}

void uart_putchar(char c)
{
	if(c == '\n') c = '\r';
    while((io_read_8(UART_LSR) & UART_LSR_EMPTY) == 0);

	io_write_8(c, UART_DAT);
}

static unsigned int uart16550_clock = 1843200;   // a common base clock
#define UART_DEFAULT_BAUD  115200

void uart_init(void)
{
	unsigned int divisor = uart16550_clock / (16 * UART_DEFAULT_BAUD);

	// 关中断
	io_write_8(0, UART_IER);

	/* Enable DLAB (set baud rate divisor)*/
	io_write_8(0x80, UART_LCR);
	io_write_8((unsigned char)divisor, UART_DLL);
	io_write_8((unsigned char)(divisor >> 8), UART_DLM);

	io_write_8(0x3, UART_LCR);

	/* 使能FIFO，清空FIFO，设置14字节threshold*/
	io_write_8(0xc7, UART_FCR);

	/* 使能接收缓冲区满中断*/
	io_write_8(0x1, UART_IER);
}

void uart_handler(void)
{
    char c;

    c = uart_getchar();

    uart_putchar(c);
	uart_putchar('\n');
    if (c == 255) // -1
        return;
}


