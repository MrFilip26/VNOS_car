#include <TimedAction.h>
#include <Utility.h>

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
int corectRightVal = 30;
int corectLeftVal = 0;
bool followVall = false;
unsigned long time;
bool enableLookAround = false;

int corect = 0;
// fuctions for threads for distance calculation
int getDist()
{
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
    digitalWrite(6, LOW);
    delayMicroseconds(2);
    digitalWrite(6, HIGH);
    delayMicroseconds(10);
    digitalWrite(6, LOW);
    duration = pulseIn(7, HIGH);
    return duration * 0.017;
}
int getDistRight()
{
    digitalWrite(5, LOW);
    delayMicroseconds(2);
    digitalWrite(5, HIGH);
    delayMicroseconds(10);
    digitalWrite(5, LOW);
    duration = pulseIn(4, HIGH);
    return duration * 0.017;
}
// create multi-threaded triggers with constrained execution frequency
TimedAction frontDistanceThread = TimedAction(50, getDist);
TimedAction leftDistanceThread = TimedAction(200, getDistLeft);
TimedAction rightDistanceThread = TimedAction(200, getDistRight);

void setup()
{
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
    int aktDistance = frontDistanceThread.check();
    if(aktDistance != 0)
      distance = aktDistance;
    //distanceLeft = leftDistanceThread.check();
    //distanceRight = rightDistanceThread.check();

    handleBluetooth(); // process bluetooth input signal
    if (enableLookAround)
      lookAround();

    handleCollision();

    // steering correction - right
    analogWrite(E2, 0); // turn off right motor
    delay(corectRightVal); // wait N [ms]
    analogWrite(E2, moveSpeed); // turn on right motor
    
    if(movingForward)
    {
      // forward drive slowdown
        analogWrite(E1, 0);
        analogWrite(E2, 0);
        delay(30); // factor of slowdown [ms]
        analogWrite(E1, moveSpeed);
        analogWrite(E2, moveSpeed);
    }
    // TODO - turn on/off by bool switch
    //fallowWallFunc();
    //printDistances();
}
void printDistances()
{
    if (distance != 0)
        Serial.println("Front" + distance);
    if (distanceLeft != 0)
        Serial.println("Left" + distanceLeft);
    if (distanceRight != 0)
        Serial.println("Right" + distanceRight);
}

float measureTemperature(){
  int senzorVal = analogRead(tempPort); // read data from silicon sensor
  float voltage = (senzorVal / 1024.0) * 5.0; // calculate voltage according to catalogue
  
  return (voltage - 0.5) * 100; // calculate temperature in [°C]
}

void lookAround()
{
  if (hor) //horizontal lookAround for servo
  {
      horizontalS.write(horPos -= 2);
      if (horPos < 75)
      {
          hor = false;
          // raz za cyklus zmeraj teplotu
          temperature = measureTemperature();
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
          turnRightParam(450);
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
        cNEW = Serial.read();
        switch (cNEW)
        {
            case 'W': { moveForward(); time = millis(); break; }
            case 'S': { moveBack(); break; }
            case '1': { speedUP(); break; }
            case '2': { speedDOWN(); break; }
            case '3': { corectLeft(); break; }
            case '4': { corectRight(); break; }
            case '5': { followVall = true; break; }
            case '6': { enableLookAround = false; break; }
            case 'A': { turnLeft(); break; }
            case 'D': { turnRight(); break; }
            case 'Q': { stopMove(); break; }
            case 'N': { turnLeftParam(350); break; }
            case 'M': { turnRightParam(450); break; }
        }
    }
}
void fallowWallFunc()
{
    if (movingForward && followVall)
    {
        horizontalS.write(180);
        if (distance > 50)
        {
            corect = 1;
            corectRightVal = 250;
        }
        else if (distance < 30)
        {
            time = millis();
            corect = -1;
            corectRightVal = -125;
        }
        else
        {
            time = millis();
            corect == 0;
            corectRightVal = 0;
        }

        if (corect == 1)
        {
            analogWrite(E2, 0);
            delay(corectRightVal);
            analogWrite(E2, moveSpeed);
        }
        else if (corect == -1)
        {
            corectLeftVal = -corectRightVal;
            analogWrite(E1, 0);
            delay(corectLeftVal);
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
        Serial.println(corectRightVal);

        // spomalenie 
        analogWrite(E1, 0);
        analogWrite(E2, 0);
        delay(30);
        analogWrite(E1, moveSpeed);
        analogWrite(E2, moveSpeed);
    }
}
//delay 350ms = 90 stupnov
// 200 - 45
// 100 - 15
void turnLeftParam(int degree)
{
    //movingForward
    analogWrite(E1, 150);
    analogWrite(E2, 150);

    digitalWrite(I1, LOW);
    digitalWrite(I2, HIGH);
    digitalWrite(I3, LOW);
    digitalWrite(I4, HIGH);

    delay(degree);
    analogWrite(E1, 0);
    analogWrite(E2, 0);
}
void turnRightParam(int degree)
{
    movingForward = false;
    analogWrite(E1, 150);
    analogWrite(E2, 150);

    digitalWrite(I1, HIGH);
    digitalWrite(I2, LOW);
    digitalWrite(I3, HIGH);
    digitalWrite(I4, LOW);

    delay(degree);
    analogWrite(E1, 0);
    analogWrite(E2, 0);
}
void corectLeft()
{
    corectRightVal += 2;
}
void corectRight()
{
    corectRightVal -= 2;
}
void moveForward()
{
    enableLookAround = true;
    movingForward = true;
    analogWrite(E1, moveSpeed);
    analogWrite(E2, moveSpeed);
    digitalWrite(I1, HIGH);
    digitalWrite(I2, LOW);
    digitalWrite(I3, LOW);
    digitalWrite(I4, HIGH);
}
void moveBack()
{
    movingForward = false;
    analogWrite(E1, moveSpeed);
    analogWrite(E2, moveSpeed);
    digitalWrite(I1, LOW);
    digitalWrite(I2, HIGH);
    digitalWrite(I3, HIGH);
    digitalWrite(I4, LOW);
}
void turnLeft()
{
    movingForward = false;
    analogWrite(E1, 150);
    analogWrite(E2, 150);

    digitalWrite(I1, LOW);
    digitalWrite(I2, HIGH);
    digitalWrite(I3, LOW);
    digitalWrite(I4, HIGH);
}
void turnRight()
{
    movingForward = false;
    analogWrite(E1, 150);
    analogWrite(E2, 150);

    digitalWrite(I1, HIGH);
    digitalWrite(I2, LOW);
    digitalWrite(I3, HIGH);
    digitalWrite(I4, LOW);
}
void speedUP()
{
    if (moveSpeed < 225)
    {
        moveSpeed += 30;
    }
    analogWrite(E1, moveSpeed);
    analogWrite(E2, moveSpeed);
}
void speedDOWN()
{
    if (moveSpeed < 120)
    {
        moveSpeed = 0;
        movingForward = false;
    }
    else
    {
        moveSpeed -= 30;
    }
    analogWrite(E1, moveSpeed);
    analogWrite(E2, moveSpeed);
}
void stopMove()
{
    movingForward = false;
    followVall = false;
    analogWrite(E1, 200);
    analogWrite(E2, 200);
    digitalWrite(I1, LOW);
    digitalWrite(I2, HIGH);
    digitalWrite(I3, HIGH);
    digitalWrite(I4, LOW);

    delay(350);
    analogWrite(E1, LOW);
    analogWrite(E2, LOW);
}
