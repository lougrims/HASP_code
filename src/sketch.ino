const int ledPin = 13;      // the pin that the LED is attached to
const int sensorNumber = 16; //Number of sensors
const int Cam1PowerPin=43; //Control camera control
const int Cam1GrdPin=45; //should always be ground
const int Cam1TestPin=51; //not implemented yet
const int Cam2PowerPin=33; //Control camera control
const int Cam2GrdPin=35; //should always be ground
const int Cam2TestPin=31; //not implemented yet

unsigned long periodCam1=40000;
unsigned long periodCam2=40000;

unsigned long delayCam1=10000;
unsigned long delayCam2=10000;

bool Cam1On,Cam2On;
unsigned long timeCam1, timeCam2;

int sensorValues[sensorNumber];
unsigned long time;
char inString[7]="      ";

void setup(){
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

	Cam1On=false;
	TurnCamOn(Cam1PowerPin);
	delay(4000);
	TurnCamOff(Cam1PowerPin);
	timeCam1=0;
	timeCam2=0;

}

void TurnCamOn(int CamPowerPin){
	// Dumb on function
	digitalWrite(Cam1PowerPin, HIGH);
	delay(100);
	digitalWrite(Cam1PowerPin, LOW);
}

void TurnCamOff(int CamPowerPin){
	// Dumb on function
	digitalWrite(Cam1PowerPin, HIGH);
	delay(1200);
	digitalWrite(Cam1PowerPin, LOW);
}

void ReadSensors(int* sensorArray) {
	int i;
	for(i=0;i<sensorNumber;i++){ // reads each sensor
		sensorArray[i]=analogRead(i);
	}
}

void DebugSensors(int* sensorArray) {
	int i;
	for(i=0;i<sensorNumber;i++){ //print each sensor value
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
	}
	Serial3.write(checksum);
	Serial3.println();
	Serial3.flush();
}

void ReadSerial(){
	while (Serial3.available()>0) {
		for(int i=1;i<7;i++) {
			inString[i]=inString[i+1];
		}
		inString[7]=Serial3.read();
		if (inString[0]==(char) 1 && inString[1]==(char) 2 && inString[4]==(char) 3 && inString[5]==(char) 13 && inString[6]==(char) 10){
			if (inString[2]==(char)0 && inString[3]==(char)255){ // low frequency pictures
				delayCam1=240000;
				delayCam2=240000;
			}
			if (inString[2]==(char)255 && inString[3]==(char)0){//medium frequency pictures
				delayCam1=60000;
				delayCam2=60000;
			}
			if (inString[2]==(char)255 && inString[3]==(char)255){//high frequency pictures
				delayCam1=30000;
				delayCam2=30000;
			}
			if (inString[2]==(char)0 && inString[3]==(char)0){
				delayCam1=60000000; //set a picture every 1000 minutes
				delayCam2=60000000;
			}
		}

	}
}

void loop() {
	ReadSerial();
	ReadSensors(sensorValues);
	//DebugSensors(sensorValues);
	SendSensors(sensorValues);
	Serial.println("");
	Serial.print("sensor sent, time ");
	Serial.println(millis());
	Serial.print("next pic at");
	Serial.println((timeCam1+periodCam1));


//	Cam1On=digitalRead(Cam1TestPin);
//	if (Cam1On==HIGH){
//	Serial.println("test cam 1 on");
//	}	
//	else if (Cam1On==LOW){
//	Serial.println("test cam 1 off");
//	}
//	Cam2On=digitalRead(Cam2TestPin);


	if (!Cam1On && millis()>(timeCam1+periodCam1)){
		Serial.println("Cam1 on ");
		TurnCamOn(Cam1PowerPin);
		timeCam1=millis();
		Cam1On==true;
	}
	else if (Cam1On && millis()>timeCam1+delayCam1){
		Serial.println("Cam1 off");
		TurnCamOff(Cam1PowerPin);
		Cam1On=false;
	}

//	if (Cam2On && millis()>timeCam2+periodCam2){ // Do we want the pic at the same time (battery usage?)
//		TurnCamOn(Cam2PowerPin,Cam2TestPin);
//		timeCam2=millis();
//	}
//	if (!Cam2On && millis()>timeCam2+delayCam2){
//		TurnCamOff(Cam2PowerPin,Cam2TestPin);
//	}
//
	delay(5000);

}

