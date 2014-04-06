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

int CurrentScene[NumLEDs][3] = { 0 };
int NextScene[NumLEDs][3] = { 0 };

int DimSettings[4] = { 1023, 0, 0, 128 };
int BrightSettings[4] = { 1023, 0, 0, 1023 };

void setup() {
	pinMode(datapin, OUTPUT);
	pinMode(latchpin, OUTPUT);
	pinMode(enablepin, OUTPUT);
	pinMode(clockpin, OUTPUT);

	digitalWrite(latchpin, LOW);
	digitalWrite(enablepin, LOW);

	InitializeLEDs();

	// Start Out
	/// We're going to need a setup for each function then a loop for each function
	/// This is the Chaser setup function
	for (int i = 0; i < NumLEDs; i++) {
		SetCurrentSceneLEDSettings(i, DimSettings);
		SetNextSceneLEDSettings(i, DimSettings); // don't forget to set the default for the next scene
	}
	WriteLEDArray();
}

void loop() {
	/// This is the Chaser loop function
	for (int i = 0; i < NumLEDs; i++) {
		SetNextSceneLEDSettings(i, BrightSettings);
		FadeToNextScene(20, 30);
		for (int j = 0; j < NumLEDs; j++) {
			SetNextSceneLEDSettings(j, DimSettings);
		}
	}
	return;
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
}