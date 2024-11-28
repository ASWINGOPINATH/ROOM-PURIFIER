#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

// LCD pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// GSM module pins
SoftwareSerial gsm(4, 5);

// Pin definitions
#define DUST_LED 3
#define DUST_V0 A0
#define CO_SENSOR A1
#define HUMIDITY_SENSOR A2
#define HUMIDIFIER 10
#define RELAY_HV 11
#define MOTOR 2
#define BUZZER 13

// Thresholds and ranges
const int dustThreshold = 300; 
const int coThreshold = 500;   
const int humidityLow = 30;    
const int humidityHigh = 70;   

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);

  lcd.begin(16, 2);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  // Set pin modes
  pinMode(DUST_LED, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);
  pinMode(RELAY_HV, OUTPUT);
  pinMode(MOTOR, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
  // Ensure devices are off initially
  digitalWrite(HUMIDIFIER, LOW);
  digitalWrite(RELAY_HV, LOW);
  digitalWrite(MOTOR, LOW);
  digitalWrite(BUZZER, LOW);
}

void loop() {
  // Read dust sensor value
  int dustValue = readDustSensor();
  lcd.setCursor(0, 0);
  lcd.print("Dust: ");
  lcd.print(dustValue);
  lcd.print("ug/m3");

  // Activate or deactivate devices based on dust levels
  if (dustValue > dustThreshold) {
    activateDevices();
  } else {
    deactivateDevices();
  }

  // Read and display humidity
  int humidityValue = analogRead(HUMIDITY_SENSOR);
  humidityValue = map(humidityValue, 0, 1023, 0, 100);
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidityValue);
  lcd.print("%");

  // Control humidifier
  controlHumidity(humidityValue);

  // Read and handle CO sensor value
  int coValue = analogRead(CO_SENSOR);
  if (coValue > coThreshold) {
    triggerCOAlert();
  }

  delay(1000);
}

// Function to read dust sensor
int readDustSensor() {
  digitalWrite(DUST_LED, LOW);
  delayMicroseconds(280);
  int dustValue = analogRead(DUST_V0); // Corrected from DUST_SENSOR
  delayMicroseconds(40);
  digitalWrite(DUST_LED, HIGH);
  delayMicroseconds(9680);
  return dustValue; 
}

// Activate devices
void activateDevices() {
  digitalWrite(MOTOR, HIGH);
  digitalWrite(RELAY_HV, HIGH);
  digitalWrite(HUMIDIFIER, HIGH);
}

// Deactivate devices
void deactivateDevices() {
  digitalWrite(MOTOR, LOW);
  digitalWrite(RELAY_HV, LOW);
  digitalWrite(HUMIDIFIER, LOW);
}

// Control humidity based on thresholds
void controlHumidity(int humidity) {
  if (humidity < humidityLow) {
    digitalWrite(HUMIDIFIER, HIGH); 
  } else if (humidity > humidityHigh) {
    digitalWrite(HUMIDIFIER, LOW); 
  }
}

// Trigger alert for CO levels
void triggerCOAlert() {
  // Activate buzzer
  digitalWrite(BUZZER, HIGH);

  // Send alert via GSM
  gsm.println("ATD+91123456789;"); // Replace with an actual emergency number
  delay(5000); // Wait 5 seconds
  gsm.println("ATH"); // Hang up call

  // Deactivate buzzer
  digitalWrite(BUZZER, LOW);
}
