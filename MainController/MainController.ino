/*
 * System for Managing Battery Charging/Discharing through a Constant Load (to be improved to constant current)
 */

/* Connection Specification:
 *  - A0: Battery Positive
 *  - D4: Discharge Relay Control (LOW: Discharge, HIGH: Disconnect)
 *  - D5: Charge Relay Control (LOW: Charge, HIGH: Disconnect)
 *  
 *  - Battery Positive into COM on Both Relay
 *  - Charge Circuit into NO on Charge Relay
 *  - Discharge Circuit into NO on Discharge Relay
 */

/* To Do List:
 *  - Connect second relay to make measuring battery voltage actually accurate (in progress)
 *    - Disable the discharge circuit for measurement
 *  - Logging Output/Plotting
 */

// State Definitions
#define STATE_CHARGE    1
#define STATE_DISCHARGE 2
#define STATE_NOTHING   3

// Pin Config
const int PIN_CHARGE_RELAY = 5;
const int PIN_DISCHARGE_RELAY = 4;
const int PIN_BATTERY_READ = A1;

// Settings Config
const int batteryReadAverageCount = 5; // number of values to average
const float batteryChargeMin = 3.67; // minimum value to charge up to
const float batteryChargeMax = 4.2; // max value to charge to (unused currently)
const float batteryDischargeMin = 3.4; // lowest value to discharge to
const float batteryDischargeStabilizeTime = 4000; // ms to wait after stopping discharge to measure
const float batteryDischargeCheckPeriod = 10; // seconds to wait after stopping discharge to measure
const int loopDelayMS = 1000; // how long to delay between main loop interations

// Logging Variables
int chargeCount = 0, dischargeCount = 0;
int currentState = STATE_CHARGE;
long lastDischargeCheckTime = millis(); // last time to check when the discharge voltage was checked

long dischargeStartTime = 0;
long chargeStartTime = 0;


void setup() {
  // Set Pinmodes
  pinMode(PIN_CHARGE_RELAY, OUTPUT);
  pinMode(PIN_DISCHARGE_RELAY, OUTPUT);

  // Enable Serial
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    String serialCommand = Serial.readStringUntil('\n');

    if (serialCommand.equals("IDLE")) {
      Serial.println("[INFO] Entering Idle Mode");

      currentState = STATE_NOTHING;
      
    }

    else if (serialCommand.equals("CHARGE")) {
      Serial.println("[INFO] Entering Charge Mode");

      currentState = STATE_CHARGE;
    }

    else if (serialCommand.equals("DISCHARGE")) {
      Serial.println("[INFO] Entering Discharge Mode");

      currentState = STATE_DISCHARGE;
    }
    
    else {
      Serial.print("[ERROR] Invalid Command Received: "); Serial.print(serialCommand); Serial.println("");
    }
  }
  
  // check whether to charge or discharge
  float batVoltage = readBatteryVoltage();

  // print out some info
  Serial.print("[INFO] Battery Voltage: "); Serial.print(batVoltage); Serial.print("V \t");
  Serial.print("FSM State (1-CHAR, 2-DISCH, 3-NONE): "); Serial.print(currentState); Serial.println("");

  // Log the discharge data
  Serial.print("[LOG] Mode=Charge, TimeMS="); Serial.print(millis()-chargeStartTime);
  Serial.print(", Voltage="); Serial.print(batVoltage); Serial.println("");

  // check whether to charge or discharge
  if (currentState == STATE_CHARGE) {
    // in charge mode
    
    setRelayStates(STATE_CHARGE);

    // next-state check
    if (batVoltage > batteryChargeMin) {
      // battery charged enough, discharge now
      currentState = STATE_DISCHARGE;      

      lastDischargeCheckTime = millis();

      Serial.println("[INFO] Switch to Discharge\t");
      dischargeStartTime = millis();
    }
   }

   else if (currentState == STATE_DISCHARGE) {
    // in discharge mode
    // set to discharge mode
    setRelayStates(STATE_DISCHARGE);

    // check to see if discharge should stop (passed theshold time)
    if (millis() - lastDischargeCheckTime >= batteryDischargeCheckPeriod * 1000) {
      // set into literal nothing mode
      setRelayStates(STATE_NOTHING); 

      // wait a while to stabilize
      Serial.print("[INFO] Stabilize Voltage: ");
      for (int i = 0; i < batteryDischargeStabilizeTime; i += 1000) {
        batVoltage = readBatteryVoltage();
        Serial.print(batVoltage);
        Serial.print("v, ");
        delay(1000);
      }
      Serial.println("");

      // Log the discharge data
      Serial.print("[LOG] Mode=Discharge, TimeMS="); Serial.print(millis()-dischargeStartTime);
      Serial.print(", Voltage="); Serial.print(batVoltage); Serial.println("");
      
      // next-state check
      if (batVoltage < batteryDischargeMin) {
        // battery discharged to limit, charge now
        currentState = STATE_CHARGE;
  
        Serial.println("[INFO] Switch to Charge\t");
        chargeStartTime = millis();
      }

      lastDischargeCheckTime = millis();

    }

    
  }

  else if (currentState == STATE_NOTHING) {
    setRelayStates(STATE_NOTHING);
  }

  //Serial.println("");

  delay(loopDelayMS);
  
}


float readBatteryVoltage() {
  float value = 0;
  for (int i = 0; i < batteryReadAverageCount; i++) {
    value += analogRead(PIN_BATTERY_READ) *5.0/1023.0;
    delay(20);
  }

  return value / batteryReadAverageCount;
  //return  analogRead(PIN_BATTERY_READ) *5.0/1023.0; // for debugging without averaging
}

void setRelayStates(int relayState) {
  // Sets the relay outputs based on the "STATE_" state machine macros
  switch (relayState) {
    case STATE_CHARGE:
      digitalWrite(PIN_CHARGE_RELAY, LOW);
      digitalWrite(PIN_DISCHARGE_RELAY, HIGH);
    break;

    case STATE_DISCHARGE:
      digitalWrite(PIN_CHARGE_RELAY, HIGH);
      digitalWrite(PIN_DISCHARGE_RELAY, LOW);
    break;

    case STATE_NOTHING:
      digitalWrite(PIN_CHARGE_RELAY, HIGH);
      digitalWrite(PIN_DISCHARGE_RELAY, HIGH);
    break;
  }
}
