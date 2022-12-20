// these two pins has the hardware interrupts as well.
#define Encoder_output_A 17  // pin2 of the Arduino
#define Encoder_output_B 5   // pin 3 of the Arduino

// led pins
#define redPin 15
#define orangePin 2
#define greenPin 16
#define bluePin 19

// make my function(ledManagement) faster.
#define red 1
#define orange 2
#define green 3
#define blue 4

#define noLight false
#define light true

#define Blink true
#define justON false

//Led variables
int color;
bool state;
bool isblinking;
int blinkingTime;

bool lightled;

int ledtoLight;

int ledstate = LOW;

// vibration var
bool vibrate;
int duration;
int force;

//Level variable
int whichLevel = 0;

//define seconds function
#define seconds() (millis() / 1000)

unsigned long LEDpreviousMillis = 0;
unsigned long VibratorpreviousMillis = 0;
unsigned long VibratorpreviousMillis2 = 0;

unsigned long isMovingpreviousMillis = 0;

unsigned long IsStoppedpreviousSeconds = 0;


//motor+
#define motorPin 32

//vibrator pin
#define vibratorPin 25
int vibrationSequence = 0;

bool motorFinishedReset = true;
int MotorTriggerCount = 0;

//Motor Encoder Value
int Count_pulses = 0;

//current Puzzle to solve
int Currentlevel = 0;

//pull string to begin.
bool gameJustStarted = true;

bool hintIsfinished = false;
bool delayIsPassed = false;
bool userskippedHint = false;
bool ismoving = false;

int LVL1Steps = 0;

bool stepIsConfirmed = false;
void setup() {

  //ESP.restart();
  Serial.begin(9600);  // activates the serial communication
  //motor encoder
  pinMode(Encoder_output_A, INPUT);  // sets the Encoder_output_A pin as the input
  pinMode(Encoder_output_B, INPUT);  // sets the Encoder_output_B pin as the input
  //leds
  pinMode(redPin, OUTPUT);
  pinMode(orangePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  //Esp32 pin to transistor to motor+
  pinMode(motorPin, OUTPUT);
  //vibratorPIN
  pinMode(vibratorPin, OUTPUT);
  //each time encoder is used -> ++Count_pulses or --Count_pulses
  attachInterrupt(digitalPinToInterrupt(Encoder_output_A), DC_Motor_Position, RISING);
}

void loop() {
  Serial.println(String(" isMoving ?  ") + isMoving(false) + String("  Is stopped for 5 sec:  ") + isStopforXsec(5000) + String("  Result:  ") + Count_pulses + String(" CurrentLevel: ") + whichLevel);

  //LevelManager(0);

  /*switch (whichLevel) {
    case 0:
      start();
      break;

    case 1:
      level();
      break;
  }*/

  LevelManager(whichLevel);

  //if (gameJustStarted) {
  //start();
  //}


  if (vibrate) {
    Serial.print("vibrating");
    vibrationHint(duration, force);
  }

  if (lightled) {
    ledManager(color, state, blinkingTime);
  }



  //Level
}



void punish(int level) {

  switch (level) {
    case 0:
      {
        if (Count_pulses <= -1) {
          digitalWrite(motorPin, HIGH);
        } else {

          digitalWrite(motorPin, LOW);
        }
      }
      break;
  }
}

void start() {
  //Serial.print("WEEEEESH");

  if (Count_pulses <= -200) {
    //Position passed. Activate -> motor has to reset
    motorFinishedReset = false;
  }
  //resetMotor, turn red light.
  if (!motorFinishedReset) {
    resetMotorTo(0, 100);
  }

  if (MotorTriggerCount == 1) {
    //pass to level
    Count_pulses = 0;
    whichLevel = 1;
    MotorTriggerCount = 0;
  }
}

void LevelManager(int Level) {
  switch (Level) {
    //Level 0 = tuto
    case 0:
      start();
      break;
    case 1:
      levelONE();
      break;
  }
}

void levelONE() {

  Serial.print(String("Vibration sequence:    ") + vibrationSequence);
  //check start function to help
  // LevelONE Hint.
  //if motor isnt moving show hint.
  //if motor is moving cancel hint and resetMotor.


  if (!hintIsfinished) {
    switch (vibrationSequence) {

      //needed to create a delay on the first vibration(case 0).
      //case -1 doesn't do anything appart to make the first vibration not going instant.
      case -1:
        setVibration(0, 0);
        break;

      case 0:
        led(blue, light, 0);
        setVibration(800, 80);
        break;
      case 1:
        led(blue, light, 0);
        setVibration(500, 80);
        break;
      case 2:
        led(blue, light, 0);
        setVibration(800, 80);
        break;
      case 3:
        vibrationSequence = 0;
        led(blue, noLight, 0);
        LVL1Steps = 1;
        Count_pulses = 0;
        hintIsfinished = true;
        break;
    }

    //if user use it before hint is finished restart.
    if (Count_pulses <= -50) {
      //Position passed. Activate -> motor has to reset
      led(blue, noLight, 0);
      setVibration(0, 0);
      LVL1Steps = 0;
      vibrationSequence = -1;
      motorFinishedReset = false;
      userskippedHint = true;
    }
  }

  /// idk if its necessary to check.
  if (userskippedHint) {
    hintIsfinished = true;
    MotorTriggerCount = 0;
    userskippedHint = false;
  }

  ///////// LEVEL 1

  //if fail return to begin
  if (!motorFinishedReset) {
    //resetMotor, turn red light.
    resetMotorTo(0, 100);
    //step 0 is lvl1 hasnt begun -> restart
    LVL1Steps = 0;
  }



  switch (LVL1Steps) {

    case 1:
      if (motorFinishedReset) {
        if (isMoving(false) && Count_pulses <= -10 && Count_pulses > -400) {
          //Position passed. Activate -> motor has to reset
          led(green, light, 0);
          //motorFinishedReset = false;
        }
        if (isMoving(false) && Count_pulses <= -301 && Count_pulses > -400) {
          //Position passed. Activate -> motor has to reset
          led(orange, light, 0);
          //motorFinishedReset = false;
        }
        //minimal distance to travel before accesing next step
        if (Count_pulses <= -250)
          if (!isMoving(false)) {
            if (!isStopforXsec(10000)) {
              //led(green, light, 0);
            } else {
              led(green, light, 5);
              Count_pulses = 0;
              LVL1Steps = 2;
            }
          }
        if (Count_pulses <= -401) {
          motorFinishedReset = false;
        }
        //led(green, light, 2);
      }
      break;

    case 2:

      if (motorFinishedReset) {
        if (isMoving(false) && Count_pulses <= -10 && Count_pulses > -200) {
          //Position passed. Activate -> motor has to reset
          led(green, light, 0);
          //motorFinishedReset = false;
        }
        if (isMoving(false) && Count_pulses <= -150 && Count_pulses > -200) {
          //Position passed. Activate -> motor has to reset
          led(orange, light, 0);
          //motorFinishedReset = false;
        }
        //minimal distance to travel before accesing next step
        if (Count_pulses <= -120)
          if (!isMoving(false)) {
            if (!isStopforXsec(10000)) {
              //led(green, light, 0);
            } else {
              led(green, light, 5);
              Count_pulses = 0;
              LVL1Steps = 3;
            }
          }
        if (Count_pulses <= -201) {
          motorFinishedReset = false;
        }
        //led(green, light, 2);
      }



      break;

    case 3:

      if (motorFinishedReset) {
        if (isMoving(false) && Count_pulses <= -10 && Count_pulses > -400) {
          //Position passed. Activate -> motor has to reset
          led(green, light, 0);
          //motorFinishedReset = false;
        }
        if (isMoving(false) && Count_pulses <= -301 && Count_pulses > -400) {
          //Position passed. Activate -> motor has to reset
          led(orange, light, 0);
          //motorFinishedReset = false;
        }
        //minimal distance to travel before accesing next step
        if (Count_pulses <= -250)
          if (!isMoving(false)) {
            if (!isStopforXsec(10000)) {
              //led(green, light, 0);
            } else {
              led(green, light, 5);
              Count_pulses = 0;
              ///LVL 1 is FINISHED
              LVL1Steps = 4;
            }
          }
        if (Count_pulses <= -401) {
          motorFinishedReset = false;
        }
        //led(green, light, 2);
      }

      break;
  }




  /*

  

  
   if (Count_pulses <= -500) {
    //Position passed. Activate -> motor has to reset
    motorFinishedReset = false;
  }
  //resetMotor, turn red light.
  if (!motorFinishedReset) {
    resetMotorTo(0, 100);
  }

  if (MotorTriggerCount == 1) {
    //pass to level
    whichLevel = 1;
    MotorTriggerCount = 0;
  }
  */

  //if motorwasreseted: hintshould start again with blue light
  // !hintisnotfinished
  // Vibration sequence = 0;
  // don't forget to reset values
}

void led(int ledcolor, bool On_Off, int timeBlinking) {

  color = ledcolor;
  state = On_Off;
  blinkingTime = timeBlinking;

  lightled = true;
}

int ledshutdown = 0;
void ledManager(int color, bool On_Off, int blinkingTime) {

  //blinking var
  const long interval = 100;
  unsigned long currentMillis = millis();
  unsigned long second = seconds();




  //Serial.print(String("   Interval value:  ") + interval + String("    "));

  //if there is a blinking time go blink.
  //if not just let it ON
  if (blinkingTime != 0) {
    ///Stop blinking and turn off
    if (ledshutdown >= 10) {
      // Serial.print("   Should TURN OFF... ");
      ledstate = LOW;
      digitalWrite(redPin, LOW);
      digitalWrite(orangePin, LOW);
      digitalWrite(greenPin, LOW);
      digitalWrite(bluePin, LOW);
      ledshutdown = 0;

      color = NULL;
      lightled = false;
    }

    if (currentMillis - LEDpreviousMillis >= interval) {
      LEDpreviousMillis = currentMillis;

      ledshutdown++;

      switch (color) {
        case 1:
          ledtoLight = redPin;
          if (ledstate == LOW) {
            ledstate = HIGH;
          } else {
            ledstate = LOW;
          }
          break;

        case 2:
          ledtoLight = orangePin;
          if (ledstate == LOW) {
            ledstate = HIGH;
          } else {
            ledstate = LOW;
          }
          break;

        case 3:
          ledtoLight = greenPin;
          if (ledstate == LOW) {
            ledstate = HIGH;
          } else {
            ledstate = LOW;
          }
          break;

        case 4:
          ledtoLight = bluePin;
          if (ledstate == LOW) {
            ledstate = HIGH;
          } else {
            ledstate = LOW;
          }
          break;

        default:

          break;
      }
    }
    //if not blink. Just turn it on or off
  } else {

    //just turn ON/OFF
    switch (color) {
      case 1:
        Serial.print(String("  Red  ") + On_Off);

        digitalWrite(orangePin, LOW);
        digitalWrite(greenPin, LOW);
        digitalWrite(bluePin, LOW);
        ledtoLight = redPin;
        ledstate = On_Off;
        break;

      case 2:
        Serial.print(String("  Orange  ") + On_Off);
        digitalWrite(redPin, LOW);


        digitalWrite(bluePin, LOW);
        ledtoLight = orangePin;
        ledstate = On_Off;
        break;

      case 3:
        Serial.print(String("  Green  ") + On_Off);
        digitalWrite(redPin, LOW);


        digitalWrite(bluePin, LOW);
        ledtoLight = greenPin;
        ledstate = On_Off;
        break;

      case 4:
        Serial.print(String("  BLUE  ") + On_Off);

        digitalWrite(redPin, LOW);
        digitalWrite(orangePin, LOW);
        digitalWrite(greenPin, LOW);

        ledtoLight = bluePin;
        ledstate = On_Off;
        break;
    }
  }
  //blink desired desired led
  digitalWrite(ledtoLight, ledstate);
  if (!On_Off) {
    lightled = false;
  }
}

void setVibration(int vibrationDuration, int vibrationForce) {

  duration = vibrationDuration;
  force = vibrationForce;
  vibrate = true;
}

void vibrationHint(int vibrationDuration, int vibrationForce) {
  //int interval = seconds();
  const long intervalue = vibrationDuration;
  const long delayBeforeVibrating = 1000;
  int power;
  unsigned long currentMillis = millis();



  if (!delayIsPassed) {

    power = 0;
    if (currentMillis - VibratorpreviousMillis >= delayBeforeVibrating) {

      VibratorpreviousMillis = currentMillis;
      //half second has passed
      Serial.print("PAAAAASSSEDDD");
      delayIsPassed = true;
    }
  } else {
    if (currentMillis - VibratorpreviousMillis >= intervalue) {
      VibratorpreviousMillis = currentMillis;

      power = 0;
      vibrationSequence++;

      vibrate = false;
      delayIsPassed = false;

    } else {
      power = vibrationForce;
    }
  }

  /* if (interval % vibrationDuration == 0) {
    power = 0;
    vibrationSequence++;
    vibrate = false;
  }*/

  if (vibrationDuration == 0) {
    power = 0;
  }
  Serial.print(String(" VIBRATING: ") + power + String("delayIsPassed:  ") + delayIsPassed);
  analogWrite(vibratorPin, power);
}

void resetMotorTo(int position, int speed) {

  if (!inRange(position)) {
    led(red, light, 0);
    int speedMap = map(speed, 0, 100, 0, 255);
    analogWrite(motorPin, speedMap);
    Serial.print("   MotorShouldReset");
  } else {
    Serial.print("   Motor should not move");
    led(red, light, 5);
    analogWrite(motorPin, 0);
    MotorTriggerCount++;
    hintIsfinished = false;
    motorFinishedReset = true;
  }
}

bool inRange(int position) {
  return ((Count_pulses >= position - 100 && Count_pulses <= position + 100));
}

void DC_Motor_Position() {
  int b = digitalRead(Encoder_output_B);
  if (b > 0) {

    Count_pulses++;
  } else {
    isMoving(true);
    Count_pulses--;
  }
}

bool isStopped;
bool isStopforXsec(int millisec) {

  unsigned long currentMillis = millis();

  if (!isMoving(false)) {
    if (currentMillis - IsStoppedpreviousSeconds >= 10000) {
      IsStoppedpreviousSeconds = currentMillis;
      isStopped = true;
    }
  } else {
    isStopped = false;
  }

  return isStopped;
}

bool isMoving(bool calledfromMotor) {

  unsigned long currentMillis = millis();

  if (calledfromMotor) {
    isMovingpreviousMillis = currentMillis;
    ismoving = true;
  } else {

    if (currentMillis - isMovingpreviousMillis >= 1000) {
      ismoving = false;
    }
  }
  return ismoving;
}