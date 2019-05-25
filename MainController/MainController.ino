/*
 * System for Managing Battery Charging/Discharing through a Constant Load (to be improved to constant current)
 */

/* Connection Specification:
 *  - A0: Battery Positive
 *  - D4: Relay Control (LOW: Discharge, HIGH: Charge)
 *  
 *  - Battery Positive into COM on Relay
 *  - Charge circuit into NC on Relay
 *  - Discharge Circuit into NO on Relay
 */

/* To Do List:
 *  - Connect second relay to make measuring battery voltage actually accurate
 *    - Disable the discharge circuit for measurement
 *  - Logging Output/Plotting
 */

// Pin Config
const int PIN_RELAY = 4;
const int PIN_BATTERY_READ = A0;

// Settings Config
const int batteryReadAverageCount = 5; // number of values to average
const float batteryChargeMin = 3.67; // minimum value to charge up to
const float batteryChargeMax = 4.2; // max value to charge to (unused currently)
const float batteryDischargeMin = 3.3; // lowest value to discharge to

// Logging Variables
int chargeCount = 0, dischargeCount = 0;
bool isChargeMode = true; // either in charge or discharge mode

void setup() {
  // Set Pinmodes
  pinMode(PIN_RELAY, OUTPUT);

  // Enable Serial
  Serial.begin(9600);
}

void loop() {
  // check whether to charge or discharge
  float batVoltage = readBatteryVoltage();

  // print out some info
  Serial.print("Battery Voltage: "); Serial.print(batVoltage); Serial.print("v \t");
  Serial.print("Charge Mode?: "); Serial.print(isChargeMode); Serial.print("\t");

  // check whether to charge or discharge
  if (isChargeMode) {
    // in charge mode
    
    digitalWrite(PIN_RELAY, HIGH);

    // next-state check
    if (batVoltage > batteryChargeMin) {
      // battery charged enough, discharge now
      isChargeMode = false;
      digitalWrite(PIN_RELAY, LOW);
      
      Serial.print("Switch to Discharge\t");

    }
   }

   else {
    // in discharge mode

    digitalWrite(PIN_RELAY, LOW);

    // next-state check
    if (batVoltage < batteryDischargeMin) {
      // battery discharged to limit, charge now
      isChargeMode = true;
      digitalWrite(PIN_RELAY, HIGH);

      Serial.print("Switch to Charge\t");
    }
    
  }

  Serial.println("");

  delay(1000);
  
}


float readBatteryVoltage() {
  float value = 0;
  for (int i = 0; i < batteryReadAverageCount; i++) {
    value += analogRead(PIN_BATTERY_READ) *5.0/1023.0;
  }

  return value / batteryReadAverageCount;
  //return  analogRead(PIN_BATTERY_READ) *5.0/1023.0;
}
