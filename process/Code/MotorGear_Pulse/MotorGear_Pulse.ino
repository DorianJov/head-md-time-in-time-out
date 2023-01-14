// these two pins has the hardware interrupts as well.
#define Encoder_output_A 17  // pin2 of the Arduino
#define Encoder_output_B 5   // pin 3 of the Arduino

//motor+
#define motorPin 32

//vibrator pin
#define vibratorPin 25

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
int blinkingTime;
bool lightled;
int ledtoLight;
int ledstate = LOW;

// vibration var
bool vibrate;
int duration;
int force;
int vibrationSequence = 0;

//define seconds function
unsigned long LEDpreviousMillis = 0;
unsigned long VibratorpreviousMillis = 0;
unsigned long isMovingpreviousMillis = 0;
unsigned long IsStoppedpreviousSeconds = 0;

//Motor string variable
bool motorFinishedReset = true;
int MotorTriggerCount = 0;

//Motor Encoder Value
int Count_pulses = 0;
int level_pulses = 0;

//Vibration motor variables
bool hintIsfinished = false;
bool delayIsPassed = false;
bool userskippedHint = false;

//Level variable
short whichLevel = 0;

//Levels steps
short LVL1Steps = 0;
short LVL2Steps = 0;
short LVL3Steps = 0;

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

  LevelManager(whichLevel);

  if (vibrate) {
    Serial.print("vibrating");
    vibrationHint(duration, force);
  }

  if (lightled) {
    ledManager(color, state, blinkingTime);
  }

  Serial.println(String(" isMoving ?  ") + isMoving(false) + String("  Is stopped for 5 sec:  ") + isStopforXsec(4500) + String("  Count_Pulses:  ") + Count_pulses + String("  Level_Pulses:  ") + level_pulses + String(" CurrentLevel: ") + whichLevel);
}

void LevelManager(int Level) {
  switch (Level) {
    //Level 0 = Pull string to start
    case 0:
      start();
      break;
      //Level 1 = "Tuto" with orange led -> easier to understand the principle
    case 1:
      levelONE();
      break;
    //Level 2 = Starting chill but no orange led indicator
    case 2:
      levelTWO();
      break;
    //Level 3 = go fast with no orange led indicator
    case 3:
      levelTHREE();
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
    level_pulses = 0;
    whichLevel = 1;
    MotorTriggerCount = 0;
  }
}

void levelONE() {
  //Serial.print(String("Vibration sequence:    ") + vibrationSequence);
  if (!hintIsfinished) {
    switch (vibrationSequence) {
      //needed to create a delay for the first vibration(case 0).
      //case -1 doesn't do anything appart to make the first vibration(case 0) not going instant.
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
        vibrationSequence = -1;
        led(blue, noLight, 0);
        LVL1Steps = 1;
        Count_pulses = 0;
        level_pulses = 0;
        hintIsfinished = true;
        break;
    }
    //if user pull string before hint is finished -> restart and reset values.
    if (Count_pulses <= -10) {
      led(blue, noLight, 0);
      setVibration(0, 0);
      LVL1Steps = 0;
      vibrationSequence = -1;
      motorFinishedReset = false;
      MotorTriggerCount = 0;
      hintIsfinished = true;
    }
  }
  ///////// LEVEL 1 STEPS
  if (motorFinishedReset) {
    switch (LVL1Steps) {
      case 1:
        if (isMoving(false) && level_pulses <= -10 && level_pulses > -400) {
          led(green, light, 0);
        }
        if (isMoving(false) && level_pulses <= -301 && level_pulses > -400) {
          led(orange, light, 0);
        }
        ////Step succes // minimal distance to travel && not moving && is_Stopped for 5sec
        if (level_pulses <= -250 && !isMoving(false) && isStopforXsec(4500)) {
          led(green, light, 5);
          level_pulses = 0;
          LVL1Steps = 2;
        }

        if (level_pulses <= -401) {
          motorFinishedReset = false;
        }

        break;

      case 2:

        if (isMoving(false) && level_pulses <= -10 && level_pulses > -200) {
          led(green, light, 0);
        }
        if (isMoving(false) && level_pulses <= -150 && level_pulses > -200) {
          led(orange, light, 0);
        }
        //Step succes // minimal distance to travel && not moving && is_Stopped for 5sec
        if (level_pulses <= -120 && !isMoving(false) && isStopforXsec(4500)) {
          led(green, light, 5);

          level_pulses = 0;
          LVL1Steps = 3;
        }
        if (level_pulses <= -201) {
          motorFinishedReset = false;
        }
        break;

      case 3:
        if (isMoving(false) && level_pulses <= -10 && level_pulses > -400) {
          led(green, light, 0);
        }
        if (isMoving(false) && level_pulses <= -301 && level_pulses > -400) {
          led(orange, light, 0);
        }
        //Step succes // minimal distance to travel && not moving && isStopped for 5sec
        if (level_pulses <= -250 && !isMoving(false) && isStopforXsec(4500)) {
          //All steps from LVL 1 are done: Blink green light. Reset Count_Pulses to make lvl design easier. Go to level 2
          led(green, light, 5);
          Count_pulses = 0;
          level_pulses = 0;
          hintIsfinished = false;
          whichLevel = 2;
        }
        //Step limit, if bigger -> failed
        if (level_pulses <= -401) {
          motorFinishedReset = false;
        }
        break;
    }
  } else {
    //FAILED -> resetMotor, turn red light.
    resetMotorTo(0, 100);
    //set current step 0 -> restart
    LVL1Steps = 0;
  }
}


void levelTWO() {

  if (!hintIsfinished) {
    switch (vibrationSequence) {
      //needed to create a delay for the first vibration(case 0).
      //case -1 doesn't do anything appart to make the first vibration(case 0) not going instant.
      case -1:
        setVibration(0, 0);
        break;
      case 0:
        led(blue, light, 0);
        setVibration(800, 80);
        break;

      case 1:
        vibrationSequence = -1;
        led(blue, noLight, 0);
        LVL2Steps = 1;
        Count_pulses = 0;
        level_pulses = 0;
        hintIsfinished = true;
        break;
    }
    //if user pull string before hint is finished -> restart and reset values.
    if (Count_pulses <= -10) {
      led(blue, noLight, 0);
      setVibration(0, 0);
      LVL2Steps = 1;
      vibrationSequence = -1;
      motorFinishedReset = false;
      MotorTriggerCount = 0;
      hintIsfinished = true;
    }
  }

  if (motorFinishedReset) {
    switch (LVL2Steps) {
      case 1:
        if (isMoving(false) && level_pulses <= -10 && level_pulses > -300) {
          led(green, light, 0);
        }
        /* if (isMoving(false) && Count_pulses <= -301 && Count_pulses > -400) {
          led(orange, light, 0);
        }*/
        ////Step succes // minimal distance to travel && not moving && is_Stopped for 4sec
        if (level_pulses <= -270 && !isMoving(false) && isStopforXsec(3500)) {
          led(green, light, 5);
          Count_pulses = 0;
          level_pulses = 0;
          hintIsfinished = false;
          whichLevel = 3;
        }
        if (level_pulses <= -401) {
          motorFinishedReset = false;
        }
        break;
    }
  } else {
    //FAILED -> resetMotor, turn red light.
    resetMotorTo(0, 100);
    //set current step 0 -> restart
    LVL2Steps = 0;
  }
}

void levelTHREE() {

  if (!hintIsfinished) {
    switch (vibrationSequence) {
      //needed to create a delay for the first vibration(case 0).
      //case -1 doesn't do anything appart to make the first vibration(case 0) not going instant.
      case -1:
        setVibration(0, 0);
        break;
      case 0:
        led(blue, light, 0);
        setVibration(200, 255);
        break;

      case 1:
        led(blue, light, 0);
        setVibration(200, 255);
        break;
      case 2:
        led(blue, light, 0);
        setVibration(200, 255);
        break;

      case 3:
        vibrationSequence = -1;
        led(blue, noLight, 0);
        LVL3Steps = 1;
        Count_pulses = 0;
        level_pulses = 0;
        hintIsfinished = true;
        break;
    }
    //if user pull string before hint is finished -> restart and reset values.
    if (Count_pulses <= -10) {
      led(blue, noLight, 0);
      setVibration(0, 0);
      LVL3Steps = 1;
      vibrationSequence = -1;
      motorFinishedReset = false;
      MotorTriggerCount = 0;
      hintIsfinished = true;
    }
  }

  if (motorFinishedReset) {
    switch (LVL3Steps) {
      case 1:
        if (isMoving(false) && level_pulses <= -10 && level_pulses > -70) {
          led(green, light, 0);
        }
        /* if (isMoving(false) && Count_pulses <= -301 && Count_pulses > -400) {
          led(orange, light, 0);
        }*/
        ////Step succes // minimal distance to travel && not moving && is_Stopped for 4sec
        if (level_pulses <= -60 && !isMoving(false) && isStopforXsec(2500)) {
          led(green, light, 5);
          Count_pulses = 0;
          level_pulses = 0;
         //hintIsfinished = false;
          LVL3Steps = 2;
        }
        if (level_pulses <= -71) {
          motorFinishedReset = false;
        }
        break;
    }
  } else {
    //FAILED -> resetMotor, turn red light.
    resetMotorTo(0, 100);
    //set current step 0 -> restart
    LVL3Steps = 0;
  }
}

void led(int ledcolor, bool On_Off, int timeBlinking) {
  color = ledcolor;
  state = On_Off;
  blinkingTime = timeBlinking;
  lightled = true;
}

int ledshutdown = 0;
void ledManager(int color, bool On_Off, int blinkingTime) {
  const long interval = 100;
  unsigned long currentMillis = millis();
  //if there is a blinking time go blink.
  //if not just let it ON
  if (blinkingTime != 0) {
    //Stop blinking and turn off
    if (ledshutdown >= 10) {
      //Serial.print("   Should TURN OFF... ");
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
  //blink desired led
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
    level_pulses = Count_pulses;
    motorFinishedReset = true;
  }
}

bool inRange(int position) {
  return ((Count_pulses >= position && Count_pulses <= position + 500));
}

void DC_Motor_Position() {
  int b = digitalRead(Encoder_output_B);
  if (b > 0) {
    Count_pulses++;
    level_pulses++;
  } else {
    isMoving(true);
    Count_pulses--;
    level_pulses--;
  }
}

bool isStopforXsec(int millisec) {
  unsigned long currentMillis = millis();
  const int interval = millisec - 1000;
  if (!isMoving(false)) {
    if (currentMillis - IsStoppedpreviousSeconds >= interval) {
      return true;
    } else {
      return false;
    }
  } else {
    IsStoppedpreviousSeconds = currentMillis;
    return false;
  }
}

bool isMoving(bool calledfromMotor) {
  unsigned long currentMillis = millis();
  if (calledfromMotor) {
    isMovingpreviousMillis = currentMillis;
    return true;
  } else if (currentMillis - isMovingpreviousMillis >= 1000) {
    return false;
  } else {
    return true;
  }
}