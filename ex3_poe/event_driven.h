#ifndef EVENT_DRIVEN_H
#define EVENT_DRIVEN_H

void timer_set(unsigned long time_in_ms, void (*function)(void));
void initialize();

#endif