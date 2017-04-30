#define SD 75 // stop distance [cm]
#define TA 15 // single step of turnaround [degrees]
#define OOR 2500 // low bound for Out Of Range signal [cm]
#define OWD 150 // optimal wall distance [cm]
#define MTA 3 // minimal turn angle [degrees]
#define Q_SIZE 7 // size of the analyzed sequence of 

enum_state state = IDLE;
enum_side wallside = LEFT;
int FD = 0;
int FSD = SD + 1; // for first step (value is used from previous step)
int LD = 0;
int RD = 0;
QueueArray <int> Q;

void init_ae()
{
	init_servos();
}

void step_ae()
{
	FD = distanceFront;
	LD = distanceLeft;
	RD = distanceRight;

	switch(state) {
		case IDLE: findDirection(); Serial.println("findDirection"); break;
		case EXPLORE: explore(); Serial.println("explore"); break;
		default: writeToDisplay(1, "WTF?! SWITCH FAILED ~ O.o"); break;
	}
}

void findDirection()
{
	if ((FD > SD && FD > min(LD, RD)) || min(FD, min(LD, RD)) > OOR)
		state = EXPLORE;
	else
		turnStep(true);
}

void explore()
{
  checkCloserSideFSD();
  if (FSD <= SD)
    turnStep(true);

	if (FD <= SD)
	{
		brake();
		state = IDLE;
	}
	else
	{
		if (max(LD, RD) < OOR)
    {
			passCorridor();
    }
		else
			if (min(LD, RD) < OOR)
      {
				followWall();
      }
			else
				moveForward();
	}
}

void passCorridor()
{
	if (debugModeOn)
		writeToDisplay(1, "CORRIDOR            ");

	push(abs(LD-RD));

	if (full() && grows())
	{
		writeToDisplay(1, "CORRIDOR - GROWS");
		turnStep(false); // true = turn, false => steer
		empty();
	}
	moveForward();
}

void followWall()
{
	int WD; // wall distance

	if (debugModeOn)
		writeToDisplay(1, "WALL                  ");

	if (LD < RD)
	{
		WD = LD;
		wallside = LEFT;
	}
	else
	{
		WD = RD;
		wallside = RIGHT;
	}

	push(abs(WD-OWD));

	if (WD < 1 || WD > 2)
	{
		if (full() && grows())
		{
			writeToDisplay(1, "WALL - GROWS");
			brake();
			state = IDLE;
			if (WD < 1)
				//turn(opposite(wallside), MTA);
				steer(opposite(wallside), MTA);
			else
				//turn(wallside, MTA);
				steer(wallside, MTA);
			empty();
			moveForward();
		}
		else
			moveForward();
	}
	else
		moveForward();
}

void push(int value)
{
	if (full())
		Q.dequeue();
	Q.enqueue(value);
}

bool full()
{
	if (Q.count() == Q_SIZE)
		return true;
	else
		return false;
}

bool grows()
{
	int incr = 0, decr = 0, curr = 0, prev = 0;

	for (int i = 0; i < Q.count(); i++)
	{
		curr = Q.dequeue();
		if (curr > prev)
			incr++;
		else
			decr++;
		Q.enqueue(curr);
		prev = curr;
	}

	Serial.print("inc > dec = ");
	Serial.print(incr > decr);
	Serial.print(" | ");
	Serial.print(incr);
	Serial.print("-");
	Serial.print(decr);
	Serial.println("");
	return (incr > decr);
}

void empty()
{
	int d;
	Serial.print("emptying queue with size ");
	Serial.println(Q.count());
	while (!Q.isEmpty())
	{
		d = Q.dequeue();
		Serial.print(d);
		Serial.print(" ");
	}
	Serial.println("");
}

void turnStep(bool turning)
{
	enum_side turnside;

	if (LD > RD)
		turnside = LEFT;
	else
		turnside = RIGHT;

	if (turning)
		turn(turnside, TA);
	else
		steer(turnside, TA);
}

enum_side opposite(enum_side input)
{
	if (input == LEFT)
		return RIGHT;
	else
		return LEFT;
}

int check_side(enum_side input)
{
  int result;

  if (input == LEFT)
  {
    horizontalS.write(horPos+45);
    delay(250);
    result = getDist();
  }
  else
  {
    horizontalS.write(horPos-45);
    delay(250);
    result = getDist();
  }
  delay(100);

  horizontalS.write(horPos);
  delay(250);

  return result;
}

void checkCloserSideFSD()
{
  // measure front side distance
  if (LD > RD)
    FSD = check_side(LEFT);
  else
    FSD = check_side(RIGHT);
}
