#include "event_driven.h"
#include "callbacks.h"
#include "Arduino.h"
#define LED_PIN 13

void turnLedOn();
void turnLedOff();

void turnLedOn()
{
	digitalWrite(LED_PIN,HIGH);
	timer_set(500,turnLedOff);
}

void turnLedOff()
{
	digitalWrite(LED_PIN,LOW);
	timer_set(500,turnLedOn);
}

void initialize()
{
	pinMode(LED_PIN,OUTPUT);
	digitalWrite(LED_PIN,HIGH);
	delay(1000);
	digitalWrite(LED_PIN,LOW);
	delay(1000);
	digitalWrite(LED_PIN,HIGH);
	timer_set(500,turnLedOn);
}
