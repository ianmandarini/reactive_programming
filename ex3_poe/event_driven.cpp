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
	Serial.print("NEW TIMER FUNCTION ADDED -> ");
	Serial.print(time_stamp);
	Serial.println("ms");
	timerHeapPush(&timer_heap,time_stamp,function);
}

void initialize();

void setup()
{
	Serial.begin(9600);
	Serial.println("PROGRAM INITIALIZED");
	current_time = millis();
	timerHeapInitialize(&timer_heap);

	initialize();
}

void loop()
{
	current_time = millis();

	Timer* heap_top = timerHeapTop(&timer_heap);
	if(heap_top != NULL)
	{
		/*
		Serial.println("NOT NULL");
		Serial.print(timerGetTimeStamp(heap_top));
		Serial.print(" ");
		Serial.print(current_time);
		Serial.println("ms");
		*/
		while(current_time >= timerGetTimeStamp(heap_top))
		{
			timerRunFunction(heap_top);
			timerHeapPopTop(&timer_heap);
			Serial.println(timer_heap.size);
			heap_top = timerHeapTop(&timer_heap);
			if(heap_top == NULL) break;
		}
	}
	else
	{
		//Serial.println("NULL");
	}
}