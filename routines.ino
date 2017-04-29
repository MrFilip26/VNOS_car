// fuctions for threads for distance calculation
void brake()
{
  Serial.println("brake");
    digitalWrite(I1, HIGH);
    digitalWrite(I2, HIGH);
    digitalWrite(I3, HIGH);
    digitalWrite(I4, HIGH); 
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
void setSpeedMove(int _speed)
{
    if (_speed < 225)
    {
      speedRight = _speed;
      speedLeft = _speed + 30;
    }
    analogWrite(E1, speedLeft);
    analogWrite(E2, speedRight);
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
void checkDistance()
{
  int aktDistance = frontDistanceThread.check();
    if(aktDistance != 0)
      distance = aktDistance;
    int aktDistanceL = leftDistanceThread.check();
    if(aktDistanceL != 0)
      distanceLeft = aktDistanceL;
    int aktDistanceR = rightDistanceThread.check();
    if(aktDistanceR != 0)
      distanceRight = aktDistanceR;

    temperature = measureTemperature();
}
float measureTemperature(){
  int senzorVal = analogRead(tempPort); // read data from silicon sensor
  float voltage = (senzorVal / 1024.0) * 5.0; // calculate voltage according to catalogue
  
  return (voltage - 0.5) * 100; // calculate temperature in [°C]
}
