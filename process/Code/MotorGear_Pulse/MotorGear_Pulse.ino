#define Encoder_output_A 17  // pin2 of the Arduino
#define Encoder_output_B 5   // pin 3 of the Arduino
// these two pins has the hardware interrupts as well.



#define redPin 15
#define orangePin 0
#define greenPin 16

#define red 1
#define orange 2
#define green 3

int pwmChannel = 0;    //Choisit le canal 0
int frequence = 1000;  //Fréquence PWM de 1 KHz
int resolution = 8;    // Résolution de 8 bits, 256 valeurs possibles

#define motorPin 32

bool motorFinishedReset = true;
int Count_pulses = 0;

int Currentlevel = 0;

bool gameJustStarted = true;

void setup() {

  //ESP.restart();
  Serial.begin(9600);  // activates the serial communication

  pinMode(Encoder_output_A, INPUT);  // sets the Encoder_output_A pin as the input
  pinMode(Encoder_output_B, INPUT);  // sets the Encoder_output_B pin as the input


  pinMode(redPin, OUTPUT);     //
  pinMode(orangePin, OUTPUT);  //
  pinMode(greenPin, OUTPUT);   //

  pinMode(motorPin, OUTPUT);  //


  //ledcAttachPin(motorPin, 0);

  // Configuration du canal 0 avec la fréquence et la résolution choisie
  //ledcSetup(pwmChannel, frequence, resolution);

  // Assigne le canal PWM au pin 23
  //ledcAttachPin(motorPin, pwmChannel);

  //analogWrite(motorPin, 0);
  //ledcWrite(pwmChannel, 0);
  //delay(1000);

  attachInterrupt(digitalPinToInterrupt(Encoder_output_A), DC_Motor_Position, RISING);
}

void loop() {
  Serial.println(String("Result:  ") + Count_pulses + String(" motorFinishedReset: ") + motorFinishedReset);
  //LevelManager(0);

  if (gameJustStarted) {
    start();
  }

  //analogWrite(motorPin, 255);


  // Créer la tension en sortie choisi
  //ledcWrite(pwmChannel, 100); //1.65 V

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
        led(red, true);
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

void led(int color, bool switchled) {

  switch (color) {

    case 1:
      digitalWrite(redPin, switchled);
      break;

    case 2:
      digitalWrite(orangePin, switchled);
      break;

    case 3:
      digitalWrite(greenPin, switchled);
      break;
  }
}

void ledBlink(int color, bool goBlink) {
  
  if (goBlink) {
    int interval = millis()%60;
    Serial.print(String("Interval value: ") + interval);

    switch (color) {

      case 1:
        if (interval % 1 == 0) {

          digitalWrite(redPin, true);
        } else {
          digitalWrite(redPin, false);
        }
        break;

      case 2:
         if (interval % 1 == 0) {
          digitalWrite(orangePin, true);
        } else {
          digitalWrite(orangePin, false);
        }
        break;

      case 3:
         if (interval % 1 == 0) {
          digitalWrite(greenPin, true);
        } else {
          digitalWrite(greenPin, false);
        }
        break;
    }
  }
}

void resetMotorTo(int position, int speed) {


  if (!inRange(position)) {
    led(red, true);
    int speedMap = map(speed, 0, 100, 0, 255);
    analogWrite(motorPin, speedMap);
    Serial.print("   MotorShouldReset");
  } else {
    Serial.print("   Motor should not move");

    ledBlink(red, true);
    analogWrite(motorPin, 0);
    
    motorFinishedReset = true;
  }
}

bool inRange(int position) {
  return ((Count_pulses >= position - 100 && Count_pulses <= position + 100));
}