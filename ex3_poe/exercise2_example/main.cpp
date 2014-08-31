#include "event_driven.h"
#include "Arduino.h"
#define LED_PIN 13
#define DOUBLE_BUTTON_LED_PIN 7
#define INCREASE_SPEED_PIN 4
#define DECREASE_SPEED_PIN 2
#define LED_CHANGE_RATE 1.1
#define MIN_SPEED 50
#define MAX_SPEED 3000
#define DOUBLE_BUTTON_TIME 500
#define DEBOUNCING_TIME 20

int led_speed = 1000;
bool blink_led = true;
bool increase_button_pressed = false;
bool decrease_button_pressed = false;
bool double_button_may_happen = false;
bool increase_button_debounce = false;
bool decrease_button_debounce = false;

void turnLedOn();
void turnLedOff();

void turnLedOn()
{
	digitalWrite(LED_PIN,HIGH);
	if(blink_led) timer_set(led_speed,turnLedOff);
}

void turnLedOff()
{
	digitalWrite(LED_PIN,LOW);
	if(blink_led) timer_set(led_speed,turnLedOn);
}

void disableDoubleButton()
{
	double_button_may_happen = false;
	digitalWrite(DOUBLE_BUTTON_LED_PIN,LOW);
}

void disableIncreaseButtonDebounce()
{
	increase_button_debounce = false;
}

void disableDecreaseButtonDebounce()
{
	decrease_button_debounce = false;
}

void increaseSpeedChanged(int pin, int state)
{
	if(!increase_button_debounce)
	{
		increase_button_debounce = true;
		timer_set(DEBOUNCING_TIME,disableIncreaseButtonDebounce);
		if(state == LOW)
		{
			increase_button_pressed = true;

			led_speed *= LED_CHANGE_RATE;
			if(led_speed > MAX_SPEED) led_speed = MAX_SPEED;

			if(increase_button_pressed && decrease_button_pressed && double_button_may_happen) 
			{
				digitalWrite(LED_PIN,HIGH);
				blink_led = false;
			}

			double_button_may_happen = true;
			digitalWrite(DOUBLE_BUTTON_LED_PIN,HIGH);
			timer_set(DOUBLE_BUTTON_TIME,disableDoubleButton);
		}
		else
		{
			increase_button_pressed = false;
			digitalWrite(DOUBLE_BUTTON_LED_PIN,LOW);	
		}
	}
}

void decreaseSpeedChanged(int pin, int state)
{
	if(!decrease_button_debounce)
	{
		decrease_button_debounce = true;
		timer_set(DEBOUNCING_TIME,disableDecreaseButtonDebounce);
		if(state == LOW)
		{
			decrease_button_pressed = true;

			led_speed /= LED_CHANGE_RATE;
			if(led_speed < MIN_SPEED) led_speed = MIN_SPEED;

			if(increase_button_pressed && decrease_button_pressed && double_button_may_happen) 
			{
				digitalWrite(LED_PIN,HIGH);
				blink_led = false;
			}

			double_button_may_happen = true;
			digitalWrite(DOUBLE_BUTTON_LED_PIN,HIGH);
			timer_set(DOUBLE_BUTTON_TIME,disableDoubleButton);
		}
		else
		{
			decrease_button_pressed = false;
			digitalWrite(DOUBLE_BUTTON_LED_PIN,LOW);
		}
	}
}

void initialize()
{
	pinMode(LED_PIN,OUTPUT);
	pinMode(DOUBLE_BUTTON_LED_PIN,OUTPUT);
	timer_set(led_speed,turnLedOn);
	button_listen(INCREASE_SPEED_PIN,increaseSpeedChanged);
	button_listen(DECREASE_SPEED_PIN,decreaseSpeedChanged);
}
