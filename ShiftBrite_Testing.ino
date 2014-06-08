#include "SPI.h"
#include "Adafruit_WS2801.h"

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
uint8_t dataPin = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

// Don't forget to connect the ground wire to Arduino ground,
// and the +5V wire to a +5V supply

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin);

// Optional: leave off pin numbers to use hardware SPI
// (pinout is then specific to each board and can't be changed)
//Adafruit_WS2801 strip = Adafruit_WS2801(25);

// For 36mm LED pixels: these pixels internally represent color in a
// different format.  Either of the above constructors can accept an
// optional extra parameter: WS2801_RGB is 'conventional' RGB order
// WS2801_GRB is the GRB order required by the 36mm pixels.  Other
// than this parameter, your code does not need to do anything different;
// the library will handle the format change.  Examples:
//Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin, WS2801_GRB);
//Adafruit_WS2801 strip = Adafruit_WS2801(25, WS2801_GRB);

void setup() {

	strip.begin();

	// Update LED contents, to start they are all 'off'
	strip.show();
}


void loop() {
	// Some example procedures showing how to display to the pixels

	colorWipe(Color(255, 0, 0), 50);
	colorWipe(Color(0, 255, 0), 50);
	colorWipe(Color(0, 0, 255), 50);
	rainbow(20);
	rainbowCycle(20);
}

void rainbow(uint8_t wait) {
	int i, j;

	for (j = 0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
		for (i = 0; i < strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel((i + j) % 255));
		}
		strip.show();   // write all the pixels out
		delay(wait);
	}
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
	int i, j;

	for (j = 0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
		for (i = 0; i < strip.numPixels(); i++) {
			// tricky math! we use each pixel as a fraction of the full 96-color wheel
			// (thats the i / strip.numPixels() part)
			// Then add in j which makes the colors go around per pixel
			// the % 96 is to make the wheel cycle around
			strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) % 256));
		}
		strip.show();   // write all the pixels out
		delay(wait);
	}
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
	int i;

	for (i = 0; i < strip.numPixels(); i++) {
		strip.setPixelColor(i, c);
		strip.show();
		delay(wait);
	}
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
	uint32_t c;
	c = r;
	c <<= 8;
	c |= g;
	c <<= 8;
	c |= b;
	return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
	if (WheelPos < 85) {
		return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
	}
	else if (WheelPos < 170) {
		WheelPos -= 85;
		return Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	else {
		WheelPos -= 170;
		return Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
}


/*

int datapin = 10; // DI
int latchpin = 11; // LI
int enablepin = 12; // EI
int clockpin = 13; // CI
unsigned long SB_CommandPacket;
int SB_CommandMode;
int SB_BlueCommand;
int SB_RedCommand;
int SB_GreenCommand;

const int NumLEDs = 4;
int ct = 0;

int CurrentScene[NumLEDs][3] = { 0 };
int NextScene[NumLEDs][3] = { 0 };

int DimSettings[4] = { 1023, 0, 0, 128 };
int BrightSettings[4] = { 1023, 0, 0, 512 };
int Multiplier = 1;
int CurrentLED = 1;


void setup() {
	pinMode(datapin, OUTPUT);
	pinMode(latchpin, OUTPUT);
	pinMode(enablepin, OUTPUT);
	pinMode(clockpin, OUTPUT);

	digitalWrite(latchpin, LOW);
	digitalWrite(enablepin, LOW);

	InitializeLEDs();

	// Start Out
	//ChaserSetup();
	CylonSetup();
}

void loop() {
	ct++;
	if (ct == NumLEDs) {
		ct = 0;
		InitializeLEDs(); // this needs to be called periodically to ensure the LEDs have the correct settings
	}

	//ChaserLoop();
	CylonLoop();
}

void ChaserSetup() {
	/// We're going to need a setup for each function then a loop for each function
	/// This is the Chaser setup function
	for (int i = 0; i < NumLEDs; i++) {
		SetCurrentSceneLEDSettings(i, DimSettings);
		SetNextSceneLEDSettings(i, DimSettings); // don't forget to set the default for the next scene
	}
	WriteLEDArray();
}

void ChaserLoop() {
	/// This is the Chaser loop function
	for (int i = 0; i < NumLEDs; i++) {
		SetNextSceneLEDSettings(i, BrightSettings);
		FadeToNextScene(20, 30);
		for (int j = 0; j < NumLEDs; j++) {
			SetNextSceneLEDSettings(j, DimSettings);
		}
	}
}

void CylonSetup() {
	Multiplier = 1;
	CurrentLED = 1;

	ChaserSetup();
}

void CylonLoop() {
	SetNextSceneLEDSettings(CurrentLED, BrightSettings);
	FadeToNextScene(20, 30);
	for (int j = 0; j < NumLEDs; j++) {
		SetNextSceneLEDSettings(j, DimSettings);
	}
	if (CurrentLED == 0 || CurrentLED == NumLEDs - 1)
		Multiplier *= -1;
	CurrentLED += Multiplier;
}

void FadeToNextScene(int Steps, int Wait) {
	float Deltas[NumLEDs][3] = { 0.0 };

	int StartScene[NumLEDs][3];
	for (int i = 0; i < NumLEDs; i++){
		for (int j = 0; j < 3; j++)
		{
			StartScene[i][j] = CurrentScene[i][j];
		}
	}

	// Get deltas and speed
	for (int i = 0; i < NumLEDs; i++) {
		Deltas[i][0] = ((float)NextScene[i][0] - (float)CurrentScene[i][0]) / (float)Steps;
		Deltas[i][1] = ((float)NextScene[i][1] - (float)CurrentScene[i][1]) / (float)Steps;
		Deltas[i][2] = ((float)NextScene[i][2] - (float)CurrentScene[i][2]) / (float)Steps;
	}

	for (int i = 0; i <= Steps; i++) {
		for (int j = 0; j < NumLEDs; j++)
		{
			SetCurrentSceneLEDColor(j, StartScene[j][0] + (Deltas[j][0] * i), StartScene[j][1] + (Deltas[j][1] * i), StartScene[j][2] + (Deltas[j][2] * i));
		}
		WriteLEDArray();
		delay(Wait);
	}
}

void SetCurrentSceneLEDSettings(int ID, int Settings[]) {
	int Red, Green, Blue;
	float Dimmer;
	Dimmer = (float)Settings[3] / 1023.0;
	Red = Settings[0] * Dimmer;
	Green = Settings[1] * Dimmer;
	Blue = Settings[2] * Dimmer;

	SetCurrentSceneLEDColor(ID, Red, Green, Blue);
}

void SetCurrentSceneLEDColor(int ID, int Red, int Green, int Blue) {
	CurrentScene[ID][0] = Red;
	CurrentScene[ID][1] = Green;
	CurrentScene[ID][2] = Blue;
}

void SetNextSceneLEDSettings(int ID, int Settings[]) {
	int Red, Green, Blue;
	float Dimmer;
	Dimmer = (float)Settings[3] / 1023.0;
	Red = Settings[0] * Dimmer;
	Green = Settings[1] * Dimmer;
	Blue = Settings[2] * Dimmer;

	SetNextSceneLEDColor(ID, Red, Green, Blue);
}

void SetNextSceneLEDColor(int ID, int Red, int Green, int Blue) {
	NextScene[ID][0] = Red;
	NextScene[ID][1] = Green;
	NextScene[ID][2] = Blue;
}










// -- DON'T MESS WITH THESE FOR A LONG WHILE
void InitializeLEDs() {
	int Current = 127; //Full = 127, Half = 64;
	SB_CommandMode = B01; // Write to current control registers
	SB_RedCommand = Current;
	SB_GreenCommand = Current;
	SB_BlueCommand = Current;
	SB_SendPacket();
}

void SB_SendPacket() {
	SB_CommandPacket = SB_CommandMode & B11;
	SB_CommandPacket = (SB_CommandPacket << 10) | (SB_BlueCommand & 1023);
	SB_CommandPacket = (SB_CommandPacket << 10) | (SB_RedCommand & 1023);
	SB_CommandPacket = (SB_CommandPacket << 10) | (SB_GreenCommand & 1023);

	shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 24);
	shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 16);
	shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 8);
	shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket);
}

void WriteLEDArray() {
	SB_CommandMode = B00; // Write to PWM control registers

	for (int i = 0; i < NumLEDs; i++) {
		SB_RedCommand = CurrentScene[i][0];
		SB_GreenCommand = CurrentScene[i][1];
		SB_BlueCommand = CurrentScene[i][2];
		SB_SendPacket();
	}

	delay(1); // adjustment may be necessary depending on chain length
	digitalWrite(latchpin, HIGH); // latch data into registers
	delay(1); // adjustment may be necessary depending on chain length
	digitalWrite(latchpin, LOW);
}*/