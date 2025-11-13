#include <Servo.h>
#include <EEPROM.h>

const int ledPin = 10;     
const int buttonPin = 2;    
const int servoPin = 9;    

Servo servo;

volatile bool buttonFlag = false;   
volatile bool timerFlag = false;   

bool ledState = false;
bool pressed = false;

int servoAngle = 0;
int storedAngle = 0;
int pressCount = 0;


const byte VALID_MARK = 0xA7;
const int addrValid = 0;
const int addrAngle = 1;
const int addrPressCount = 3;


unsigned long lastTick = 0;
const unsigned long tickInterval = 10; 

void onButtonPress() {
  if (digitalRead(buttonPin) == LOW) {
    buttonFlag = true;
  }
}

ISR(TIMER2_COMPA_vect) {
  timerFlag = true;
}

void loadEEPROM() {
  byte valid = EEPROM.read(addrValid);
  if (valid == VALID_MARK) {
    EEPROM.get(addrAngle, storedAngle);
    EEPROM.get(addrPressCount, pressCount);
  } else {
    storedAngle = 0;
    pressCount = 0;
    EEPROM.write(addrValid, VALID_MARK);
    EEPROM.put(addrAngle, storedAngle);
    EEPROM.put(addrPressCount, pressCount);
  }
}

void saveEEPROM() {
  EEPROM.put(addrAngle, storedAngle);
  EEPROM.put(addrPressCount, pressCount);
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  servo.attach(servoPin);
  servo.write(0);

  loadEEPROM();

  attachInterrupt(digitalPinToInterrupt(buttonPin), onButtonPress, FALLING);

  noInterrupts();
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;
  OCR2A = 156; // 16 MHz / 1024 preskaleris ≈ 15625 Hz → 156 count = ~10 ms
  TCCR2A |= (1 << WGM21);                     // CTC rezimas
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // preskaleris 1024
  TIMSK2 |= (1 << OCIE2A);                    // ijungiam compare match pertraukti
  interrupts();

  Serial.begin(9600);
  Serial.println("HW2: LED + Servo + Mygtukas + EEPROM (INT0 + Timer2)");
  Serial.print("Issaugotas kampas: "); Serial.println(storedAngle);
  Serial.print("Paspaudimu skaicius: "); Serial.println(pressCount);
}

void loop() {
  if (timerFlag) {
    timerFlag = false;
  }

  if (buttonFlag) {
    buttonFlag = false;

    pressed = !pressed;
    ledState = pressed;
    digitalWrite(ledPin, ledState);

    if (pressed) {
      servoAngle = 90;  
    } else {
      servoAngle = 0;   
    }

    servo.write(servoAngle);
    pressCount++;
    storedAngle = servoAngle;
    saveEEPROM();

    Serial.print("Mygtuko paspaudimas #");
    Serial.print(pressCount);
    Serial.print(" → Servo kampas: ");
    Serial.println(servoAngle);
  }
}
