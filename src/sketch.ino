const int ledPin = 13;      // the pin that the LED is attached to
const int sensorNumber = 16; //Number of sensors
const int Cam1PowerPin=43; //Control camera control
const int Cam1GrdPin=45; //should always be ground
const int Cam1TestPin=41; //not implemented yet

void setup()
{
	// initialize the serial communication with computer for debug
	Serial.begin(9600);

	// initialize the serial communication with HASP transmition
	Serial3.begin(1200);
	// initialize the ledPin as an output:
	pinMode(ledPin, OUTPUT);
	pinMode(Cam1PowerPin, OUTPUT);
	pinMode(Cam1GrdPin, OUTPUT);
	pinMode(Cam1TestPin, INPUT);

	digitalWrite(Cam1GrdPin, HIGH);
	digitalWrite(Cam1PowerPin, LOW);

}

void TurnCamOn(int CamPowerPin, int CamTestPin){
	// Dumb on function
	digitalWrite(Cam1PowerPin, LOW);
	digitalWrite(Cam1PowerPin, HIGH);
	delay(100);
	digitalWrite(Cam1PowerPin, LOW);
}

void TurnCamOff(int CamPowerPin, int CamTestPin){
	// Dumb on function
	digitalWrite(Cam1PowerPin, LOW);
	digitalWrite(Cam1PowerPin, HIGH);
	delay(1200);
	digitalWrite(Cam1PowerPin, LOW);
}

void ReadSensors(int* sensorArray) {
	int i;
	for(i=0;i<sensorNumber;i++){
		sensorArray[i]=analogRead(i);
	}
}


void DebugSensors(int* sensorArray) {
	int i;
	for(i=0;i<sensorNumber;i++){
		Serial.print(sensorArray[i]*5.0/1023);
		Serial.print("  ");
	}
	Serial.println();
}


void SendSensors(int* sensorArray) {
	int i,checksum;
	checksum=(int)'!';

	Serial3.print("!");
	for(i=0;i<sensorNumber;i++){
		Serial3.write(sensorArray[i]);
			checksum+=sensorArray[i];
		if(i<sensorNumber-1){
		Serial3.print(",");
		checksum+= (int)',';
		}
	}
	Serial3.print("*");
	Serial3.write(checksum);
	Serial3.println();
	Serial3.flush();
}


void loop() {
	int sensorValues[sensorNumber];
	ReadSensors(sensorValues);
	//DebugSensors(sensorValues);
	SendSensors(sensorValues);
	TurnCamOn(Cam1PowerPin,Cam1TestPin);
	delay(20000);
	TurnCamOff(Cam1PowerPin,Cam1TestPin);
	delay(10000);

}

