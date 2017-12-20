#include <NewPing.h>

#define BAT_PIN      A0
#define LDR_PIN      A1
#define VIBRATE_PIN  13
#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define BUTTON_PIN    2
#define MAX_DISTANCE 250 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define LED_PIN      10
#define BUZZ_PIN     9

//variables
int check = 0;
int sensorValue = 0; // variable to store the value coming from the sensor
int buttonState = 0;
int minLight = 35;
int pingDelay = 50;
float minVoltage = 7.0;
boolean stopLoop = false;
boolean vibrated = false;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(VIBRATE_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  Serial.begin(9600); //115200
}

void loop() {
  delay(pingDelay); // Wait 29ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  buttonState = digitalRead(BUTTON_PIN);
  Serial.println(sonar.ping_cm()); // Send ping, get distance in cm and print result (0 = outside set distance range)

  if (sonar.ping_cm() <= 70 && sonar.ping_cm() >= 1) {
    digitalWrite(VIBRATE_PIN, HIGH);
  } else {
    digitalWrite(VIBRATE_PIN, LOW);
  }

  //check orientation here every 1000 miliseconds
  if (check == 9) measureLight();
  if (check >= 19) measureBattery();
  if (buttonState == HIGH) alarmMode();

  check = check + 1;
  Serial.println(check);
  if ( check >= 20) {
    check = 0;
  }
}

void measureBattery() { //measure the battery voltage
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
}

void measureLight() { //measure the value of the LDR-resistor
  sensorValue = analogRead(LDR_PIN);
  Serial.println(sensorValue);
  if (sensorValue <= minLight) {
    digitalWrite(LED_PIN, HIGH);
  }
  if (sensorValue > 35) {
    digitalWrite(LED_PIN, LOW);
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

