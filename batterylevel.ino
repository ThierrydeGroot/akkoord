#define BAT_PIN A0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int sensorValue = analogRead(A0);
  float voltage = (sensorValue * (5.0 / 1024.0) * 10);
  Serial.println(voltage);
  delay(1000);
}
