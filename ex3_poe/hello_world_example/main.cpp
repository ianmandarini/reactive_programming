#include "event_driven.h"
#include "Arduino.h"
#define LED_PIN 13

void turnLedOn();
void turnLedOff();

void turnLedOn()
{
	digitalWrite(LED_PIN,HIGH);
	timer_set(1000,turnLedOff);
}

void turnLedOff()
{
	digitalWrite(LED_PIN,LOW);
	timer_set(1000,turnLedOn);
}

void initialize()
{
	pinMode(LED_PIN,OUTPUT);
	timer_set(1000,turnLedOn);
}
