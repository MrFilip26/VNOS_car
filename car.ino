#include <TimedAction.h>
#include <Utility.h>
#include <LiquidCrystal.h>
#include "Servo.h"

LiquidCrystal lcd(32, 30, 28, 26, 24, 22); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
Servo horizontalS, verticalS;

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
#define DISTANCECHACK 400 // stop [cm]

bool movingForward = false;
bool ver = false;
bool hor = false;
int verPos = 90;
int horPos = 90;
int distance;
int distanceLeft;
int distanceRight;
float temperature;
char cNEW;
int correctRightVal = 30;
int correctLeftVal = 0;
bool followVall = false;
unsigned long time;
bool enableLookAround = false;
int speedLeft = 130;
int speedRight = 100;
bool paralyzerState = false;

int getDist()
{
  long duration;
  digitalWrite(11, LOW);
  delayMicroseconds(2);
  digitalWrite(11, HIGH);
  delayMicroseconds(10);
  digitalWrite(11, LOW);
  duration = pulseIn(8, HIGH);
  return duration * 0.017;
}
int getDistLeft()
{
  long durationLeft;
  digitalWrite(6, LOW);
  delayMicroseconds(2);
  digitalWrite(6, HIGH);
  delayMicroseconds(10);
  digitalWrite(6, LOW);
  durationLeft = pulseIn(7, HIGH);
  return durationLeft * 0.017;
}
int getDistRight()
{
  long durationRight;
  digitalWrite(5, LOW);
  delayMicroseconds(2);
  digitalWrite(5, HIGH);
  delayMicroseconds(10);
  digitalWrite(5, LOW);
  durationRight = pulseIn(4, HIGH);
  return durationRight * 0.017;
}
// create multi-threaded triggers with constrained execution frequency
TimedAction frontDistanceThread = TimedAction(DISTANCECHACK, getDist);
TimedAction leftDistanceThread = TimedAction(DISTANCECHACK, getDistLeft);
TimedAction rightDistanceThread = TimedAction(DISTANCECHACK, getDistRight);
TimedAction printOnDisplayThread = TimedAction(1000, printDistance);

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
    pinMode(3, OUTPUT); // paralyzer ON / OFF
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
  checkDistance();
  printOnDisplayThread.check();
  /*
    if (enableLookAround)
      lookAround();

    handleCollision();
    */
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
int printDistance()
{
  lcd.setCursor(0,0); // Sets the location at which subsequent text written to the LCD will be displayed
  lcd.print("L"); // Prints string "Distance" on the LCD
  if(distanceLeft < 999)
  {
    lcd.print(distanceLeft);
    lcd.print("   ");
  }
  else
  {
    lcd.print("INF   ");  
  }
  lcd.setCursor(6,0);
  lcd.print("F");
  if(distance < 999)
  {
    lcd.print(distance);
    lcd.print("    "); 
  }
  else
  {
    lcd.print("INF   ");  
  }
  lcd.setCursor(12,0);
  lcd.print("R");
  if(distanceRight < 999)
  {
    lcd.print(distanceRight);
    lcd.print("    ");
  }
  else
  {
    lcd.print("INF     ");  
  }

  lcd.setCursor(0,1);
  lcd.print("Tmp: "); // Prints string "Distance" on the LCD
  lcd.print(temperature);
  return 1;
}
