#include <Wire.h>

const int sda = 21;
const int scl = 22;
const int latchPin = 18; // Latch pin of 74HC595
const int clockPin = 5; // Clock pin of 74HC595
const int dataPin = 19; // Data pin of 74HC595 

byte GYRO_XOUT_H = 0;
byte GYRO_XOUT_L = 0;
int16_t GYRO_XOUT_ALL = 0;
float GYRO_X;

double current_time = 0;
double previous_time = 0;
double elapsed_time = 0;

double angle_x = 0;

double offset_x = 0.0;

int counter = 100;
int led_number = 0;
byte led_data = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(21,22);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

void loop() {
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(0x68,1);
  GYRO_XOUT_H = Wire.read();

  Wire.beginTransmission(0x68);
  Wire.write(0x44);
  Wire.endTransmission();
  Wire.requestFrom(0x68,1);
  GYRO_XOUT_L = Wire.read();

  GYRO_XOUT_ALL = GYRO_XOUT_H << 8 | GYRO_XOUT_L;
 
  GYRO_X = GYRO_XOUT_ALL/131.0;

  if (counter <= 100){
  offset_x += GYRO_X/100;
  counter ++;
  }

  if(counter > 100){
    current_time = millis();

  elapsed_time = (current_time - previous_time)/1000;
  angle_x = angle_x + (GYRO_X - offset_x) * elapsed_time;
  // Serial.println(angle_x);
  led_number = map(angle_x,-90,90,0,7);
  Serial.println(led_number);

  led_data = 0;
  bitSet(led_data,led_number);
  updateShiftRegister();
  previous_time = current_time;
  }
  delay(100);
}

void updateShiftRegister()
  {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, led_data);
    digitalWrite(latchPin, HIGH);
  }