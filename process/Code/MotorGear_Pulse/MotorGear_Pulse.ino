// these two pins has the hardware interrupts as well.
#define Encoder_output_A 17  // pin2 of the Arduino
#define Encoder_output_B 5   // pin 3 of the Arduino

// led pins
#define redPin 15
#define orangePin 0
#define greenPin 16

// make my function(ledManagement) faster.
#define red 1
#define orange 2
#define green 3

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





//define seconds function
#define seconds() (millis() / 1000)

//motor+
#define motorPin 32

bool motorFinishedReset = true;

//Motor Encoder Value
int Count_pulses = 0;

//current Puzzle to solve
int Currentlevel = 0;

//pull string to begin.
bool gameJustStarted = true;

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

  //Esp32 pin to transistor to motor+
  pinMode(motorPin, OUTPUT);

  //each time encoder is used -> ++Count_pulses or --Count_pulses
  attachInterrupt(digitalPinToInterrupt(Encoder_output_A), DC_Motor_Position, RISING);
}

void loop() {
  Serial.println(String("Result:  ") + Count_pulses + String(" motorFinishedReset: ") + motorFinishedReset);

  //LevelManager(0);

  if (gameJustStarted) {
    start();
  }



  if (lightled) {
    ledManager(color, state, isblinking, blinkingTime);
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
  } else {
    //Serial.print("GAME HAS STARTED");
    //gameJustStarted = true;
    //Puzzle begin
    //LevelOneCanStart = true;
  }
}

int LevelRules(int rulesLevel) {
}

void LevelManager(int Level) {


  switch (Level) {

    //Level 0 = tuto
    case 0:



      if (Count_pulses <= -1000) {
        punish(Level);
        //led(red, light,);
      }

      break;
  }
}

void DC_Motor_Position() {
  int b = digitalRead(Encoder_output_B);
  if (b > 0) {
    Count_pulses++;
  } else {
    Count_pulses--;
  }
}

void led(int ledcolor, bool On_Off, bool blink, int timeBlinking) {

  color = ledcolor;
  state = On_Off;
  isblinking = blink;
  blinkingTime = timeBlinking;

  lightled = true;
}

void ledManager(int color, bool On_Off, bool blink, int blinkingTime) {


  int interval = seconds();
  Serial.print(String("   Interval value:  ") + interval + String("    "));

  if (blinkingTime != 0) {
    Serial.print("   Should Blink... ");
    if (interval % blinkingTime == 0) {
      //goBlink = false;
      digitalWrite(redPin, false);
      digitalWrite(orangePin, false);
      digitalWrite(greenPin, false);
      lightled = false;
    }
  }
  if (blink) {
    switch (color) {
      case 1:
        if (interval % 2 == 0) {

          digitalWrite(redPin, true);
        } else {
          digitalWrite(redPin, false);
        }
        break;

      case 2:
        if (interval % 2 == 0) {
          digitalWrite(orangePin, true);
        } else {
          digitalWrite(orangePin, false);
        }
        break;

      case 3:
        if (interval % 2 == 0) {
          digitalWrite(greenPin, true);
        } else {
          digitalWrite(greenPin, false);
        }
        break;
    }
  } else {

    //just turn ON/OFF
    switch (color) {
      case 1:
        digitalWrite(redPin, On_Off);
        break;

      case 2:
        digitalWrite(orangePin, On_Off);
        break;

      case 3:
        digitalWrite(greenPin, On_Off);
        break;
    }
  }
}

void resetMotorTo(int position, int speed) {

  if (!inRange(position)) {

    led(red, light, justON, 0);
    int speedMap = map(speed, 0, 100, 0, 255);
    analogWrite(motorPin, speedMap);
    Serial.print("   MotorShouldReset");

  } else {

    Serial.print("   Motor should not move");
    led(red, light, Blink, 4);
    analogWrite(motorPin, 0);

    motorFinishedReset = true;
  }
}

bool inRange(int position) {
  return ((Count_pulses >= position - 100 && Count_pulses <= position + 100));
}