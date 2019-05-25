// Code for battery tester.
// This tester is designed to charge/discharge the battery multiple times in order to make useful battery discharge curves.

#define PWM_PIN 5
#define RESISTOR_READ A0
#define BATTERY_READ A1

const double RESISTOR_VALUE = 0.7;

void setup() {
  pinMode(PWM_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  double batVolt = readBatteryVoltage();
  if((batVolt > 3.35)) {
    setCurrent(5); //set to 4A
  }
  delay(500);
  Serial.print("Discharge: \t");
  Serial.print("Resistor A: ");
  Serial.print(readResistorCurrent());
  Serial.print("\tResistor V: ");
  Serial.print(readResistorVoltage());
  Serial.print("\tBattery V: ");
  Serial.println(batVolt);
  
  delay(500);

  setCurrent(3.35); //set to 0V

  delay(1500); //Need time for the voltage on the battery to recover after the heavy current draw

  Serial.print("No Discharge: \t");
  Serial.print("\tBattery V: ");
  Serial.println(readBatteryVoltage());
}

/* Needed functions:
 *  - read the current
 *  - read the battery voltage
 *  - make sure the battery voltage is at a safe level
 *  - 
 */

 double readResistorVoltage(){
  return analogRead(RESISTOR_READ)*5.0/1024;
 }

 double readResistorCurrent(){
  return (readResistorVoltage()/RESISTOR_VALUE);
 }

 double readBatteryVoltage(){
  return analogRead(BATTERY_READ)*5.0/1024;
 }

 void setCurrent(double current){
  setPWMVoltage(current * 1); // This function needs testing! Coefficient needs to be changed
 }

 void setPWMVoltage(double voltage){
  analogWrite(PWM_PIN, (voltage * 255 / 5));
 }
