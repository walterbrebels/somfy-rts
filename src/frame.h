#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>

#define FRAME_SIZE 7

enum ControlCode
{
	CC_MY           = 0x1,
	CC_UP           = 0x2,
	CC_MY_AND_UP    = 0x3,
	CC_DOWN         = 0x4,
	CC_MY_AND_DOWN  = 0x5,
	CC_UP_AND_DOWN  = 0x6,
	CC_PROG         = 0x8,
	CC_SUN_AND_FLAG = 0x9,
	CC_FLAG         = 0xA
};

struct Frame
{
	uint8_t controlCode;
	uint16_t rollingCode;
	uint32_t address;
};

#endif
