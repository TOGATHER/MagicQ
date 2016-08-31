#include <SoftwareSerial.h>
#define BLE   1

SoftwareSerial sSerial(8, 7);

const int cMotorIn1 = 2;
const int cMotorIn2 = 3;
const int cMotorIn3 = 4;
const int cMotorIn4 = 5;

void setup() {
  pinMode(cMotorIn1, OUTPUT);
  pinMode(cMotorIn2, OUTPUT);
  pinMode(cMotorIn3, OUTPUT);
  pinMode(cMotorIn4, OUTPUT);
  
  digitalWrite(cMotorIn1, LOW);
  digitalWrite(cMotorIn2, LOW);
  digitalWrite(cMotorIn3, LOW);
  digitalWrite(cMotorIn4, LOW);

  Serial.begin(115200);
  sSerial.begin(38400);
}

int cmd = 0;

void loop() {
#if BLE
/*
  int c;
  if (sSerial.available()) {
    c = sSerial.read();
    Serial.println(c, HEX);
  }
  return ;
*/
  int c;

  if (sSerial.available()) {
    // Message Start ID.
    if ((c = sSerial.read()) != 0xAB) return ;
    Serial.println(c);
    
    // Message Data.
    if ((c = sSerial.read()) != 0xAD) return ;
    Serial.println(c);
    
    while (sSerial.available()) {
      c = sSerial.read();
      Serial.println(c);
      
      switch (c) {
        case 1:
          MoveUp(255);
          delay(1000);
          Hold();
          break;
        case 2:
          MoveDown(150);
          delay(1000);
          Hold();
          break;
      }
    }
  }
#else
  ConsoleTest();
#endif
}

void ConsoleTest()
{
  if (Serial.available()) {
    cmd = Serial.read();

    switch (cmd) {
      case 'i':
        MoveUp(255);
        delay(1000);
        Hold();
        break;
      case 'k':
        MoveDown(150);
        delay(1500);
        Hold();
        break;
      case 's':
        // Straight up.
        MoveUp(255);
        delay(3000);

        // Round 1
        // Fast down.
        MoveDown(150);
        delay(1500);

        Hold();
        delay(1000);

        // Fast up.
        MoveUp(255);
        delay(1800);

        // Round 2
        // Fast down.
        MoveDown(150);
        delay(1500);

        Hold();
        delay(1000);

        // Fast up.
        MoveUp(255);
        delay(1800);

        // Round 3
        // Fast down.
        MoveDown(150);
        delay(1500);

        Hold();
        delay(1000);

        // Fast up.
        MoveUp(255);
        delay(1800);

        Hold();
        break;
    }
  }
}

void MoveUp(int value)
{
  digitalWrite(cMotorIn1, LOW);
  analogWrite(cMotorIn2, value);
//  digitalWrite(cMotorIn2, HIGH);

//  digitalWrite(cMotorIn3, HIGH);
  analogWrite(cMotorIn3, value);
  digitalWrite(cMotorIn4, LOW);
}

void MoveDown(int value)
{
//  digitalWrite(cMotorIn1, HIGH);
  analogWrite(cMotorIn1, value);
  digitalWrite(cMotorIn2, LOW);

  digitalWrite(cMotorIn3, LOW);
  analogWrite(cMotorIn4, value);
//  digitalWrite(cMotorIn4, HIGH);
}

void Hold()
{
  digitalWrite(cMotorIn1, HIGH);
  digitalWrite(cMotorIn2, HIGH);
  digitalWrite(cMotorIn3, HIGH);
  digitalWrite(cMotorIn4, HIGH); 
}

