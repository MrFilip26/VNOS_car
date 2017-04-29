#define SD 100 // stop distance [cm]
#define TA 15 // single step of turnaround [degrees]
#define OOR 2700 // low bound for Out Of Range signal [cm]
#define OWD 150 // optimal wall distance [cm]
#define MTA 1 // minimal turn angle [degrees]
#define Q_SIZE 3 // size of the analyzed sequence of distances

enum_state state = IDLE;
enum_side wallside = LEFT;
bool forward = false;
int FD = 0;
int LD = 0;
int RD = 0;
QueueArray <int> Q;

void init_ae()
{
	init_servos();
	forward = false;
}

void step_ae()
{
	FD = distanceFront;
	LD = distanceLeft;  	
	RD = distanceRight; 

	switch(state) {
		case IDLE: findDirection(); break;
		case EXPLORE: explore(); break;
		default: writeToDisplay(1, "SWITCH FAILED !!!!!!"); break;
	}
}

void findDirection()
{
	if ((FD > SD && FD > min(LD, RD)) || min(FD, min(LD, RD)) > OOR)
		state = EXPLORE;
	else
		turnStep();
}

void explore()
{
	if (FD <= SD)
	{
		brake();
		state = IDLE;
	}
	else
	{
		if (max(LD, RD) < OOR)
			passCorridor();
		else
			if (min(LD, RD) < OOR)
				followWall();
			else
				moveForward();
	}
}

void passCorridor()
{
	if (debugModeOn)
		writeToDisplay(1, "CORRIDOR");

	push(abs(LD-RD));

	if (Q.isFull() && grows())
		turnStep();
	moveForward();
}

void followWall()
{
	int WD; // wall distance

	if (debugModeOn)
		writeToDisplay(1, "WALL");
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
		if (Q.isFull() && grows())
		{
			brake();
			state = IDLE;
			if (WD < 1)
				turn(opposite(wallside), MTA);
			else
				turn(wallside, MTA);
		}
		else
			moveForward();
	}
	else
		moveForward();
}

void push(int value)
{
	Q.enqueue(value);
	if (Q.count() > Q_SIZE)
		Q.dequeue();
}

bool grows()
{
	bool result = true;
	int prev = 0;
	int curr = 0;

	for (int i = 0; i < Q.count(); i++)
	{
		curr = Q.dequeue();
		if (curr < prev)
			result = false;
		Q.enqueue(curr);
		prev = curr;
	}

	return result;
}

void turnStep()
{
	enum_side turnside;

	if (LD > RD)
		turnside = LEFT;
	else
		turnside = RIGHT;

	turn(turnside, TA);
}

enum_side opposite(enum_side input)
{
	if (input == LEFT)
		return RIGHT;
	else
		return LEFT;
}

