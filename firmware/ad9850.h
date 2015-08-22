#ifndef __AD9850_H__
#define __AD9850_H__

#include "inttypes.h"

extern void ad9850_init();
extern void ad9850_set(uint32_t fword);
extern void ad9850_shutdown();

#endif // __AD9850_H__
