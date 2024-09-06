#include <PID_v1.h>
#include <EEPROM.h>

// Define pins for temperature and setpoint
#define temperaturePin  23 // Analog pin for temperature
#define currentControlIron 16 // Iron current controlling pin

// Define variables we'll be connecting to
double Setpoint = 150;
double Input, Output;

// Define the aggressive and conservative tuning parameters
double aggKp = 4, aggKi = 0.2, aggKd = 1;
double consKp = 1, consKi = 0.05, consKd = 0.25;

// Initialize the PID controller and set initial parameters
PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

void setup() {
  // Read setpoint from EEPROM
  EEPROM.get(0, Setpoint);

  // Set up the PID controller
  myPID.SetOutputLimits(0, 220);
  myPID.SetMode(AUTOMATIC);

  pinMode(currentControlIron, OUTPUT);
  pinMode(temperaturePin, INPUT);
}

double readTemperature(){
  // Read temperature
  double Input = analogRead(temperaturePin);
  Serial.println(Input);
  // Transform the 10-bit reading into degrees Celsius
  Input = map(Input, 0, 450, 25, 350); //(Input,min sensor, max sensor, min target, max target)
  Serial.println("mapped:");
  Serial.println(Input);
  return Input;
}

void loop() {
  // Read temperature in Celsius
  double Input = readTemperature();

  // Compute the gap between the setpoint and the input
  double gap = abs(Setpoint - Input);

  // Set the tuning parameters based on the gap
  if (gap < 10) {
    myPID.SetTunings(consKp, consKi, consKd);
  } else {
    myPID.SetTunings(aggKp, aggKi, aggKd);
  }

  // Compute the PID output and drive the output
  myPID.Compute();
  analogWrite(16, Output);

}
