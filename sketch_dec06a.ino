int sensorPin = A0; // will be used for analog 0.

int sensorValue = 0; // value of output
int led = 3;
int buz = 4;

void setup() {
pinMode(led, OUTPUT);
pinMode(buz, OUTPUT);
Serial.begin(9600); //start serial Monitor

}

void loop() {

Serial.println("Automatic Light ON/OFF based on day and night"); 
sensorValue = analogRead(sensorPin);
Serial.println(sensorValue);

if (sensorValue < 35){
Serial.println("its Dark! LED light ON");
digitalWrite(led, HIGH);
digitalWrite(buz, LOW);
delay (2000);
}
digitalWrite(led, LOW);
if (sensorValue > 500) {
  Serial.println("Its Extra Bright! Switch off some light to conserve energy");
  digitalWrite(buz, HIGH);
  delay (100);
  digitalWrite(buz, LOW);
  delay(2000);
  }
  delay (sensorValue);
}


