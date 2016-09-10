#include <EEPROM.h>
#include <SoftwareSerial.h>

#define BLE   0

// Mechanical parameters.
const int radius = 3537;
const int PI = 314;

SoftwareSerial sSerial(9, 8);

const int cEncoderPin = 2;
const int cMotorIn1 = 3;
const int cMotorIn2 = 4;
const int cMotorIn3 = 5;
const int cMotorIn4 = 6;

byte uPWM = 100;
byte dPWM = 100;

volatile int ticks = 0;
int ticksShadow = 0;

void setup() {
  pinMode(cMotorIn1, OUTPUT);
  pinMode(cMotorIn2, OUTPUT);
  pinMode(cMotorIn3, OUTPUT);
  pinMode(cMotorIn4, OUTPUT);
  
  digitalWrite(cMotorIn1, LOW);
  digitalWrite(cMotorIn2, LOW);
  digitalWrite(cMotorIn3, LOW);
  digitalWrite(cMotorIn4, LOW);

  Serial.begin(38400);
  sSerial.begin(38400);

  // Setup interrupt for encoder.
  pinMode(cEncoderPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(cEncoderPin), EncoderISR, RISING);

  Serial.setTimeout(5000);
  DisplayUsage();

  EEPROM.get(0, uPWM);
  EEPROM.get(1, dPWM);

  Serial.print("Up PWM: ");
  Serial.println(uPWM);
  Serial.print("Down PWM: ");
  Serial.println(dPWM);
}

int cmd = 0;

void loop() {
  if (ticks != ticksShadow) {
    Serial.println(ticks);
    ticksShadow = ticks;  
  }

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
          MoveUpRound(1, 255);
          break;
        case 2:
          MoveDownRound(1, 150);
          break;
      }
    }
  }
#else
  ConsoleTest();
#endif
}

void SaveToEEPROM()
{
  // Save to EEPROM.
  EEPROM.put(0, uPWM);
  EEPROM.put(1, dPWM);

  Serial.print("Up PWM: ");
  Serial.println(uPWM);
  Serial.print("Down PWM: ");
  Serial.println(dPWM);
}

void DisplayUsage()
{
  Serial.println("\n(u) Move Up minium PWM.");
  Serial.println("(d) Move Down minium PWM.");
  Serial.println("(e) Save to EEPROM.");
  Serial.println("(i) Move up.");
  Serial.println("(k) Move down.");
  Serial.println("(t) Tunning minium speed.");
  Serial.println("(x) Demo.\n");
}

void ConsoleTest()
{
  if (Serial.available()) {
    cmd = Serial.read();

    switch (cmd) {
      case 'u':
        Serial.print("Current Up PWM: ");
        Serial.println(uPWM);
        
        uPWM= Serial.parseInt();
        Serial.print("New Up PWM: ");
        Serial.println(uPWM);
        break;
      case 'd':
        Serial.print("Current Down PWM: ");
        Serial.println(dPWM);
        
        dPWM= Serial.parseInt();
        Serial.print("New Down PWM: ");
        Serial.println(dPWM);
        break;
      case 'e':
        SaveToEEPROM();
        break;
      case 'i':
        MoveUpRound(1, uPWM);
        break;
      case 'k':
        MoveDownRound(1, dPWM);
        break;
      case 't':
        TuningSpeed();
        break;
      case 'x':
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
      default:
        DisplayUsage();
        break;
    }
  }
}

void MoveUp(int value)
{
  digitalWrite(cMotorIn1, LOW);
  analogWrite(cMotorIn2, value);

  analogWrite(cMotorIn3, value);
  digitalWrite(cMotorIn4, LOW);
}

void MoveDown(int value)
{
  analogWrite(cMotorIn1, value);
  digitalWrite(cMotorIn2, LOW);

  digitalWrite(cMotorIn3, LOW);
  analogWrite(cMotorIn4, value);
}

void Hold()
{
  digitalWrite(cMotorIn1, HIGH);
  digitalWrite(cMotorIn2, HIGH);
  digitalWrite(cMotorIn3, HIGH);
  digitalWrite(cMotorIn4, HIGH);
}

void Release()
{
  digitalWrite(cMotorIn1, LOW);
  digitalWrite(cMotorIn2, LOW);
  digitalWrite(cMotorIn3, LOW);
  digitalWrite(cMotorIn4, LOW);  
}

volatile bool encoding = false;
volatile int tickGoal = 0;

void MoveUpRound(int rounds, int value)
{
  ticks = 0;
  tickGoal = ticks + 60 * rounds;
  encoding = true;
  MoveUp(value);
}

void MoveDownRound(int rounds, int value)
{
  ticks = 0;
  tickGoal = ticks + 60 * rounds;
  encoding = true;
  MoveDown(value);
}

void TuningSpeed()
{
  long t;

  // Tuning Move up speed.
  encoding = true;

  for (uPWM = 20; uPWM < 256; uPWM ++) {
    Serial.print("speed: ");
    Serial.println(uPWM);

    ticks = ticksShadow = 0;
    tickGoal = ticks + 10;

    Release();
    delay(500);
    t = millis();
    MoveUp(uPWM);
    
    while (encoding && millis() - t < 1000) delay(1);

    if (encoding == false) {
      Serial.print("Minium move up speed: ");
      Serial.println(uPWM);
      break;
    }
  }

  // Tuning Move down speed.
  encoding = true;
  
  for (dPWM = 20; dPWM < 256; dPWM ++) {
    Serial.print("speed: ");
    Serial.println(dPWM);

    ticks = ticksShadow = 0;
    tickGoal = ticks + 10;

    Release();
    delay(500);
    t = millis();
    MoveDown(dPWM);
    
    while (encoding && millis() - t < 1000) delay(1);

    if (encoding == false) {
      Serial.print("Minium move down speed: ");
      Serial.println(dPWM);
      break;
    }
  }

  SaveToEEPROM();
}

volatile unsigned long prevMicros = 0;

void EncoderISR()
{
  if (!encoding) return ;

  if (micros() - prevMicros < 10000) return ;

  prevMicros = micros();
  ticks ++;
    
  if (ticks == tickGoal) {
    encoding = false;
    Hold();      
  }
}

