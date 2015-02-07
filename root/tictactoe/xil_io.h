#ifndef XIL_IO_H_INCLUDED
#define XIL_IO_H_INCLUDED

#include <stdint.h>



#define PAGE_SIZE  ((size_t)getpagesize())
#define PAGE_MASK ((uint64_t) (long)~(PAGE_SIZE - 1))

void Xil_Out32(uint64_t phyaddr, uint32_t val);
int Xil_In32(uint64_t phyaddr);

#endif // XIL_IO_H_INCLUDED
