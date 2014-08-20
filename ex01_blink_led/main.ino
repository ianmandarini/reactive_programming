#define LED_PIN 13
#define BUTTON_PIN 2
#define BLINK_PERIOD 500

//--------------------------------
// General Control Variables

int last_iteration_time;

boolean is_blink_loop_on;
boolean is_button_loop_on;

//--------------------------------
// Blink Variables

int blink_timer;
boolean is_led_on;

//--------------------------------
// Blink Functions

void setupBlink()
{
	pinMode(LED_PIN, OUTPUT);

	digitalWrite(LED_PIN, LOW);
	is_led_on = false;

	blink_timer = 0;
}

void loopBlink(int time_elapsed)
{
	blink_timer+=time_elapsed;
	if(blink_timer>=BLINK_PERIOD)
	{
		if(is_led_on)
		{
			digitalWrite(LED_PIN, LOW);
			is_led_on = false;
		}
		else
		{
			digitalWrite(LED_PIN, HIGH);
			is_led_on = true;
		}
		blink_timer-=BLINK_PERIOD;
	}
}

//--------------------------------
// Button Functions

void setupButton()
{
	pinMode(BUTTON_PIN, INPUT);

 	// Turns on the pull-up resistor for the button pin
	digitalWrite(BUTTON_PIN, HIGH);
}

void loopButton(int time_elapsed)
{
	boolean is_button_pressed = !digitalRead(BUTTON_PIN);
	if(is_button_pressed)
	{
		digitalWrite(LED_PIN, HIGH);
		is_led_on = true;
		is_blink_loop_on = false;
		is_button_loop_on = false;
	}
}

//--------------------------------
// General Functions

void setup()
{
	last_iteration_time = millis();

	setupBlink();
	setupButton();

	is_blink_loop_on = true;
	is_button_loop_on = true;
}

void loop()
{
	int time_elapsed = millis() - last_iteration_time;
	last_iteration_time+=time_elapsed;

	if(is_blink_loop_on) loopBlink(time_elapsed);	
	if(is_button_loop_on) loopButton(time_elapsed);
}
