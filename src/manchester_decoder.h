#ifndef MANCHESTER_DECODER_H
#define MANCHESTER_DECODER_H

#include <stdlib.h>
#include <stdint.h>

class ManchesterDecoder
{
public:
	ManchesterDecoder(uint8_t *bytes, size_t bytesSize) : bytes(bytes), bytesSize(bytesSize)
	{
		init(1, false);
	}

	void init(bool nextBit, bool secondPulse)
	{
		this->nextBit = nextBit;
		this->secondPulse = secondPulse;
		byteIdx = 0;
		bitIdx = 0;
		bytes[0] = 0;
	}

	void addShortPulse()
	{
		if (secondPulse)
		{
			storeBit();
			secondPulse = false;
		}
		else
		{
			secondPulse = true;
		}
	}

	bool addLongPulse()
	{
		if (!secondPulse) return false;
		storeBit();
		nextBit = !nextBit;
		return true;
	}

	bool isLastBit()
	{
		return (byteIdx == bytesSize - 1) && (bitIdx == 7);
	}

	bool completed()
	{
		return (byteIdx == bytesSize) && (bitIdx == 0);
	}

	void storeBit()
	{
		bytes[byteIdx] <<= 1;
		bytes[byteIdx] |= nextBit;
		if (++bitIdx == 8)
		{
			byteIdx++;
			bitIdx = 0;
			bytes[byteIdx] = 0;
		}
	}

private:
	bool secondPulse;
	bool nextBit;
	uint8_t *bytes;
	size_t bytesSize;
	size_t byteIdx;
	size_t bitIdx;
};

#endif
