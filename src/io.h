#ifndef IO_H
#define IO_H

#define __arch_getl(a)			(*(volatile unsigned int *)(a))
#define __arch_putl(v,a)		(*(volatile unsigned int *)(a) = (v))

#define __arch_getb(a)			(*(volatile unsigned char *)(a))
#define __arch_putb(v,a)		(*(volatile unsigned char *)(a) = (v))

#define __arch_getq(a)			(*(volatile unsigned long *)(a))
#define __arch_putq(v,a)		(*(volatile unsigned long *)(a) = (v))

#define dmb()		__asm__ __volatile__ ("" : : : "memory")
#define __iormb()	dmb()
#define __iowmb()	dmb()

#define io_read_32(c)	({ unsigned int  __v = __arch_getl((unsigned long)c); __iormb(); __v; })
#define io_write_32(v,c)	({ unsigned int  __v = v; __iowmb(); __arch_putl(__v, (unsigned long)c);})

#define io_read_8(c)	({ unsigned char  __v = __arch_getb(c); __iormb(); __v; })
#define io_write_8(v,c)	({ unsigned char  __v = v; __iowmb(); __arch_putb(__v,c);})

#define io_read_64(c)	({ unsigned long  __v = __arch_getq(c); __iormb(); __v; })
#define io_write_64(v,c)	({ unsigned long  __v = v; __iowmb(); __arch_putq(__v,c);})

#endif