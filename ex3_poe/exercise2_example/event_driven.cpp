#include "event_driven.h"
#include "timer_heap.h"
#include "Arduino.h"
#include "main.cpp"
#define MAX_NUMBER_OF_BUTTONS 30

TimerHeap timer_heap;
unsigned long current_time;

int button_pins[MAX_NUMBER_OF_BUTTONS];
void (*button_functions[MAX_NUMBER_OF_BUTTONS])(int,int);
int last_state[MAX_NUMBER_OF_BUTTONS];
int num_buttons = 0;

void button_listen(int pin,void (*function)(int,int))
{
	pinMode(pin,INPUT);
	digitalWrite(pin,HIGH);
	button_pins[num_buttons] = pin;
	button_functions[num_buttons] = function;
	last_state[num_buttons] = digitalRead(pin);
	num_buttons++;
}

void timer_set(unsigned long time_in_ms, void (*function)(void))
{
	unsigned long time_stamp = millis() + time_in_ms;
	timerHeapPush(&timer_heap,time_stamp,function);
}

void initialize();

void setup()
{
	current_time = millis();
	timerHeapInitialize(&timer_heap);
	initialize();
}

void timerLoop()
{
	Timer* heap_top = timerHeapTop(&timer_heap);
	Timer event_to_fire;
	if(heap_top != NULL)
	{
		timerCopy(&event_to_fire,heap_top);
		while(current_time >= timerGetTimeStamp(&event_to_fire))
		{
			timerHeapPopTop(&timer_heap);
			timerRunFunction(&event_to_fire);
			heap_top = timerHeapTop(&timer_heap);

			if(heap_top != NULL) 
			{
				timerCopy(&event_to_fire,heap_top);
			}
			else
			{
				break;
			}
		}
	}
}

void buttonLoop()
{
	int reading;
	for(int i=0;i<num_buttons;i++)
	{
		reading = digitalRead(button_pins[i]);
		if(reading!=last_state[i])
		{
			last_state[i] = reading;
			button_functions[i](button_pins[i],reading);
		}
	}
}

void loop()
{
	current_time = millis();

	timerLoop();
	buttonLoop();
}