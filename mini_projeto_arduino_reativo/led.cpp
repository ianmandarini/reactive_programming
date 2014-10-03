#include <math.h>

#define MAX_LIGHT_FACTOR 0.9

// A sensibilidade pode ser definida por aqui
//#define NORMALIZE_DISTANCE_FACTOR 0.17 // -> RUIM
//#define NORMALIZE_DISTANCE_FACTOR 0.10 // -> OK
#define NORMALIZE_DISTANCE_FACTOR 0.06 // -> BOM

#define NUM_SENSORS 4
#define NUM_ROWS 5
#define NUM_COLS 5

#define TIME_BLINK 200
#define TIME_READ 0

#define TOP_SENSOR A7
#define LEFT_SENSOR A11
#define RIGHT_SENSOR A1
#define BOT_SENSOR A3

int sensors[NUM_SENSORS] = {TOP_SENSOR,LEFT_SENSOR,RIGHT_SENSOR,BOT_SENSOR};

int LED_PIN[NUM_ROWS][NUM_COLS] = { 	{36,48,32,34,44},
										{46,52,29,38,40},
										{27,39,33,43,45},
										{31,42,51,41,50},
										{47,53,37,35,49}		};

int isLedOn[NUM_ROWS][NUM_COLS];

bool leds_on;

int time_now;
int time_before;
int time_elapsed;

int timer_blink;

int timer_read;

int max_light;

void testHardware()
{
	for(int i=0;i<NUM_ROWS;i++)
	{
		for(int j=0;j<NUM_COLS;j++)
		{
			digitalWrite(LED_PIN[i][j],HIGH);
			delay(50);
		}
	}
	for(int i=0;i<NUM_ROWS;i++)
	{
		for(int j=0;j<NUM_COLS;j++)
		{
			digitalWrite(LED_PIN[i][j],LOW);
			delay(50);
		}
	}
}

float distance(int x, int y, int sensor)
{
	float fx = (float) x;
	float fy = (float) y;
	float vx;
	float vy;
	if(sensor == TOP_SENSOR)
	{
		vx = fx + 1;
		vy = fy - 2;
	}
	else if(sensor == LEFT_SENSOR)
	{
		vx = fx - 2;
		vy = fy + 1;
	}
	else if(sensor == RIGHT_SENSOR)
	{
		vx = fx - 2;
		vy = fy - 5;
	}
	else if(sensor == BOT_SENSOR)
	{
		vx = fx - 5;
		vy = fy - 2;
	}
	else
	{
		return -1.0;
	}
	return (float) sqrt((vx*vx)+(vy*vy));
}

void setup()
{
	for(int i=0;i<NUM_ROWS;i++)
	{
		for(int j=0;j<NUM_COLS;j++)
		{
			pinMode(LED_PIN[i][j],OUTPUT);
		}
	}
	for(int i=0;i<NUM_SENSORS;i++)
	{
		pinMode(sensors[i],INPUT);
	}

	int sum=0;
	for(int i=0;i<NUM_SENSORS;i++)
	{
		sum += analogRead(sensors[i]);
	}
	max_light = (int) (MAX_LIGHT_FACTOR * (sum/4));

	for(int i=0;i<NUM_ROWS;i++)
	{
		for(int j=0;j<NUM_COLS;j++)
		{
			isLedOn[i][j] = HIGH;
		}
	}

	leds_on = true;
	timer_blink = TIME_BLINK;
	time_now = millis();

	testHardware();
}

void loop()
{
	time_before = time_now;
	time_now = millis();
	time_elapsed = time_now - time_before;

	timer_read -= time_elapsed;
	if(timer_read<0)
	{
		timer_read = TIME_READ;

		int reading;
		float light_percentage_read;

		for(int i=0;i<NUM_ROWS;i++)
		{
			for(int j=0;j<NUM_COLS;j++)
			{
				isLedOn[i][j] = HIGH;
			}
		}

		for(int k=0;k<NUM_SENSORS;k++)
		{
			reading = analogRead(sensors[k]);
			light_percentage_read = (float)reading/(float)max_light;
			for(int i=0;i<NUM_ROWS;i++)
			{
				for(int j=0;j<NUM_COLS;j++)
				{
					float normalized_distance = distance(i,j,sensors[k])*NORMALIZE_DISTANCE_FACTOR;
					if((1.0 - normalized_distance)>=light_percentage_read)
					{
						isLedOn[i][j] = LOW;
					}
				}
			}
		}
	}

/*
	timer_blink -= time_elapsed;
	if(timer_blink < 0)
	{
		timer_blink = TIME_BLINK;
		if(leds_on) leds_on = false;
		else leds_on = true;
	}


	for(int i=0;i<NUM_ROWS;i++)
	{
		for(int j=0;j<NUM_COLS;j++)
		{
			if(isLedOn[i][j] == HIGH)
			{
				if(leds_on)
				{
					digitalWrite(LED_PIN[i][j],HIGH);
				}
				else
				{
					digitalWrite(LED_PIN[i][j],LOW);
				}
			}
			else
			{
					digitalWrite(LED_PIN[i][j],LOW);
			}
		}
	}
*/
	for(int i=0;i<NUM_ROWS;i++)
	{
		for(int j=0;j<NUM_COLS;j++)
		{
			digitalWrite(LED_PIN[i][j],!isLedOn[i][j]);
		}
	}
}