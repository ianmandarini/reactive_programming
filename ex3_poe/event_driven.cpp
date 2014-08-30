#include "event_driven.h"
#include "timer_heap.h"
#include "Arduino.h"
#include "main.cpp"

TimerHeap timer_heap;
unsigned long current_time;

/*
void button_listen(int pin)
{

}
*/

void timer_set(unsigned long time_in_ms, void (*function)(void))
{
	unsigned long time_stamp = millis() + time_in_ms;
	Serial.print("NEW EVENT -> ");
	Serial.println(time_stamp);
	timerHeapPush(&timer_heap,time_stamp,function);
}

void initialize();

void setup()
{
	Serial.begin(9600);
	current_time = millis();
	timerHeapInitialize(&timer_heap);
	initialize();
}

void loop()
{
	current_time = millis();

	Timer* heap_top = timerHeapTop(&timer_heap);
	Timer event_to_fire;
	if(heap_top != NULL)
	{
		timerCopy(&event_to_fire,heap_top);
		while(current_time >= timerGetTimeStamp(&event_to_fire))
		{
			Serial.print("TIMESTAMP GOTTEN -> ");
			Serial.println(timerGetTimeStamp(&event_to_fire));

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