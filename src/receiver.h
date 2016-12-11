#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>
#include "manchester_decoder.h"
#include "frame.h"

class Receiver
{
	enum State
	{
		ST_HW_SYNC1,
		ST_HW_SYNC2,
		ST_HW_SYNC3,
		ST_HW_SYNC4,
		ST_SW_SYNC1,
		ST_SW_SYNC2,
		ST_PAYLOAD,
		ST_DONE
	};
public:
	Receiver();
	void pulse(int pulse);
	bool hasFrame();
	bool storeFrame(Frame *output);
	void reset();

private:
	void hwSyncPulse(int pulse);
	void swSyncPulse1(int pulse);
	void swSyncPulse2(int pulse);
	void payloadPulse(int pulse);

	bool isLongPulse(int pulse);
	bool isShortPulse(int pulse);

	void deobfuscate();
	bool hasValidChecksum();

	uint8_t state;
	uint8_t bytes[FRAME_SIZE];
	ManchesterDecoder decoder;
};

#endif
