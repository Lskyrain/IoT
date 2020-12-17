#include <wiringPi.h>

#define LED_PIN_1 5
#define LED_PIN_2 6

#define KEYPAD_PB_1 23
#define KEYPAD_PB_2 24

#define LED_ON 1
#define LED_OFF 0

#define MAX_LED_NUM 2
#define MAX_KEY_BT_NUM 2

const int LedPinTable[2]={LED_PIN_1,LED_PIN_2};
const int KeypadTable[2]={KEYPAD_PB_1,KEYPAD_PB_2};

int KeypadRead(void)
{
	int i,nKeypadstate=0;

	for(i=0;i<MAX_KEY_BT_NUM;i++)
	{
		if(!digitalRead(KeypadTable[i]))
		{
			nKeypadstate |= (1<<i);
		}
	}
	return nKeypadstate;
}
void LedControl(int LedNum, int Cmd)
{
	int i;

	for(i=0;i<MAX_LED_NUM;i++)
	{
		if(i==LedNum)
		{
			if(Cmd==LED_ON)
			digitalWrite(LedPinTable[i],HIGH);
			else
			digitalWrite(LedPinTable[i],LOW);
		}
	}
}
int main()
{
	if(wiringPiSetupGpio() == -1)
		return 1;

	int i, nKeypadstate;

	for(i=0;i<MAX_LED_NUM;i++)
		pinMode(LedPinTable[i],OUTPUT);
	for(i=0;i<MAX_KEY_BT_NUM;i++)
		pinMode(KeypadTable[i],INPUT);

	while(1)
	{
		nKeypadstate = KeypadRead();

		for(i=0;i<MAX_KEY_BT_NUM;i++)
		{
			if((nKeypadstate & (1<<i)))
				LedControl(i,LED_ON);
			else
				LedControl(i,LED_OFF);
		}
	}
	return 0;
}
