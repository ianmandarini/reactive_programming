#include "event_driven.h"
#include "Arduino.h"
#define RED_LED_PIN 13
#define YELLOW_LED_PIN 8

void turnRedLedOn();
void turnRedLedOff();

void turnRedLedOn()
{
	Serial.println("RED ON");
	digitalWrite(RED_LED_PIN,HIGH);
	timer_set(1000,turnRedLedOff);
}

void turnRedLedOff()
{
	Serial.println("RED OFF");
	digitalWrite(RED_LED_PIN,LOW);
	timer_set(1000,turnRedLedOn);
}

void turnYellowLedOn();
void turnYellowLedOff();

void turnYellowLedOn()
{
	Serial.println("YELLOW ON");
	digitalWrite(YELLOW_LED_PIN,HIGH);
	timer_set(300,turnYellowLedOff);
}

void turnYellowLedOff()
{
	Serial.println("YELLOW OFF");
	digitalWrite(YELLOW_LED_PIN,LOW);
	timer_set(300,turnYellowLedOn);
}

void testLed(int led_pin)
{
	digitalWrite(led_pin,HIGH);
	delay(1000);
	digitalWrite(led_pin,LOW);
	delay(200);
	digitalWrite(led_pin,HIGH);
	delay(1000);
	digitalWrite(led_pin,LOW);
}

void initialize()
{
	pinMode(RED_LED_PIN,OUTPUT);
	pinMode(YELLOW_LED_PIN,OUTPUT);

	testLed(RED_LED_PIN);
	testLed(YELLOW_LED_PIN);

	digitalWrite(RED_LED_PIN,HIGH);
	digitalWrite(YELLOW_LED_PIN,HIGH);

	timer_set(1000,turnRedLedOff);
	timer_set(300,turnYellowLedOff);
}
