#include "receiver.h"

Receiver::Receiver() : decoder(bytes, sizeof(bytes))
{
	reset();
}

void Receiver::pulse(int pulse)
{
	switch (state)
	{
	case ST_HW_SYNC1:
	case ST_HW_SYNC2:
	case ST_HW_SYNC3:
	case ST_HW_SYNC4:
		hwSyncPulse(pulse);
		break;
	case ST_SW_SYNC1:
		swSyncPulse1(pulse);
		break;
	case ST_SW_SYNC2:
		swSyncPulse2(pulse);
		break;
	case ST_PAYLOAD:
		payloadPulse(pulse);
		break;
	}
}

bool Receiver::hasFrame()
{
	return state == ST_DONE;
}

bool Receiver::storeFrame(Frame *output)
{
	if (!hasValidChecksum()) return false;
	output->controlCode = bytes[1] >> 4;
	output->rollingCode = (bytes[2] << 8) | bytes[3];
	output->address = (bytes[4] << 16) | (bytes[5] << 8) | bytes[6];
	return true;
}

void Receiver::reset()
{
	state = ST_HW_SYNC1;
}

void Receiver::hwSyncPulse(int pulse)
{
	if (pulse > 1792 && pulse < 3328)
		state++;
	else
		reset();
}

void Receiver::swSyncPulse1(int pulse)
{
	if (pulse > 3136 && pulse < 5824)
		state++;
	else
		reset();
}

void Receiver::swSyncPulse2(int pulse)
{
	if (isLongPulse(pulse))
	{
		decoder.init(1, true);
		state++;
	}
	else if (isShortPulse(pulse))
	{
		decoder.init(0, false);
		state++;
	}
	else
	{
		reset();
	}
}

void Receiver::payloadPulse(int pulse)
{
	if (isLongPulse(pulse))
	{
		if (!decoder.addLongPulse()) reset();
	}
	else if (isShortPulse(pulse))
	{
		decoder.addShortPulse();
	}
	else
	{
		reset();
	}

	/* When we only have one bit left we already now what the value is going to be. We skip
	 * validation because the last pulse might be longer. */
	if (decoder.isLastBit()) decoder.storeBit();

	if (decoder.completed())
	{
		deobfuscate();
		state++;
	}
}

bool Receiver::isLongPulse(int pulse)
{
	return (pulse > 896 && pulse < 1664);
}

bool Receiver::isShortPulse(int pulse)
{
	return (pulse > 448 && pulse < 832);
}

void Receiver::deobfuscate()
{
	for (size_t i = FRAME_SIZE-1; i > 0; i--) bytes[i] = bytes[i] ^ bytes[i-1];
}

bool Receiver::hasValidChecksum()
{
	uint8_t checksum = 0;
	for (size_t i = 0; i < FRAME_SIZE; i++) checksum = checksum ^ bytes[i] ^ (bytes[i] >> 4);
	checksum = checksum & 0xf;
	return checksum == 0;
}
