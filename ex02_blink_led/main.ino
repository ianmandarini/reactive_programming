#define BLINK_LED_PIN 13
#define BUTTON_UP_PIN 2
#define BUTTON_DOWN_PIN 4
#define BLINK_ON_LED_PIN 7
#define BUTTON_DOUBLE_LED_PIN 8

#define BLINK_STARTING_PERIOD 1000
#define BLINK_MAX_PERIOD 5000
#define BLINK_MIN_PERIOD 50
#define BLINK_STEP_FACTOR 1.1

#define BUTTON_RESPONSE_TIME 100

#define BUTTON_DOUBLE_TIME 500

//--------------------------------
// General Control Variables

int last_iteration_time;

boolean is_blink_loop_on;
boolean is_button_up_loop_on;
boolean is_button_down_loop_on;
boolean is_button_double_loop_on;

//--------------------------------
// Blink Variables

int blink_timer;
int blink_period;
boolean is_led_on;

//--------------------------------
// Button Variables

boolean button_up_last_state;
boolean button_down_last_state;
boolean can_button_double_happen;
int button_up_timer;
int button_down_timer;
int button_double_timer;

//--------------------------------
// Blink Functions

void setupBlink()
{
	pinMode(BLINK_LED_PIN, OUTPUT);
	pinMode(BLINK_ON_LED_PIN, OUTPUT);

	digitalWrite(BLINK_LED_PIN, LOW);
	is_led_on = false;

	digitalWrite(BLINK_ON_LED_PIN, HIGH);

	blink_timer = 0;
	blink_period = BLINK_STARTING_PERIOD;
}

void loopBlink(int time_elapsed)
{
	blink_timer+=time_elapsed;
	if(blink_timer>=blink_period)
	{
		if(is_led_on)
		{
			digitalWrite(BLINK_LED_PIN, LOW);
			is_led_on = false;
		}
		else
		{
			digitalWrite(BLINK_LED_PIN, HIGH);
			is_led_on = true;
		}
		blink_timer-=blink_period;
	}
}

//--------------------------------
// Button Up Functions

void setupButtonUp()
{
	pinMode(BUTTON_UP_PIN, INPUT);

 	// Turns on the pull-up resistor for the button pin
	digitalWrite(BUTTON_UP_PIN, HIGH);

	button_up_last_state = false;
	button_up_timer = 0;
}

void loopButtonUp(int time_elapsed)
{
	boolean button_up_current_state = !digitalRead(BUTTON_UP_PIN);
	if(button_up_current_state != button_up_last_state && button_up_timer >= BUTTON_RESPONSE_TIME)
	{
		if(button_up_current_state)
		{
			blink_period *= BLINK_STEP_FACTOR;
			if(!is_button_double_loop_on)
			{
				startButtonDouble();
			}
			if(blink_period > BLINK_MAX_PERIOD)
			{
				blink_period = BLINK_MAX_PERIOD;
			}
			Serial.println(blink_period);
		}
		button_up_last_state = button_up_current_state;
		button_up_timer = 0;
	}
	button_up_timer += time_elapsed;
}

//--------------------------------
// Button Down Functions

void setupButtonDown()
{
	pinMode(BUTTON_DOWN_PIN, INPUT);

 	// Turns on the pull-up resistor for the button pin
	digitalWrite(BUTTON_DOWN_PIN, HIGH);

	button_down_last_state = false;
	button_down_timer = 0;
}

void loopButtonDown(int time_elapsed)
{
	boolean button_down_current_state = !digitalRead(BUTTON_DOWN_PIN);
	if(button_down_current_state != button_down_last_state && button_down_timer >= BUTTON_RESPONSE_TIME)
	{
		if(button_down_current_state)
		{
			blink_period /= BLINK_STEP_FACTOR;
			if(!is_button_double_loop_on)
			{
				startButtonDouble();
			}
			if(blink_period < BLINK_MIN_PERIOD)
			{
				blink_period = BLINK_MIN_PERIOD;
			}
			Serial.println(blink_period);
		}
		button_down_last_state = button_down_current_state;
		button_down_timer = 0;
	}
	button_down_timer += time_elapsed;
}


//--------------------------------
// Button Double Functions

void setupButtonDouble()
{
	pinMode(BUTTON_DOUBLE_LED_PIN, OUTPUT);

}

void startButtonDouble()
{
	digitalWrite(BUTTON_DOUBLE_LED_PIN, HIGH);
	button_double_timer = 0;
	is_button_double_loop_on = true;
	can_button_double_happen = true;
}

void loopButtonDouble(int time_elapsed)
{
	if(!button_down_last_state && !button_up_last_state)
	{
		digitalWrite(BUTTON_DOUBLE_LED_PIN, LOW);
		is_button_double_loop_on = false;
	}
	else if(button_down_last_state && button_up_last_state && can_button_double_happen)
	{
		is_blink_loop_on = false;
		is_button_up_loop_on = false;
		is_button_down_loop_on = false;
		digitalWrite(BLINK_ON_LED_PIN, LOW);
		digitalWrite(BUTTON_DOUBLE_LED_PIN, LOW);
	}
	else
	{
		button_double_timer += time_elapsed;
	}

	if(button_double_timer > BUTTON_DOUBLE_TIME)
	{
		digitalWrite(BUTTON_DOUBLE_LED_PIN, LOW);
		can_button_double_happen = false;
	}
}

//--------------------------------
// General Functions

void setup()
{
	Serial.begin(9600);

	last_iteration_time = millis();

	setupBlink();
	setupButtonUp();
	setupButtonDown();
	setupButtonDouble();

	is_blink_loop_on = true;
	is_button_up_loop_on = true;
	is_button_down_loop_on = true;
	is_button_double_loop_on = false;
}

void loop()
{
	int time_elapsed = millis() - last_iteration_time;
	last_iteration_time += time_elapsed;

	if(is_blink_loop_on) loopBlink(time_elapsed);	
	if(is_button_up_loop_on) loopButtonUp(time_elapsed);
	if(is_button_down_loop_on) loopButtonDown(time_elapsed);
	if(is_button_double_loop_on) loopButtonDouble(time_elapsed);
}
