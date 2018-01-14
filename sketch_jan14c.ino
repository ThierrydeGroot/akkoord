#include <NewPing.h>

#define TRIGGER_PIN0    12 // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN0       11 // Arduino pin tied to echo pin on the ultrasonic sensor.
#define TRIGGER_PIN1    5 // Arduino pin tied to trigger pin on the other ultrasonic sensor.
#define ECHO_PIN1       6 // Arduino pin tied to echo pin on the other ultrasonic sensor.
#define MAX_DISTANCE    250 

int sensorValue = 0; // variable to store the value coming from the sensor
NewPing sonar(TRIGGER_PIN0, ECHO_PIN0, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonar1(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCE);

void setup() {
  Serial.begin(115200); //9600
}

void loop() {
  delay(50); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.

  Serial.print(sonar.ping_cm(), DEC); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.print("\t");
  Serial.println(sonar1.ping_cm(), DEC); // Secondary ping.

} 
