#include <NewPing.h>
#include <elapsedMillis.h>
#include <Wire.h>


#define BAT_PIN      A0
#define LDR_PIN      A1
#define bluetoothReceivePin 0 // The input pin for receiving bluetouth messages.
#define bluetoothTransmitPin 1 // The output pin for transmitting bluetouth messages.
#define VIBRATE_PIN  13 // Vibration feature for kpt. Ernst's mum.
#define TRIGGER_PIN0 12 // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN0    11 // Arduino pin tied to echo pin on the ultrasonic sensor.
#define TRIGGER_PIN1 5 // Arduino pin tied to trigger pin on the other ultrasonic sensor.
#define ECHO_PIN1    6 // Arduino pin tied to echo pin on the other ultrasonic sensor.
#define DIR_PIN      2 // The direction of the stepper motor.
#define STEP_PIN     3 // The amount of steps of the stepper motor.
#define BUTTON_PIN   4
#define MAX_DISTANCE 250 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define LED_PIN      10
#define BUZZ_PIN     9

//variables

int incomingBluetoothCommand = 0;
int commandInt = 0;
String commandString = "";
String commandArgument = "";


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
elapsedMillis timeElapsedUltrasonic;
elapsedMillis timeElapsedOrientation;
elapsedMillis timeElapsedBattery;
elapsedMillis timeElapsedLight;
NewPing sonar0(TRIGGER_PIN0, ECHO_PIN0, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonar1(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


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
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == HIGH) alarmMode();

  if (timeElapsedUltrasonic >= 30) measureDistance(); // Wait 30ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  //if (timeElapsedOrientation >= 1000) measureOrientation();
  if (timeElapsedBattery >= 5000) measureBattery();
  if (timeElapsedLight >= 5000) measureLight();
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

  int xAng = map(AcX, minVal, maxVal, -90, 90);
  int yAng = map(AcY, minVal, maxVal, -90, 90);
  int zAng = map(AcZ, minVal, maxVal, -90, 90);

  double calc_X = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
  double calc_Y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  double calc_Z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

  if (calc_X <= 195 && calc_X >= 150 )
  {
    //als de gyroscope ondersteboven(onderzijde van chip ligt rechtop) is print die shutdown.
    Serial.print("shut down functions");
    Serial.print("DegX= ");  Serial.println(calc_X);
  } else
  {
    Serial.print("DegX= ");  Serial.println(calc_X);
  }
  if (calc_X >= 357 && calc_X <= 360)
  {
    if (timer == 5)
    {
      Serial.print("shut down");
      timer = 1;
    } else
    {
      delay(1000);
      timer++;
      Serial.print(timer);
    }
  }
  if (calc_X <= 355)
    timer = 1;
  Serial.println("-----------------------------------------");
  delay(450);

  timeElapsedOrientation = 0;
}


void measureBattery() { //measure the battery voltage
  int sensorValue = analogRead(A0);
  float voltage = (sensorValue * (5.0 / 1024.0) * 10);
  //Serial.println(voltage);
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
  elapsedMillis timeElapsedBattery;
}


void measureLight() { //measure the value of the LDR-resistor
  sensorValue = analogRead(LDR_PIN);
  //Serial.println(sensorValue);
  if (sensorValue <= minLight) {
    digitalWrite(LED_PIN, HIGH);
  }
  if (sensorValue > 35) {
    digitalWrite(LED_PIN, LOW);
  }
  elapsedMillis timeElapsedLight;
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
