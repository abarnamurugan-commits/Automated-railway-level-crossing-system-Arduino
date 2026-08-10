/*
  Automated Railway Level Crossing System
  Track 2 — Tinkercad (Arduino)

  Pins below are placeholders — confirm against your Tinkercad wiring
  before uploading/simulating, then update these constants.
*/

#include <Servo.h>

Servo gate;

const int ARRIVAL_BUTTON_PIN   = 7;  // train "arriving" sensor
const int DEPARTURE_BUTTON_PIN = 6;  // train "departing" sensor
const int RED_LED_PIN          = 4;
const int GREEN_LED_PIN        = 5;
const int BUZZER_PIN           = 8;

const int GATE_OPEN_ANGLE   = 0;
const int GATE_CLOSED_ANGLE = 90;

bool gateClosed = false;

void setup() {
  gate.attach(9);
  pinMode(ARRIVAL_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DEPARTURE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  openGate();
}

void loop() {
  if (!gateClosed && digitalRead(ARRIVAL_BUTTON_PIN) == LOW) {
    closeGate();
  }
  if (gateClosed && digitalRead(DEPARTURE_BUTTON_PIN) == LOW) {
    openGate();
  }
}

void closeGate() {
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000);
  gate.write(GATE_CLOSED_ANGLE);
  gateClosed = true;
}

void openGate() {
  noTone(BUZZER_PIN);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, HIGH);
  gate.write(GATE_OPEN_ANGLE);
  gateClosed = false;
}
