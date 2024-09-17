#include <PID_v1.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

// Constants for OLED display
#define SCREEN_WIDTH 128 // OLED display width
#define SCREEN_HEIGHT 64 // OLED display height

// OLED display
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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
  Serial.begin(9600);
  // Initialize OLED display
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // Initialization failed, handle it here
    while (true);
  }
  oled.clearDisplay();
  oled.display();
  
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

  // Display the temperature on the OLED display
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setTextColor(SSD1306_WHITE);
  
  // Display the current temperature
  oled.setCursor(0, 0);
  oled.print("Temp: ");
  oled.print(Input);
  oled.println(" C");

  // Display the setpoint temperature
  oled.setCursor(0, 20);
  oled.print("Set: ");
  oled.print(Setpoint);
  oled.println(" C");
  
  oled.display();
}
