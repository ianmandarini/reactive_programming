#include "event_driven.h"
#include "Arduino.h"
#define RED_LED_PIN 13
#define YELLOW_LED_PIN 8
#define BLUE_BUTTON_PIN 4
#define WHITE_BUTTON_PIN 2

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

void testLeds()
{
	for(int i=0;i<10;i++)
	{
		digitalWrite(RED_LED_PIN,HIGH);
		digitalWrite(YELLOW_LED_PIN,HIGH);
		delay(100);
		digitalWrite(RED_LED_PIN,LOW);
		digitalWrite(YELLOW_LED_PIN,LOW);
		delay(100);
	}
}

void blueButton(int pin, int state)
{
	if(state == LOW)
	{
		Serial.println("LOW");
	}
	else
	{		
		Serial.println("HIGH");
	}
}

void whiteButton(int pin, int state)
{
	if(state == LOW)
	{
		Serial.println("LOW");
	}
	else
	{		
		Serial.println("HIGH");
	}
}

void initialize()
{
	Serial.println("Initialized");
	pinMode(RED_LED_PIN,OUTPUT);
	pinMode(YELLOW_LED_PIN,OUTPUT);

	pinMode(BLUE_BUTTON_PIN,INPUT);
	pinMode(WHITE_BUTTON_PIN,INPUT);
	digitalWrite(BLUE_BUTTON_PIN,HIGH);
	digitalWrite(WHITE_BUTTON_PIN,HIGH);

	testLeds();

	digitalWrite(RED_LED_PIN,HIGH);
	digitalWrite(YELLOW_LED_PIN,HIGH);

	button_listen(BLUE_BUTTON_PIN,blueButton);
	button_listen(WHITE_BUTTON_PIN,whiteButton);
	timer_set(1000,turnRedLedOn);
	timer_set(300,turnYellowLedOn);
}
