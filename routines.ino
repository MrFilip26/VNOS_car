// fuctions for threads for distance calculation
void brake()
{
  Serial.println("brake");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH); 
}
//delay 550ms = 90 stupnov
//otacanie na mieste .. parameter urci cas teda stupne
void turnLeftParam(int degree)
{
  Serial.println("Turn Left");
    //movingForward
    analogWrite(E1, 220);
    analogWrite(E2, 250);

    digitalWrite(I1, LOW);
    digitalWrite(I2, HIGH);
    digitalWrite(I3, LOW);
    digitalWrite(I4, HIGH);

    delay(degree);
    analogWrite(E1, LOW);
    analogWrite(E2, LOW);
}
void turnRightParam(int degree)
{
  Serial.println("Turn Right");
    movingForward = false;
    analogWrite(E1, 220);
    analogWrite(E2, 250);

    digitalWrite(I1, HIGH);
    digitalWrite(I2, LOW);
    digitalWrite(I3, HIGH);
    digitalWrite(I4, LOW);

    delay(degree);
    analogWrite(E1, LOW);
    analogWrite(E2, LOW);
}
void moveForward()
{
  Serial.println("Move forward");
    enableLookAround = true;
    movingForward = true;
    speedLeft = 120;
    speedRight = 150;
    analogWrite(E1, speedLeft);
    analogWrite(E2, speedRight);
    digitalWrite(I1, HIGH);
    digitalWrite(I2, LOW);
    digitalWrite(I3, LOW);
    digitalWrite(I4, HIGH);
}
void moveBack()
{
  Serial.println("Move Back");
    speedLeft = 120;
    speedRight = 150;
    movingForward = false;
    analogWrite(E1, speedLeft);
    analogWrite(E2, speedRight);
    digitalWrite(I1, LOW);
    digitalWrite(I2, HIGH);
    digitalWrite(I3, HIGH);
    digitalWrite(I4, LOW);
}
void turnLeft()
{
  Serial.println("Spin Left");
    movingForward = false;
    analogWrite(E1, speedLeft);
    analogWrite(E2, speedRight);

    digitalWrite(I1, LOW);
    digitalWrite(I2, HIGH);
    digitalWrite(I3, LOW);
    digitalWrite(I4, HIGH);
}
void turnRight()
{
  Serial.println("Spin Right");
    movingForward = false;
    analogWrite(E1, speedLeft);
    analogWrite(E2, speedRight);

    digitalWrite(I1, HIGH);
    digitalWrite(I2, LOW);
    digitalWrite(I3, HIGH);
    digitalWrite(I4, LOW);
}
void correctLeft()
{
    speedLeft += 30;
    analogWrite(E1, speedLeft);
    analogWrite(E2, speedRight);
}
void correctRight()
{
    speedRight += 30;
    analogWrite(E1, speedLeft);
    analogWrite(E2, speedRight);
}
void speedUP()
{
    if (speedLeft < 225 && speedRight < 225)
    {
      speedRight += 30;
      speedLeft += 30;
    }
    analogWrite(E1, speedLeft);
    analogWrite(E2, speedRight);
}
void speedDOWN()
{
    if (speedLeft > 100 && speedRight > 100)
    {
      speedRight -= 30;
      speedLeft -= 30;
    }
    else
    {
      analogWrite(E1, LOW);
      analogWrite(E2, LOW);
    }
    analogWrite(E1, speedLeft);
    analogWrite(E2, speedRight);
}
void stopMove()
{
    movingForward = false;
    followVall = false;
    analogWrite(E1, HIGH);
    analogWrite(E2, HIGH);
    digitalWrite(I1, LOW);
    digitalWrite(I2, HIGH);
    digitalWrite(I3, HIGH);
    digitalWrite(I4, LOW);

    delay(300);
    analogWrite(E1, LOW);
    analogWrite(E2, LOW);
}
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
void handleParalyzer()
{
  if(paralyzerState)
  {
    Serial.println("Paralyzer OFF");
    digitalWrite(3, LOW); 
    paralyzerState = false;
  }
  else
  {
    paralyzerState = true;
    Serial.println("Paralyzer ON");
    digitalWrite(3, HIGH); 
  }
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
