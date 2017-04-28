#include <TimedAction.h>
#include <Utility.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(32, 30, 28, 26, 24, 22); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)

#include "Servo.h"
#include "Func.h"

#define echoPin 8  // for ultrasonic senzor
#define trigPin 11 // for ultrasonic senzor
#define echoPinLeft 7  // for ultrasonic senzor
#define trigPinLeft 6 // for ultrasonic senzor
#define echoPinRight 4  // for ultrasonic senzor
#define trigPinRight 5 // for ultrasonic senzor
#define horizontal 9  // horizontal servo
#define vertical 10   // vertical servo
#define E2 12   // Enable Pin for motor 2
#define E1 13   // Enable Pin for motor 1

#define I4 A0   // Control pin 2 for motor 2
#define I3 A1   // Control pin 1 for motor 2
#define tempPort A2
#define I2 A4   // Control pin 2 for motor 1
#define I1 A5   // Control pin 1 for motor 1

#define HITDISTANCE 50 // stop [cm]

const int ENA = 12;
const int ENB = 13;
const int IN1 = A5; 
const int IN2 = A4;
const int IN3 = A1;
const int IN4 = A0;

Servo horizontalS, verticalS;

bool movingForward = false;
bool ver = false;
bool hor = false;
int verPos = 90;
int horPos = 90;
int moveSpeed = 190;
int distance;
int distanceLeft;
int distanceRight;
float temperature;
long duration;
long durationLeft;
long durationRight;
char cNEW;
int correctRightVal = 30;
int correctLeftVal = 0;
bool followVall = false;
unsigned long time;
bool enableLookAround = false;
int speedLeft = 130;
int speedRight = 100;
bool paralyzerState = false;

int correct = 0;
/*
// create multi-threaded triggers with constrained execution frequency
TimedAction frontDistanceThread = TimedAction(50, getDist);
TimedAction leftDistanceThread = TimedAction(200, getDistLeft);
TimedAction rightDistanceThread = TimedAction(200, getDistRight);
*/
void setup()
{
  lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display
    Serial.begin(9600); // init serial port
    // set modes to pins for speed of left & right motors
    pinMode(E1, OUTPUT);
    pinMode(E2, OUTPUT);
    // attach front sonar servos
    horizontalS.attach(horizontal);
    verticalS.attach(vertical);
    // init front sonar to look straight ahead
    horizontalS.write(90); // middle of range
    verticalS.write(100); // middle of range
    
    // set modes to pins for direction of all 4 motors 
    pinMode(I1, OUTPUT);
    pinMode(I2, OUTPUT);
    pinMode(I3, OUTPUT);
    pinMode(I4, OUTPUT);

    pinMode(3, OUTPUT);

    // sonar: trigger sends, echo receives
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(trigPinLeft, OUTPUT);
    pinMode(echoPinLeft, INPUT);
    pinMode(trigPinRight, OUTPUT);
    pinMode(echoPinRight, INPUT);
}

void loop()
{
  handleBluetooth(); // process bluetooth input signal
  int aktDistance = getDist();
  /*
   * 
    int aktDistance = frontDistanceThread.check();
    if(aktDistance != 0)
      distance = aktDistance;
    //distanceLeft = leftDistanceThread.check();
    //distanceRight = rightDistanceThread.check();

    handleBluetooth(); // process bluetooth input signal
    if (enableLookAround)
      lookAround();

    handleCollision();

    // raz za cyklus zmeraj teplotu
    temperature = measureTemperature();
    lcd.setCursor(0,0); // Sets the location at which subsequent text written to the LCD will be displayed
    lcd.print("Distance: "); // Prints string "Distance" on the LCD
    lcd.print(distance);
    lcd.print(" cm      ");
    lcd.setCursor(0,1);
    lcd.print("Teplota: "); // Prints string "Distance" on the LCD
    lcd.print(temperature);
    
    if(movingForward)
    {
        // steering correction - right
        analogWrite(E2, 0); // turn off right motor
        delay(correctRightVal); // wait N [ms]
        analogWrite(E2, moveSpeed); // turn on right motor
        
      // forward drive slowdown
        analogWrite(E1, 0);
        analogWrite(E2, 0);
        delay(30); // factor of slowdown [ms]
        analogWrite(E1, moveSpeed);
        analogWrite(E2, moveSpeed);
    }
    // TODO - turn on/off by bool switch
    //followWallFunc();
    //printDistances();
    */
    
    lcd.setCursor(0,0); // Sets the location at which subsequent text written to the LCD will be displayed
    lcd.print("Distance: "); // Prints string "Distance" on the LCD
    lcd.print(aktDistance);
    lcd.print(" cm     ");
    /*
    lcd.setCursor(0,0); // Sets the location at which subsequent text written to the LCD will be displayed
    lcd.print("SpeedLeft: "); // Prints string "Distance" on the LCD
    lcd.print(speedLeft);
    lcd.print("     ");
    lcd.setCursor(0,1);
    lcd.print("SpeedRight: "); // Prints string "Distance" on the LCD
    lcd.print(speedRight);
    lcd.print("     ");
    */
}
void lookAround()
{
  if (hor) //horizontal lookAround for servo
  {
      horizontalS.write(horPos -= 2);
      if (horPos < 75)
      {
          hor = false;
      }
  }
  else
  {
      horizontalS.write(horPos += 2);
      if (horPos > 115)
      {
          hor = true;
      }
  }
  /*
  if (ver) // vertical lookAround for servo
  {
      verticalS.write(verPos -= 2);
      if (verPos < 80)
      {
          ver = false;
      }
  }
  else
  {
      verticalS.write(verPos += 2);
      if (verPos > 110)
      {
          ver = true;
      }
  }*/
}

void handleCollision()
{
    if (distance < HITDISTANCE && movingForward == true && !followVall)
    {
        int distanceLeftAkt;
        int distanceRightAkt;

        stopMove();
        distanceLeftAkt = getDistLeft();
        delay(300);
        distanceRightAkt = getDistRight();
        if(distanceLeftAkt > distanceRightAkt)
        { 
          turnLeftParam(400);
          delay(100);
          moveForward();
        }
        else
        {
          turnRightParam(400);
          delay(100);
          moveForward();
        }
    }
}

void handleBluetooth()
{
    cNEW = 0;
    if (Serial.available() > 0)
    {
      time = millis();
        cNEW = Serial.read();
        switch (cNEW)
        {
            case 'W': { moveForward(); time = millis(); break; }
            case 'S': { moveBack(); break; }
            case 'A': { turnLeft(); break; }
            case 'D': { turnRight(); break; }
            case 'Q': { stopMove(); break; }

            case 'O': { handleParalyzer(); break; }
            case 'X': { enableLookAround = false; break; }
            
            case '1': { speedUP(); break; }
            case '2': { speedDOWN(); break; }
            case '3': { correctLeft(); break; }
            case '4': { correctRight(); break; }
            default : { brake(); break; }
            
            //case '5': { followVall = true; break; }
            //550
            case 'N': { turnLeftParam(100); break; }
            case 'M': { turnRightParam(200); break; }
        }
    }
    else
    {
      if(millis() - time > 500)
      {
        brake();    
      }
    }
}
void followWallFunc()
{
    if (movingForward && followVall)
    {
        horizontalS.write(180);
        if (distance > 50)
        {
            correct = 1;
            correctRightVal = 250;
        }
        else if (distance < 30)
        {
            time = millis();
            correct = -1;
            correctRightVal = -125;
        }
        else
        {
            time = millis();
            correct == 0;
            correctRightVal = 0;
        }

        if (correct == 1)
        {
            analogWrite(E2, 0);
            delay(correctRightVal);
            analogWrite(E2, moveSpeed);
        }
        else if (correct == -1)
        {
            correctLeftVal = -correctRightVal;
            analogWrite(E1, 0);
            delay(correctLeftVal);
            analogWrite(E1, moveSpeed);
        }
        else
        {

        }
        //ak nevidi stenu 2,5 sekundy pozrie sa vpred
        if (millis() - time > 2500)
        {
            followVall = false;
        }
        Serial.println(correctRightVal);

        // spomalenie 
        analogWrite(E1, 0);
        analogWrite(E2, 0);
        delay(30);
        analogWrite(E1, moveSpeed);
        analogWrite(E2, moveSpeed);
    }
}

