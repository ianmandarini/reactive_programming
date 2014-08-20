#define LED_PIN 13
#define BUTTON_PIN 2

void setup()
{
	pinMode(LED_PIN, OUTPUT);
	pinMode(BUTTON_PIN, INPUT);
	digitalWrite(BUTTON_PIN, HIGH); // Turns on the pull-up resistor for the button pin
}

void loop()
{
	int is_button_pressed = !digitalRead(BUTTON_PIN);
	digitalWrite(LED_PIN, is_button_pressed);
}
