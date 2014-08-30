#include "event_driven.h"
#include "Arduino.h"
#define LED_PIN 13

void turnLedOn();
void turnLedOff();

void turnLedOn()
{
	digitalWrite(LED_PIN,HIGH);
	Serial.println("TURN LED ON");
	timer_set(1000,turnLedOff);
}

void turnLedOff()
{
	digitalWrite(LED_PIN,LOW);
	Serial.println("TURN LED OFF");
	timer_set(500,turnLedOn);
}

void initialize()
{
	Serial.println("START INITIALIZATION");
	pinMode(LED_PIN,OUTPUT);

	digitalWrite(LED_PIN,HIGH);
	// Test LED
	digitalWrite(LED_PIN,HIGH);
	delay(1000);
	digitalWrite(LED_PIN,LOW);
	delay(200);
	digitalWrite(LED_PIN,HIGH);
	delay(200);
	digitalWrite(LED_PIN,LOW);
	delay(200);
	digitalWrite(LED_PIN,HIGH);
	delay(1000);
	digitalWrite(LED_PIN,LOW);
	delay(200);
	digitalWrite(LED_PIN,HIGH);

	Serial.println("SET TIMER");
	timer_set(500,turnLedOn);
	Serial.println("END INITIALIZATION");
}
