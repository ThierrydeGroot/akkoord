#include <NewPing.h>
#include <elapsedMillis.h>
#include <Wire.h>

#define BAT_PIN      A0
#define LDR_PIN      A1
#define VIBRATE_PIN  13
#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define BUTTON_PIN    4
#define MAX_DISTANCE 250 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define LED_PIN      10
#define BUZZ_PIN     9

//variables
elapsedMillis timeElapsedUltrasonic;
elapsedMillis timeElapsedOrientation;
int buttonState = 0;
int minLight = 40;
int sensorValue = 0;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
float minVoltage = 7.0;
boolean stopLoop = false;
boolean vibrated = false;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


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
  Serial.begin(9600); //115200
}

void loop() {
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == HIGH) alarmMode();

  if (timeElapsedUltrasonic > 30) {  // Wait 30ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
    measureDistance();
  }







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

  if (sonar.ping_cm() <= 70 && sonar.ping_cm() >= 1) {
    Serial.println("object detected!11");
    digitalWrite(VIBRATE_PIN, HIGH);
  } else {
    digitalWrite(VIBRATE_PIN, LOW);
  }

  timeElapsedUltrasonic = 0;
}


