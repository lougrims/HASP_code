const int sensorNumber = 14; //Number of sensors
const int Cam1PowerPin=43; //Control camera control
const int Cam1GrdPin=45; //should always be ground
const int Cam2PowerPin=35; //Control camera control
const int Cam2GrdPin=33; //should always be ground

unsigned long periodCam1=30000;
unsigned long periodCam2=30000;

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
	pinMode(Cam1PowerPin, OUTPUT);
	pinMode(Cam1GrdPin, OUTPUT);
	pinMode(Cam2PowerPin, OUTPUT);
	pinMode(Cam2GrdPin, OUTPUT);

	digitalWrite(Cam1GrdPin, HIGH);
	digitalWrite(Cam1PowerPin, LOW);
	digitalWrite(Cam2GrdPin, HIGH);
	digitalWrite(Cam2PowerPin, LOW);

	Cam1On=false;
	Cam2On=false;
	TurnCamOn(Cam1PowerPin);
	delay(4000);
	TurnCamOff(Cam1PowerPin);
	TurnCamOn(Cam2PowerPin);
	delay(4000);
	TurnCamOff(Cam2PowerPin);
	timeCam1=0;
	timeCam2=periodCam1/2;

		for(int j=0;j<sensorNumber;j++){
			pinMode(j,INPUT);
		}
}

void TurnCamOn(int CamPowerPin){
	// Dumb on function
	digitalWrite(CamPowerPin, HIGH);
	delay(100);
	digitalWrite(CamPowerPin, LOW);
}

void TurnCamOff(int CamPowerPin){
	// Dumb on function
	digitalWrite(CamPowerPin, HIGH);
	delay(2500);
	digitalWrite(CamPowerPin, LOW);
}

void ReadSensors(int* sensorArray) {
	int i;
	/* Sensors values, meaning:
	   - 1: V V_mppt1 + V_mppt2
	   - 2: A MPPT1
	   - 3: V sp1 + V_mppt2
	   - 4: A Sp1
	   - 5: V_mppt2
	   - 6: A mppt2
	   - 7: V sp2
	   - 8: A sp2
	   - 9: V batt
	   - 10: A batt
	   - 11: V cam2
	   - 12: A_cam2
	   - 13: V cam1
	   - 14: A_cam1 */ 
	for(i=0;i<sensorNumber;i++){ // reads each sensor
		sensorArray[i]=analogRead(i);
	}
}

void DebugSensors(int* sensorArray) {
	int i;
	Serial.print("Vm1   ");
	Serial.print("Am1   ");
	Serial.print("Vs1   ");
	Serial.print("As1   ");
	Serial.print("Vm2   ");
	Serial.print("Am2   ");
	Serial.print("Vs2   ");
	Serial.print("As2   ");
	Serial.print("Vba   ");
	Serial.print("Aba   ");
	Serial.print("Vc2   ");
	Serial.print("Ac2   ");
	Serial.print("Vc1   ");
	Serial.println("Ac1   ");

	sensorArray[0]-=sensorArray[4];
	sensorArray[2]-=sensorArray[4];

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
			if (inString[2]==(char)0 && inString[3]==(char)0){//camera off
				delayCam1=60000000; //set a picture every 1000 minutes
				delayCam2=60000000;
			}
		}

	}
}

void loop() {
	ReadSerial();
	ReadSensors(sensorValues);
	DebugSensors(sensorValues);
	SendSensors(sensorValues);
	Serial.println("");
	Serial.print("sensor sent, time ");
	Serial.print(millis());
	Serial.print(" next pic at ");
	Serial.println((timeCam1+periodCam1));


	//	Cam1On=digitalRead(Cam1TestPin);
	//	if (Cam1On==HIGH){
	//	Serial.println("test cam 1 on");
	//	}	
	//	else if (Cam1On==LOW){
	//	Serial.println("test cam 1 off");
	//	}
	//	Cam2On=digitalRead(Cam2TestPin);


	if (!Cam1On && !Cam2On && millis()>(timeCam1+periodCam1)){
		Serial.println("Cam1 on ");
		TurnCamOn(Cam1PowerPin);
		timeCam1=millis();
		Cam1On=true;
	}
	else if (Cam1On && millis()>timeCam1+delayCam1){
		Serial.println("Cam1 off");
		TurnCamOff(Cam1PowerPin);
		Cam1On=false;
	}
	if (!Cam2On && !Cam1On && millis()>(timeCam2+periodCam2)){
		Serial.println("Cam2 on ");
		TurnCamOn(Cam2PowerPin);
		timeCam2=millis();
		Cam2On=true;
	}
	else if (Cam2On && millis()>timeCam2+delayCam2){
		Serial.println("Cam2 off");
		TurnCamOff(Cam2PowerPin);
		Cam2On=false;
	}

	//	if (Cam2On && millis()>timeCam2+periodCam2){ // Do we want the pic at the same time (battery usage?)
	//		TurnCamOn(Cam2PowerPin,Cam2TestPin);
	//		timeCam2=millis();
	//	}
	//	if (!Cam2On && millis()>timeCam2+delayCam2){
	//		TurnCamOff(Cam2PowerPin,Cam2TestPin);
	//	}
	//
	delay(2000);

}

