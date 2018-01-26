#include <NewPing.h>
#include <elapsedMillis.h>
#include <Wire.h>
#include <SoftwareSerial.h> // use the software uart 


#define BAT_PIN      A0 // Analog pin for the battery level measurement.
#define LDR_PIN      A1 // Analog pin for the light sensitive resistor.
#define VIBRATE_PIN  13 // Vibration feature for kpt. Ernst's mum.
#define TRIGGER_PIN0 12 // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN0    11 // Arduino pin tied to echo pin on the ultrasonic sensor.
#define TRIGGER_PIN1 5 // Arduino pin tied to trigger pin on the other ultrasonic sensor.
#define ECHO_PIN1    6 // Arduino pin tied to echo pin on the other ultrasonic sensor.
#define DIR_PIN      2 // The direction of the stepper motor.
#define STEP_PIN     3 // The amount of steps of the stepper motor.
#define BUTTON_PIN   4 // Digital pin for the S.O.S. button.
#define LED_PIN      10 // Digital pin for the LED strip.
#define BUZZ_PIN     9 // Digital pin for the speaker.

SoftwareSerial bluetooth(2, 4); // RX, TX

//variables
int MAX_DISTANCE = 250; // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
int buttonState = 0; 
int minLight = 40;
int sensorValue = 0;
int timer = 1;
int minVal = 265;
int maxVal = 402;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
float minVoltage = 7.0;
boolean stopLoop = false;
boolean vibrated = false;
boolean rotatedLeft = false;
boolean rotatedRight = false;
elapsedMillis timeElapsedUltrasonic;
elapsedMillis timeElapsedOrientation;
elapsedMillis timeElapsedBattery;
elapsedMillis timeElapsedLight;
elapsedMillis timeElapsedRotated;
NewPing sonar0(TRIGGER_PIN0, ECHO_PIN0, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonar1(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
unsigned long previousMillis = 0;                      // will store last time
const long interval = 500;                             // interval at which to delay
static uint32_t tmp;                                   // increment this


void setup() {
  Wire.begin();
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(VIBRATE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  Serial.begin(115200); //9600
}


void loop() {
  buttonState = digitalRead(BUTTON_PIN); // check if the button pin is high or low
  if (buttonState == HIGH) alarmMode();

  if (bluetooth.available()) { // check if anything in UART buffer
    if (bluetooth.read() == '1') turnLeft();
    if (bluetooth.read() == '2') turnRight();
  }

  if (rotatedLeft == true)
  {
    if (timeElapsedRotated >= 1000)
    {
      Serial.println("go right to starting position");
      rotateDeg(-22.5, .3); // turn 90 degrees to the right
      rotatedLeft = false;
      timeElapsedRotated = 0;
    }
  } else if (rotatedRight == true)
  {
    if (timeElapsedRotated >= 1000)
    {
      Serial.println("go left to starting position");
      rotateDeg(22.5, .3); // turn 90 degrees to the left
      rotatedRight = false;
      timeElapsedRotated = 0;
    }
  }

  if (timeElapsedUltrasonic >= 30) measureDistance(); // Wait 30ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  if (timeElapsedOrientation >= 500) measureOrientation();
  if (timeElapsedBattery >= 5000) measureBattery();
  if (timeElapsedLight >= 2000) measureLight();
}


void alarmMode() {
  while (stopLoop == false) {
    Serial.println("loop start");
    digitalWrite(BUZZ_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    delay(200);

    buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == HIGH)
    {
      stopLoop = true;
      Serial.println("first check");
    }
    digitalWrite(BUZZ_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    delay(200);

    buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == HIGH)
    {
      stopLoop = true;
      Serial.println("second check");
    }
  }
  stopLoop = false;
  Serial.println("loop end");
}


void measureDistance() {
  Serial.println(timeElapsedUltrasonic);

  Serial.print(sonar0.ping_cm(), DEC); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.print("\t");
  Serial.println(sonar1.ping_cm(), DEC); // Secondary ping.

  if (sonar0.ping_cm() <= 70 && sonar0.ping_cm() >= 1 || sonar1.ping_cm() <= 70 && sonar1.ping_cm() >= 1) {
    Serial.println("object detected!11");
    digitalWrite(VIBRATE_PIN, HIGH);
  } else {
    digitalWrite(VIBRATE_PIN, LOW);
  }
  timeElapsedUltrasonic = 0;
}


void measureOrientation() {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(0x68, 14, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  int yAng = map(AcY, minVal, maxVal, -90, 90);
  int zAng = map(AcZ, minVal, maxVal, -90, 90);
  double calc_X = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);  

  if (calc_X <= 195 && calc_X >= 150 )
  {
    Serial.println("shut down functions"); // als de gyroscope ondersteboven(onderzijde van chip ligt rechtop) is print die shutdown.
    Serial.print("DegX= ");  Serial.println(calc_X);
    delay(2000); //cane is upside-down and is now in sleep mode.    
  }      
  timeElapsedOrientation = 0;
}


void measureBattery() { // measure the battery voltage
  int sensorValue = analogRead(A0);
  float voltage = (sensorValue * (5.0 / 1024.0) * 10);
  Serial.println(voltage);
  if ( voltage < minVoltage) {
    if (vibrated == false) {
      for (int i = 0; i < 3; i++) {
        digitalWrite(VIBRATE_PIN, HIGH);
        delay(500);
        digitalWrite(VIBRATE_PIN, LOW);
        delay(500);
      }
      vibrated = true;
    }
  }
  timeElapsedBattery = 0;
}


void measureLight() { 
  sensorValue = analogRead(LDR_PIN); // measure the value of the LDR-resistor
  Serial.println(sensorValue);
  if (sensorValue <= minLight) {
    digitalWrite(LED_PIN, HIGH);
  }
  if (sensorValue > minLight) {
    digitalWrite(LED_PIN, LOW);
  }
  timeElapsedLight = 0;
}


void turnLeft() {
  Serial.println("go left");
  rotateDeg(22.5, .3);  // Rotate 90 degrees to the left
  rotatedLeft = true;
  timeElapsedRotated = 0;
}


void turnRight() {
  Serial.println("go right");
  rotateDeg(-22.5, .3); // Rotate 90 degrees to the right
  rotatedRight = true;
  timeElapsedRotated = 0;
}


void rotateDeg(float deg, float speed) { //rotate a specific number of degrees (negitive for reverse movement) //speed is any number from .01 -> 1 with 1 being fastest – Slower is stronger
  int dir = (deg > 0) ? HIGH : LOW;
  digitalWrite(DIR_PIN, dir);

  int steps = abs(deg) * (1 / 0.225);
  float usDelay = (1 / speed) * 70;

  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(usDelay);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(usDelay);
  }
}
