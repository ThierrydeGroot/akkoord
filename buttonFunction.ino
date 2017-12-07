/**
  To keep a physical interface as simple as possible, this sketch demonstrates generating four output events from a single push-button.
  1) Click:  rapid press and release
  2) Double-Click:  two clicks in quick succession
  3) Press and Hold:  holding the button down
  4) Long Press and Hold:  holding the button for a long time
*/

#define buttonPin 19        // analog input pin to use as a digital input
#define ledPin1 17          // digital output pin for LED 1
#define ledPin2 16          // digital output pin for LED 2
#define ledPin3 15          // digital output pin for LED 3
#define ledPin4 14          // digital output pin for LED 4

// LED variables
boolean ledVal1 = false;    // state of LED 1
boolean ledVal2 = false;    // state of LED 2
boolean ledVal3 = false;    // state of LED 3
boolean ledVal4 = false;    // state of LED 4

//=================================================

void setup() {
  // Set button input pin
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH );
  // Set LED output pins
  pinMode(ledPin1, OUTPUT);
  digitalWrite(ledPin1, ledVal1);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin2, ledVal2);
  pinMode(ledPin3, OUTPUT);
  digitalWrite(ledPin3, ledVal3);
  pinMode(ledPin4, OUTPUT);
  digitalWrite(ledPin4, ledVal4);
}

void loop() {
  // Get button event and act accordingly
  int b = checkButton();
  if (b == 1) clickEvent();
  if (b == 2) doubleClickEvent();
  if (b == 3) holdEvent();
  if (b == 4) longHoldEvent();
}

//=================================================
// Events to trigger

void clickEvent() {
  ledVal1 = !ledVal1;
  digitalWrite(ledPin1, ledVal1);
}
void doubleClickEvent() {
  ledVal2 = !ledVal2;
  digitalWrite(ledPin2, ledVal2);
}
void holdEvent() {
  ledVal3 = !ledVal3;
  digitalWrite(ledPin3, ledVal3);
}
void longHoldEvent() {
  ledVal4 = !ledVal4;
  digitalWrite(ledPin4, ledVal4);
}

//=================================================
//  MULTI-CLICK:  One Button, Multiple Events

// Button timing variables
int debounce = 20;          // ms debounce period to prevent flickering when pressing or releasing the button
int DCgap = 250;            // max ms between clicks for a double click event
int holdTime = 1000;        // ms hold period: how long to wait for press+hold event
int longHoldTime = 3000;    // ms long hold period: how long to wait for press+hold event

// Button variables
boolean buttonVal = HIGH;   // value read from button
boolean buttonLast = HIGH;  // buffered value of the button's previous state
boolean DCwaiting = false;  // whether we're waiting for a double click (down)
boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
boolean singleOK = true;    // whether it's OK to do a single click
long downTime = -1;         // time the button was pressed down
long upTime = -1;           // time the button was released
boolean ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
boolean waitForUp = false;        // when held, whether to wait for the up event
boolean holdEventPast = false;    // whether or not the hold event happened already
boolean longHoldEventPast = false;// whether or not the long hold event happened already

int checkButton() {
  int event = 0;
  buttonVal = digitalRead(buttonPin);
  // Button pressed down
  if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce)
  {
    downTime = millis();
    ignoreUp = false;
    waitForUp = false;
    singleOK = true;
    holdEventPast = false;
    longHoldEventPast = false;
    if ((millis() - upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
    else  DConUp = false;
    DCwaiting = false;
  }
  // Button released
  else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce)
  {
    if (not ignoreUp)
    {
      upTime = millis();
      if (DConUp == false) DCwaiting = true;
      else
      {
        event = 2;
        DConUp = false;
        DCwaiting = false;
        singleOK = false;
      }
    }
  }
  // Test for normal click event: DCgap expired
  if ( buttonVal == HIGH && (millis() - upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
  {
    event = 1;
    DCwaiting = false;
  }
  // Test for hold
  if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
    // Trigger "normal" hold
    if (not holdEventPast)
    {
      event = 3;
      waitForUp = true;
      ignoreUp = true;
      DConUp = false;
      DCwaiting = false;
      //downTime = millis();
      holdEventPast = true;
    }
    // Trigger "long" hold
    if ((millis() - downTime) >= longHoldTime)
    {
      if (not longHoldEventPast)
      {
        event = 4;
        longHoldEventPast = true;
      }
    }
  }
  buttonLast = buttonVal;
  return event;
}
