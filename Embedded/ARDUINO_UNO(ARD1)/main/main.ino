#include <Wire.h>
#include <Servo.h>
#define MY_ADDRESS 2 


Servo myServo;
const int led1 = 7; 
const int led2 = 6;  
const int led3 = 5;  // LED 5
const int servo_pin = 8 ; 
const int soilMoisturePin = A3;
int moistureSensor;
int moisture;


void setup() {
  Wire.begin(MY_ADDRESS); // Slave address
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  Serial.println("Slave 1 Ready to work");
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  myServo.attach(servo_pin);
}

void loop() {
  moistureSensor = analogRead(soilMoisturePin);
  if(moistureSensor >= 800){
    moisture = 250; // Mositure more that 80%
  } else if(moistureSensor >= 500 && moistureSensor < 800){
    moisture = 170; // Moisture less than 50%
  } else {
    moisture = 80;// Moisture between 80% and 50%
  }
  Serial.print("Moisture S1 = ");
  Serial.println(moistureSensor);
  Serial.print("Mapping S1= ");
  Serial.println(moisture);
  delay(250);
}

void receiveEvent(int howMany) {
  char buffer[10];
  readMessage(buffer, sizeof(buffer));
  
  Serial.print("Received: ");
  Serial.println(buffer);

  char pos;
  int rate;
  if (parseMessage(buffer, pos, rate)) {
    Serial.print("---Parsed rate: ");
    Serial.println(rate);
    Serial.print("---Parsed position: ");
    Serial.println(pos);

    setServoPosition(pos);
    setLEDs(rate);
  }
}

void requestEvent(){
  Wire.write((byte*)&moisture, sizeof(moisture)); // Sizeof int is 2 bytes
}

void readMessage(char* buffer, size_t bufferSize) {
  byte i = 0;
  while (Wire.available() && i < bufferSize - 1) {
    buffer[i++] = Wire.read();
  }
  buffer[i] = '\0'; // null-terminate
}

bool parseMessage(char* buffer, char& pos, int& rate) {
  char* dashPtr = strchr(buffer, '-');
  if (dashPtr == NULL) return false;

  *dashPtr = '\0';
  pos = buffer[0];
  rate = atoi(dashPtr + 1);
  return true;
}

void setServoPosition(char pos) {
  int position_rotate = (pos == 'L') ? 60 : 120;
  myServo.write(position_rotate);
}

void setLEDs(int rate) {
  digitalWrite(led1, rate >= 5);
  digitalWrite(led2, rate >= 10);
  digitalWrite(led3, rate >= 15);
}
