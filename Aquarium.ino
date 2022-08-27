/*
 Name:		Aquarium.ino
 Created:	8/26/2022 3:56:31 PM
 Author:	Gerald
*/

#include <HardwareSerial.h>
#include <FastLED.h>

//#define LED D0            // Led in NodeMCU at pin GPIO16 (D0).
#define LED_INTERNAL 2     // Led in NodeMCU at pin GPIO14 (D2).
#define LED_STRIP 16		// STRIP D0 = GPIO16
#define BUTTON D5			// Button to GND

#define LED_COUNT 50
CRGB leds[LED_COUNT];

CRGB WarmWhite = CRGB(0xFF, 0x59, 0x1F);

ulong now;
int stripMode = 0;
#define STRIP_MODE_COUNT 4
ulong nextStrip = 0;
ulong startMode = 0;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(56000);
	pinMode(LED_INTERNAL, OUTPUT);    // LED pin as output.
	pinMode(BUTTON, INPUT_PULLUP);    // button push = 0
	FastLED.addLeds<NEOPIXEL, LED_STRIP>(leds, LED_COUNT);
	Serial.println("setup completed");

}

// the loop function runs over and over again until power down or reset
void loop() {
	now = millis();
	handleButton();
	handleInternalLED();
	handleStrip();
}



ulong nextInternal = 0;
bool internalOn = false;
void handleInternalLED()
{
	if (nextInternal > now) return;
	internalOn = !internalOn;
	nextInternal = now + 500;
	digitalWrite(LED_INTERNAL, internalOn ? HIGH : LOW); // turn the LED on/off.
	Serial.print(internalOn ? 'L' : 'l');
}

void setStripColor(CRGB color)
{
	for (int i = 0; i < LED_COUNT; i++) {
		leds[i] = color;
	}
	FastLED.show();

}


void handleStrip() 
{
	if (nextStrip > now) return;
	switch (stripMode)
	{
	case 1:
		handleStripDim(true);
		break;
	case 2:
		handleStripDim(false);
		break;
	case 3:
		handleStripColorTest();
		break;
	default:
		nextStrip = now + 10000;
		setStripColor(CRGB::Black);
		break;
	}
}


void handleStripDim(bool on) 
{
	int bright = (now - startMode) / 5;
	if (bright > 0xff)
	{
		nextStrip = now + 10000;
		setStripColor(on ? WarmWhite : CRGB::Black);
	}
	else
	{
		if (!on) bright = 0xff - bright;
		setStripColor(WarmWhite.scale8(bright));
	}
}


void handleStripColorTest ()
{
	int pos = ((now - startMode) / 10) % (511 * 3);
	int co = pos / 511;
	pos = 0xff - abs ((pos % 0x1FF) - 0xff);
	uint8_t r = co == 0 ? pos : 0;
	uint8_t g = co == 1 ? pos : 0;
	uint8_t b = co == 2 ? pos : 0;

	setStripColor(CRGB(r, g, b));

}

bool wasButton = false;
void handleButton() {
	bool isButton = !digitalRead(BUTTON);
	if (isButton == wasButton) return;
	wasButton = isButton;
	Serial.print(isButton ? 'B' : 'b');
	if (!isButton) return;
	stripMode = ++stripMode % STRIP_MODE_COUNT;
	nextStrip = 0;
	startMode = now;
	Serial.print(stripMode);
}

