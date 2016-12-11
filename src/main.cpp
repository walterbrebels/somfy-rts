#include <Arduino.h>
#include <receiver.h>

#define RF_PIN D1

#define MAX_PULSES 16
volatile int pulses[MAX_PULSES];
volatile size_t pulseIndex;
volatile int lastTime;

Frame frame;
Receiver receiver;

static void getFrame()
{
	if (!receiver.storeFrame(&frame))
	{
		Serial.println("Frame checksum NOK");
		return;
	}

	Serial.print("Frame address=");
	Serial.print(frame.address, HEX);
	Serial.print(", control code=");
	Serial.print(frame.controlCode, HEX);
	Serial.print(", rolling code=");
	Serial.println(frame.rollingCode, HEX);
}

ICACHE_RAM_ATTR void pinChanged()
{
	int time = micros();
	int duration = time - lastTime;
	lastTime = time;
	if (pulseIndex < MAX_PULSES) pulses[pulseIndex++] = duration;
}

void setup(void)
{
	Serial.begin(115200);
	Serial.println("\nStarting...");
	pulseIndex = 0;
	pinMode(RF_PIN, INPUT);
	attachInterrupt(digitalPinToInterrupt(RF_PIN), pinChanged, CHANGE);
}

void loop(void)
{
	static int pulses2[MAX_PULSES];
	static int pulseIndex2;

	noInterrupts();
	pulseIndex2 = pulseIndex;
	for (size_t i = 0; i < pulseIndex2; i++) pulses2[i] = pulses[i];
	pulseIndex = 0;
	interrupts();

	if (pulseIndex2 == MAX_PULSES)
	{
		Serial.println("ERROR: Overflow detected!");
	}

	for (size_t i = 0; i < pulseIndex2; i++)
	{
		receiver.pulse(pulses2[i]);

		if (receiver.hasFrame())
		{
			getFrame();
			receiver.reset();
		}
	}
}
