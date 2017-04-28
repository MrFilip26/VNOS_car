// fuctions for threads for distance calculation


void correctLeft()
{
    correctRightVal += 2;
}

void correctRight()
{
    correctRightVal -= 2;
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

