//#include <LiquidCrystal.h>
//LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)

#include "Servo.h"
#include "Func.h"

#define echoPin 8  // for ultrasonic senzor
#define echoPinLeft 4  // for ultrasonic senzor
#define trigPinLeft 5 // for ultrasonic senzor
#define echoPinRight 7  // for ultrasonic senzor
#define trigPinRight 6 // for ultrasonic senzor
#define trigPin 11 // for ultrasonic senzor
#define horizontal 9  // horizontal servo
#define vertical 10   // vertical servo
#define E2 12   // Enable Pin for motor 2
#define E1 13   // Enable Pin for motor 1

#define I4 A0   // Control pin 2 for motor 2
#define I3 A1   // Control pin 1 for motor 2
#define tempPort A2
#define I2 A4   // Control pin 2 for motor 1
#define I1 A5   // Control pin 1 for motor 1

#define HITDISTANCE 30
#define DEBUG

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
int senzorVal;
float voltage;
float temperature;
long duration;
long durationLeft;
long durationRight;
char cNEW;
int corectRightVal = 20;
int corectLeftVal = 0;
bool followVall = false;
unsigned long time;
bool enable = false;

int corect = 0;

void setup() 
{
  //lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display
    Serial.begin(9600);
    pinMode(E1, OUTPUT);
    pinMode(E2, OUTPUT);
    horizontalS.attach(horizontal);
    verticalS.attach(vertical);
    
    pinMode(I1, OUTPUT);
    pinMode(I2, OUTPUT);
    pinMode(I3, OUTPUT);
    pinMode(I4, OUTPUT);
    
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}
void loop() {
#ifdef DEBUG
  cNEW=0;
  if (Serial.available() > 0)
  {
      cNEW = Serial.read();
        switch(cNEW)
        {
          case 'W' : {moveForward(); time = millis(); break;}
          case 'S' : {moveBack(); break;}
          case '1' : {speedUP(); break;}
          case '2' : {speedDOWN(); break;}
          case '3' : {corectLeft(); break;}
          case '4' : {corectRight(); break;}
          case '5' : {followVall = true; break;}
          case '6' : {enable = false; break;}
          case 'A' : {turnLeft(); break;}
          case 'D' : {turnRight(); break;}
          case 'Q' : {stopMove(); break;}
          case 'N' : {turnLeftParam(0); break;}
          case 'M' : {turnRightParam(0); break;}
        }
  }
  if(!followVall && enable)
  {
    if(hor) //horizontal lookAround for servo
    {
        horizontalS.write(horPos -= 3);
        if(horPos < 70)
        {
          hor = false;
          // raz za cyklus zmeraj teplotu
          senzorVal = analogRead(tempPort);
          voltage = (senzorVal/1024.0) * 5.0;
          temperature = (voltage - 0.5) * 100;
        }
    }
    else
    {
      horizontalS.write(horPos += 3);
      if(horPos > 120)
      {
        hor = true;
      }
    }
    if(ver) // vertical lookAround for servo
    {
        verticalS.write(verPos -= 2);
        if(verPos < 80)
        {
          ver = false;
        }
    }
    else
    {
      verticalS.write(verPos += 2);
      if(verPos > 110)
      {
        ver = true;
      }
    }  
  }
  // meranie vzdialenosti ultrazvukovym senyoom
  //digitalWrite(trigPin, LOW);
  //digitalWrite(trigPinLeft, LOW);
  digitalWrite(trigPinRight, LOW);
  delayMicroseconds(2);
  //digitalWrite(trigPin, HIGH);
  //digitalWrite(trigPinLeft, HIGH);
  digitalWrite(trigPinRight, HIGH);
  delayMicroseconds(10);
  //digitalWrite(trigPin, LOW);
  //digitalWrite(trigPinLeft, LOW);
  digitalWrite(trigPinRight, LOW);
  //duration = pulseIn(echoPin, HIGH);
  //durationLeft = pulseIn(echoPinLeft, HIGH);
  durationRight = pulseIn(echoPinRight, HIGH);
  distance = duration*0.017;
  distanceLeft = durationLeft * 0.017;
  distanceRight = durationRight * 0.017;
  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.print("DistanceLeft: ");
  Serial.println(distanceLeft);
  Serial.print("DistanceRight: ");
  Serial.println(distanceRight);

  if(distance < HITDISTANCE && movingForward == true && !followVall)
  {
    stopMove();
  }
  /*
  lcd.setCursor(0,0); // Sets the location at which subsequent text written to the LCD will be displayed
  lcd.print("Distance: "); // Prints string "Distance" on the LCD
  lcd.print(distance); // Prints the distance value from the sensor
  lcd.print(" cm      ");
  
  lcd.setCursor(0,1);
  lcd.print("Teplota: "); // Prints string "Distance" on the LCD
  //lcd.print(temperature);
*/
  if(movingForward && followVall)
  {
    horizontalS.write(180);
    if(distance > 50)
    {
      corect = 1;
      corectRightVal = 250;
    }
    else if(distance < 30)
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
    
    if(corect == 1)
    {
      analogWrite(E2, 0);
      delay(corectRightVal);
      analogWrite(E2, moveSpeed);
    }
    else if(corect == -1)
    {
      corectLeftVal = -corectRightVal;
      analogWrite(E1, 0);
      delay(corectLeftVal);
      analogWrite(E1, moveSpeed);
    }
    else
    {
    
    }

    if(millis() - time > 2500)
    {
        followVall = false;
    }
    //lcd.print(corectRightVal);
    //lcd.print("         ");
    Serial.println(corectRightVal);

    analogWrite(E1, 0);
    analogWrite(E2, 0);
    delay(30);
    analogWrite(E1, moveSpeed);
    analogWrite(E2, moveSpeed);
  }
#endif
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

    delay(335);
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

    delay(100);
    analogWrite(E1, 0);
    analogWrite(E2, 0);
}
void corectLeft()
{
  corectRightVal += 100;
}
void corectRight()
{
  corectRightVal -= 100;
}
void moveForward()
{
  enable = true;
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
    if(moveSpeed < 225)
    {
      moveSpeed += 30;
    }
    analogWrite(E1, moveSpeed);
    analogWrite(E2, moveSpeed);
}
void speedDOWN()
{
    if(moveSpeed < 120)
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

