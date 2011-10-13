#ifndef __memory_h
#define __memory_h

#define NEW(type) (type *)Malloc(sizeof(type))

void *Malloc(unsigned n);

#endif
