/*
 * Created: March 2016
 * By: Ab Kurk
 * Version:1
 * Description:
 * File Name:Timer_demo2.ino
 * This is part of a tutoral to show the difference between using a timer library elapsedMillis.h v.s. the delay() function. 
 * With a timer you won't halt your program and your buttons and input sensors will respond corectly. This demo uses the timer library.
 *Link to the Tutorial:http://www.thearduinomakerman.info/blog/2016/4/8/tutorial-delay-vs-timer
 * 
 */


#include <elapsedMillis.h>

elapsedMillis timeElapsed; //declare global if you don't want it reset every time loop runs

//Delcaring the button and LED Pins
#define button1Pin 9
#define led1Pin 13
#define led2Pin 12
#define led3Pin 11
#define led4Pin 10
int onoff=0;//0 lights are off 1 light are on


// the setup function runs once when you press reset or power the board
void setup() {
 
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  pinMode(button1Pin, INPUT);
  Serial.begin(115200);
  
}

// the loop function runs over and over again forever
void loop() {
  /*
   * If the button is pressed the light turn on. If the button is released
   * the lights will first turn off and then sequentially turn on and then off. 
   */
  if (digitalRead(button1Pin) == LOW) {
    // turn LEDS on:
    Serial.println("on");
    digitalWrite(led1Pin, HIGH); 
    digitalWrite(led2Pin, HIGH);
    digitalWrite(led3Pin, HIGH);  
    digitalWrite(led4Pin, HIGH);
    onoff=1; 
  } else if(digitalRead(button1Pin)==HIGH) {
    // turn LEDs off:
    if(onoff==1){
      digitalWrite(led1Pin, LOW);
      digitalWrite(led2Pin, LOW);
      digitalWrite(led3Pin, LOW);
      digitalWrite(led4Pin, LOW);
      onoff=0; 
      timeElapsed=0;//reset timer to 0
    }
    
    //walkning LED
    if(timeElapsed<1000){//if less than a second has elaped turn first led on
        digitalWrite(led1Pin, HIGH);   // turn the LED on (HIGH is the voltage level)
      }
    if(timeElapsed>1000 && timeElapsed<2000){ // if more than a second has passed and less than 2 turn led 1 off and turn led 2 on
        digitalWrite(led1Pin, LOW);    // turn the LED off by making the voltage LOW
        digitalWrite(led2Pin, HIGH);   // turn the LED on (HIGH is the voltage level)
      }
    if(timeElapsed>2000 && timeElapsed<3000){  // if more than a 2 seconds have passed and less than 3 turn led 2 off and turn led 3 on
        digitalWrite(led2Pin, LOW);    // turn the LED off by making the voltage LOW
        digitalWrite(led3Pin, HIGH);   // turn the LED on (HIGH is the voltage level)
      }
    if(timeElapsed>3000 && timeElapsed<4000){  // if more than a 3 seconds have passed and less than 4 turn led 3 off and turn led 4 on
        digitalWrite(led3Pin, LOW);    // turn the LED off by making the voltage LOW
        digitalWrite(led4Pin, HIGH);   // turn the LED on (HIGH is the voltage level)
      }
     if(timeElapsed>4000){            // // if more than a 4 seconds have passed turn led 4 off and reset the timer to 0
        digitalWrite(led4Pin, LOW);    // turn the LED off by making the voltage LOW
        timeElapsed=0;  //Set timer to 0
     }
      
  }
}
