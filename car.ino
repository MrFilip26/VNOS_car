#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)

#define echoPin 8  // for ultrasonic senzor
#define horizontal 9  // horizontal servo
#define vertical 10   // vertical servo
#define trigPin 11 
#define E2 12   // Enable Pin for motor 2
#define E1 13   // Enable Pin for motor 1

#define I4 A0   // Control pin 2 for motor 2
#define I3 A1   // Control pin 1 for motor 2
#define tempPort A2
#define I2 A4   // Control pin 2 for motor 1
#define I1 A5   // Control pin 1 for motor 1
#define HITDISTANCE 15
#define DEBUG

Servo horizontalS, verticalS;

bool ver = false;
bool hor = false;
bool movingForward = false;
int verPos = 90;
int horPos = 90;
int moveSpeed = 190;
int distance;
int senzorVal;
float voltage;
float temperature;
long duration;
char cNEW;
int corectRightVal = 20;

void setup() 
{
  lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display
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
          case 'W' : {moveForward(); break;}
          case 'S' : {moveBack(); break;}
          case '1' : {speedUP(); break;}
          case '2' : {speedDOWN(); break;}
          case '3' : {corectLeft(); break;}
          case '4' : {corectRight(); break;}
          case 'A' : {turnLeft(); break;}
          case 'D' : {turnRight(); break;}
          case 'Q' : {stopMove(); break;}
        }
  }
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
  
  // meranie vzdialenosti ultrazvukovym senyoom
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;
  //Serial.print("Distance: ");
  //Serial.println(distance);

  if(distance < HITDISTANCE && movingForward == true)
  {
    stopMove();
  }
  lcd.setCursor(0,0); // Sets the location at which subsequent text written to the LCD will be displayed
  lcd.print("Distance: "); // Prints string "Distance" on the LCD
  lcd.print(distance); // Prints the distance value from the sensor
  lcd.print(" cm");
  
  lcd.setCursor(0,1);
  lcd.print("Teplota: "); // Prints string "Distance" on the LCD
  lcd.print(temperature);

  if(movingForward)
  {
    analogWrite(E2, 0);
    delay(corectRightVal);
    analogWrite(E2, moveSpeed);
  //lcd.print(corectRightVal);
  }
  
#endif
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
