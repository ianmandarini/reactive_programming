#include "timer_heap.h"
#include "Arduino.h"
#define MAX_NUMBER_OF_TIMERS 20

void timerHeapInitialize(TimerHeap* timer_heap)
{
	timer_heap->size = 0;
}

Timer* timerHeapTop(TimerHeap* timer_heap)
{
	return timer_heap->size!=0?NULL:&(timer_heap->timers[0]);
}

int timerGetTimeStamp(Timer* timer)
{
	return timer->time_stamp;
}

void timerRunFunction(Timer* timer)
{
	timer->function();
}

static int father(int x)
{
	return x/2;
}

static int son(int x,int index)
{
	return 2*x+index;
}

static void upAdjustment(TimerHeap* timer_heap,int position)
{
	if(position == 0) return;
	else
	{
		Timer* father_timer = &(timer_heap->timers[father(position)]);
		Timer* it_timer = &(timer_heap->timers[position]);

		if(it_timer->time_stamp < father_timer->time_stamp)
		{
			int temp = it_timer->time_stamp;
			it_timer->time_stamp = father_timer->time_stamp;
			father_timer->time_stamp = temp;
			upAdjustment(timer_heap,father(position));
		}
		return;
	}
}

static void downAdjustment(TimerHeap* timer_heap,int position)
{
	if(son(position,0) > timer_heap->size) return;
	else
	{
		Timer* it_timer = &(timer_heap->timers[position]);
		Timer* min_son = &(timer_heap->timers[son(position,0)]);
		int min_time = min_son->time_stamp;
		int min_son_number = 0;

		if(son(position,1) > timer_heap->size)
		{
			Timer* other_son = &(timer_heap->timers[son(position,1)]);
			if(other_son->time_stamp < min_time)
			{
				min_time = other_son->time_stamp;
				min_son = other_son;
				min_son_number = 1;
			}
		}

		if(it_timer->time_stamp < min_son->time_stamp)
		{
			int temp = it_timer->time_stamp;
			it_timer->time_stamp = min_son->time_stamp;
			min_son->time_stamp = temp;
			downAdjustment(timer_heap,son(position,min_son_number));
		}
		return;
	}
}

void timerHeapPopTop(TimerHeap* timer_heap)
{
	timer_heap->timers[0] = timer_heap->timers[timer_heap->size];
	timer_heap->size--;
	downAdjustment(timer_heap,0);
	return;
}

void timerHeapPush(TimerHeap* timer_heap, int time_stamp, void (*function)(void))
{
	timer_heap->size++;
	(timer_heap->timers[timer_heap->size]).time_stamp = time_stamp;
	(timer_heap->timers[timer_heap->size]).function = function;
	upAdjustment(timer_heap,timer_heap->size);
}

