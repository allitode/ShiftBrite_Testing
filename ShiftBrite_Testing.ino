int datapin = 10; // DI
int latchpin = 11; // LI
int enablepin = 12; // EI
int clockpin = 13; // CI
unsigned long SB_CommandPacket;
int SB_CommandMode;
int SB_BlueCommand;
int SB_RedCommand;
int SB_GreenCommand;

const int NumLEDs = 3;
int ct = 0;
int CurrentLED = 0;
int CurrentLEDSettings[4]; // Red, Green, Blue, Dimmer

int LEDChannels[NumLEDs][3] = { 0 };

void setup() {
	pinMode(datapin, OUTPUT);
	pinMode(latchpin, OUTPUT);
	pinMode(enablepin, OUTPUT);
	pinMode(clockpin, OUTPUT);

	digitalWrite(latchpin, LOW);
	digitalWrite(enablepin, LOW);

	InitializeLEDs();

	/*CurrentLEDSettings[0] = 255;
	CurrentLEDSettings[1] = 0;
	CurrentLEDSettings[2] = 0;
	CurrentLEDSettings[3] = 64;
	SetLEDColor(0, CurrentLEDSettings);

	CurrentLEDSettings[0] = 0;
	CurrentLEDSettings[1] = 255;
	CurrentLEDSettings[2] = 0;
	CurrentLEDSettings[3] = 64;
	SetLEDColor(1, CurrentLEDSettings);

	CurrentLEDSettings[0] = 0;
	CurrentLEDSettings[1] = 0;
	CurrentLEDSettings[2] = 255;
	CurrentLEDSettings[3] = 64;
	SetLEDColor(2, CurrentLEDSettings);*/

	

	//WriteLEDArray();
}

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
		SB_RedCommand = LEDChannels[i][0];
		SB_GreenCommand = LEDChannels[i][1];
		SB_BlueCommand = LEDChannels[i][2];
		SB_SendPacket();
	}

	delay(1); // adjustment may be necessary depending on chain length
	digitalWrite(latchpin, HIGH); // latch data into registers
	delay(1); // adjustment may be necessary depending on chain length
	digitalWrite(latchpin, LOW);
}

void loop() {
	if (ct++ == 255) {
		ct = 0;
	}
	int Dimmer = ((float)ct / 255.0) * 1023;

	SetLEDColor(0, Dimmer, 0, 0);
	SetLEDColor(1, 0, Dimmer, 0);
	SetLEDColor(2, 0, 0, Dimmer);
	WriteLEDArray();
	delay(30);

	return;
	// every 10 loops make sure the LEDs are initialized
	if (ct++ == 10) {
		ct = 0; // reset to keep from overflowing
		InitializeLEDs();
	}

	//DoColorWipe();
	int Settings[4] = { 0, 0 , 0, 255 };

	for (int i = 0; i < NumLEDs; i++) {
		// fade in red
		for (int j = 0; j < 1024; j += 32) {
			Settings[0] = j;
			//Settings[2] = constrain(LEDChannels[i][0] - j, 0, 1023);
			SetLEDColor(i, Settings);
			WriteLEDArray();
			delay(30);
		}
		// fade in green
		for (int j = 0; j < 1024; j += 32) {
			Settings[1] = j;
			//Settings[0] = constrain(LEDChannels[i][0] - j, 0, 1023);
			SetLEDColor(i, Settings);
			WriteLEDArray();
			delay(30);
		}
		// fade in blue
		for (int j = 0; j < 1024; j += 32) {
			Settings[2] = j;
			//Settings[1] = constrain(LEDChannels[i][0] - j, 0, 1023);
			SetLEDColor(i, Settings);
			WriteLEDArray();
			delay(30);
		}
	}

	
	for (int i = 0; i < NumLEDs; i++) {
		for (int j = 0; j < 3; j++) {
			LEDChannels[i][j] = 0;
		}
	}
	WriteLEDArray();
	
	// Change the colors
	//CurrentLEDSettings[0] = (CurrentLEDSettings[0] + 8) % 255;
	//CurrentLEDSettings[1] = (CurrentLEDSettings[1] + 16) % 255;
	//CurrentLEDSettings[2] = (CurrentLEDSettings[2] + 32) % 255;
}

void DoColorWipe() {
	CurrentLED = (CurrentLED++) % NumLEDs;
	SetLEDColor(CurrentLED, CurrentLEDSettings);
	WriteLEDArray();
}

void SetLEDColor(int ID, int Settings[]) {
	int Red, Green, Blue, Dimmer;
	Dimmer = ((float)Settings[3] / 255.0) * 1023;
	Red = Settings[0] * Dimmer;
	Green = Settings[1] * Dimmer;
	Blue = Settings[2] * Dimmer;

	SetLEDColor(ID, Red, Green, Blue);
}

void SetLEDColor(int ID, int Red, int Green, int Blue) {
	LEDChannels[ID][0] = Red;
	LEDChannels[ID][1] = Green;
	LEDChannels[ID][2] = Blue;
}
