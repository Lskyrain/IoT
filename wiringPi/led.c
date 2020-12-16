#include <wiringPi.h>

#define LED_RED_1 5
#define LED_RED_2 6

int main()
{
	if(wiringPiSetupGpio() == -1)
		return 1;

	pinMode(LED_RED_1,OUTPUT); //LEDs PORT SET
	pinMode(LED_RED_2,OUTPUT);
	digitalWrite(LED_RED_1,LOW);
	digitalWrite(LED_RED_2,LOW);

	while(1)
	{
		digitalWrite(LED_RED_1,HIGH);
		digitalWrite(LED_RED_2,LOW);
		delay(500);
		digitalWrite(LED_RED_1,LOW);
		digitalWrite(LED_RED_2,HIGH);
		delay(500);
	}
	return 0;
}
