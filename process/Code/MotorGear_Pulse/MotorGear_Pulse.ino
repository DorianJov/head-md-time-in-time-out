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
  Serial.println(String("Result:  ") + Count_pulses + String(" CurrentLevel: ") + whichLevel);

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


  //check start function to help
  // LevelONE Hint.
  //if motor isnt moving show hint.
  //if motor is moving cancel hint and resetMotor.
  if (!hintIsfinished) {
    led(blue, light, 0);
    switch (vibrationSequence) {
      case 0:
        setVibration(250, 100);
        break;
      case 1:
        setVibration(1000, 255);
        break;
        case 2;
        setVibration(250, 100);
        break;
        case 3:
        vibrationSequence = 0;
        led(blue, noLight, 0);
        hintIsfinished = true;
        break;
    }
  }

  ///////// LEVEL RULES
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
}
else {

  //just turn ON/OFF
  switch (color) {
    case 1:
      ledtoLight = redPin;
      ledstate = On_Off;
      break;

    case 2:
      ledtoLight = orangePin;
      ledstate = On_Off;
      break;

    case 3:
      ledtoLight = greenPin;
      ledstate = On_Off;
      break;

    case 4:
      Serial.print(String("  BLUE LIGHT  ") + On_Off);
      ledtoLight = bluePin;
      ledstate = On_Off;
      break;
  }
}
//blink desired desired led
digitalWrite(ledtoLight, ledstate);
}

void setVibration(int vibrationDuration, int vibrationForce) {

  duration = vibrationDuration;
  force = vibrationForce;
  vibrate = true;
}

void vibrationHint(int vibrationDuration, int vibrationForce) {
  int interval = seconds();
  const long intervalue = vibrationDuration;
  int power;
  unsigned long currentMillis = millis();

  if (!delayIsPassed) {
    if (currentMillis - VibratorpreviousMillis >= 500) {
      VibratorpreviousMillis = currentMillis;
      //half second has passed
      delayIsPassed = true;
    }
  } else {
    if (currentMillis - VibratorpreviousMillis >= intervalue) {
      VibratorpreviousMillis = currentMillis;

      power = 0;
      vibrationSequence++;

      vibrate, delayIsPassed = false;

    } else {
      power = vibrationForce;
    }
  }

  /* if (interval % vibrationDuration == 0) {
    power = 0;
    vibrationSequence++;
    vibrate = false;
  }*/
  Serial.print(String(" VIBRATING: ") + power);
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
    Count_pulses--;
  }
}