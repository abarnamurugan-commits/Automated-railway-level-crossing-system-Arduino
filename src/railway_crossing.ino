/*
  AUTOMATED RAILWAY LEVEL CROSSING SYSTEM
  ----------------------------------------
  Tinkercad Arduino Uno Prototype

  Button 1 -> Train Approaching Sensor -> D2
  Button 2 -> Train Departed Sensor   -> D3

  Servo     -> D9
  Red LED   -> D6
  Green LED -> D5
  Buzzer    -> D8

  SEQUENCE:

  NORMAL
      ↓
  TRAIN APPROACHING
      ↓
  GATE CLOSING
      ↓
  TRAIN CROSSING
  GATE CLOSED / ROAD BLOCKED
      ↓
  TRAIN CLEARED
      ↓
  SAFETY CHECK
      ↓
  GATE OPENING
      ↓
  NORMAL
*/

#include <Servo.h>

Servo gateServo;

// ---------------- PIN CONFIGURATION ----------------

const int PIN_APPROACH = 2;
const int PIN_DEPART   = 3;

const int PIN_SERVO    = 9;
const int PIN_RED      = 6;
const int PIN_GREEN    = 5;
const int PIN_BUZZER   = 8;

// ---------------- GATE ANGLES ----------------

const int GATE_OPEN_ANGLE   = 0;
const int GATE_CLOSED_ANGLE = 90;

// ---------------- SYSTEM STATES ----------------

enum State
{
  NORMAL,
  TRAIN_APPROACHING,
  GATE_CLOSING,
  TRAIN_CROSSING,
  TRAIN_CLEARED,
  GATE_OPENING
};

State currentState = NORMAL;

// ---------------- SETUP ----------------

void setup()
{
  pinMode(PIN_APPROACH, INPUT_PULLUP);
  pinMode(PIN_DEPART, INPUT_PULLUP);

  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  gateServo.attach(PIN_SERVO);

  // Start with gate OPEN
  gateServo.write(GATE_OPEN_ANGLE);

  Serial.begin(9600);

  setNormalState();
}

// ---------------- MAIN LOOP ----------------

void loop()
{
  bool approachDetected =
      (digitalRead(PIN_APPROACH) == LOW);

  bool departureDetected =
      (digitalRead(PIN_DEPART) == LOW);


  switch (currentState)
  {

    // =================================================
    // NORMAL
    // =================================================

    case NORMAL:

      if (approachDetected)
      {
        Serial.println();
        Serial.println("================================");
        Serial.println("TRAIN APPROACHING DETECTED");
        Serial.println("Preparing to close railway gate");
        Serial.println("================================");

        currentState = TRAIN_APPROACHING;
      }

      break;


    // =================================================
    // TRAIN APPROACHING
    // =================================================

    case TRAIN_APPROACHING:

      Serial.println();
      Serial.println("WARNING!");
      Serial.println("Train approaching...");
      Serial.println("Red signal ON");
      Serial.println("Buzzer ON");
      Serial.println("Gate closing soon...");

      digitalWrite(PIN_GREEN, LOW);
      digitalWrite(PIN_RED, HIGH);

      tone(PIN_BUZZER, 1000);

      // Give road users warning time
      delay(2000);

      currentState = GATE_CLOSING;

      break;


    // =================================================
    // GATE CLOSING
    // =================================================

    case GATE_CLOSING:

      Serial.println();
      Serial.println("GATE CLOSING...");
      Serial.println("Please stop road traffic.");

      gateServo.write(GATE_CLOSED_ANGLE);

      // Allow servo to reach closed position
      delay(1000);

      Serial.println();
      Serial.println("--------------------------------");
      Serial.println("GATE CLOSED");
      Serial.println("ROAD TRAFFIC BLOCKED");
      Serial.println("TRAIN CROSSING");
      Serial.println("--------------------------------");

      currentState = TRAIN_CROSSING;

      break;


    // =================================================
    // TRAIN CROSSING
    // =================================================

    case TRAIN_CROSSING:

      /*
         IMPORTANT SAFETY STATE

         The gate remains CLOSED here.

         Even if the approach button is pressed
         repeatedly, nothing happens.

         The system waits for the departure
         sensor/button.
      */

      digitalWrite(PIN_RED, HIGH);
      digitalWrite(PIN_GREEN, LOW);
      tone(PIN_BUZZER, 1000);

      // Keep gate physically closed
      gateServo.write(GATE_CLOSED_ANGLE);

      // Clear and simple status message
      Serial.println();
      Serial.println("================================");
      Serial.println("      TRAIN CROSSING");
      Serial.println("      GATE CLOSED");
      Serial.println("      ROAD BLOCKED");
      Serial.println("      WAITING FOR DEPARTURE");
      Serial.println("================================");

      /*
         ONLY the departure sensor can move
         the system out of this state.
      */

      if (departureDetected)
      {
        Serial.println();
        Serial.println("TRAIN DEPARTURE DETECTED!");

        currentState = TRAIN_CLEARED;

        // Small delay prevents repeated button detection
        delay(300);
      }

      break;


    // =================================================
    // TRAIN CLEARED
    // =================================================

    case TRAIN_CLEARED:

      Serial.println();
      Serial.println("TRAIN CLEARED");
      Serial.println("Performing safety check...");

      // Keep gate CLOSED during safety check
      gateServo.write(GATE_CLOSED_ANGLE);

      // Stop warning after train has cleared
      noTone(PIN_BUZZER);

      delay(1500);

      Serial.println("Safety check completed.");
      Serial.println("Crossing clear.");
      Serial.println("Gate can now open.");

      currentState = GATE_OPENING;

      break;


    // =================================================
    // GATE OPENING
    // =================================================

    case GATE_OPENING:

      Serial.println();
      Serial.println("GATE OPENING...");

      gateServo.write(GATE_OPEN_ANGLE);

      delay(1000);

      Serial.println();
      Serial.println("================================");
      Serial.println("GATE OPEN");
      Serial.println("ROAD TRAFFIC RESUMED");
      Serial.println("SYSTEM RETURNING TO NORMAL");
      Serial.println("================================");

      setNormalState();

      break;
  }
}


// =====================================================
// NORMAL STATE FUNCTION
// =====================================================

void setNormalState()
{
  currentState = NORMAL;

  gateServo.write(GATE_OPEN_ANGLE);

  digitalWrite(PIN_RED, LOW);
  digitalWrite(PIN_GREEN, HIGH);

  noTone(PIN_BUZZER);

  Serial.println();
  Serial.println("--------------------------------");
  Serial.println("SYSTEM STATUS: NORMAL");
  Serial.println("GATE: OPEN");
  Serial.println("ROAD: CLEAR");
  Serial.println("GREEN SIGNAL: ON");
  Serial.println("Waiting for train...");
  Serial.println("--------------------------------");
}