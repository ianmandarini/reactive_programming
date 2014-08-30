#include "event_driven.h"
#include "timer_heap.h"
#include "Arduino.h"
#include "main.cpp"

TimerHeap timer_heap;
int current_time;

/*
void button_listen(int pin)
{

}
*/

void timer_set(int time_in_ms, void (*function)(void))
{
	int time_stamp = millis() + time_in_ms;
	timerHeapPush(&timer_heap,time_stamp,function);
}

void initialize();

void setup()
{
	current_time = millis();
	timerHeapInitialize(&timer_heap);

	initialize();
	Serial.begin(9600);
}

void loop()
{
	Serial.println("Loop");
	current_time = millis();

	Timer* heap_top = timerHeapTop(&timer_heap);
	while(timerGetTimeStamp(heap_top) >= current_time)
	{
		timerRunFunction(heap_top);
		timerHeapPopTop(&timer_heap);
		heap_top = timerHeapTop(&timer_heap);
	}
}