#include <wiringPi.h>
#include <softPwm.h>

#define LED_PIN_1 5
#define LED_PIN_2 6
#define LED_PIN_3 13

#define KEYPAD_BP1 23
#define KEYPAD_BP2 24
#define KEYPAD_BP3 25
#define KEYPAD_BP4 8
#define KEYPAD_BP5 7

#define LED_ON  1
#define LED_OFF 0

#define MAX_LED_NUM    3
#define MAX_KEY_BT_NUM 5

#define MOTOR_MT_P_PIN 4
#define MOTOR_MT_N_PIN 17

#define LEFT_ROTATE  1
#define RIGHT_ROTATE 2

const int LedPinTable[3]={LED_PIN_1,LED_PIN_2,LED_PIN_3};
const int KeypadTable[5]={KEYPAD_BP1,KEYPAD_BP2,KEYPAD_BP3,KEYPAD_BP4,KEYPAD_BP5};
//int rotate = RIGHT_ROTATE;

int KeypadRead()
{
	int i, nKeypadstate=0;
	for(i=0;i<MAX_KEY_BT_NUM;i++)
	{
		if(!digitalRead(KeypadTable[i]))
			nKeypadstate |= (1<<i);
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
void MotorStop()
{
	softPwmWrite(MOTOR_MT_N_PIN,0);
	softPwmWrite(MOTOR_MT_P_PIN,0);
}
void MotorControl(unsigned char speed, unsigned char rotate)
{
	if(rotate == LEFT_ROTATE)
	{
		digitalWrite(MOTOR_MT_P_PIN,LOW);
		softPwmWrite(MOTOR_MT_N_PIN,speed);
	}
	else if(rotate == RIGHT_ROTATE)
	{
		digitalWrite(MOTOR_MT_N_PIN,LOW);
		softPwmWrite(MOTOR_MT_P_PIN,speed);
	}
}
int main()
{
	if(wiringPiSetupGpio()==-1)
		return 1;
	pinMode(MOTOR_MT_N_PIN,OUTPUT);
	pinMode(MOTOR_MT_P_PIN,OUTPUT);
	softPwmCreate(MOTOR_MT_N_PIN,0,100);
	softPwmCreate(MOTOR_MT_P_PIN,0,100);

	int i, nKeypadstate;
	int rotate, a=0;
	
	for(i=0;i<MAX_LED_NUM;i++)
		pinMode(LedPinTable[i],OUTPUT);
	for(i=0;i<MAX_KEY_BT_NUM;i++)
		pinMode(KeypadTable[i],INPUT);

	while(1)
	{
		nKeypadstate = KeypadRead();
		rotate = a;
		for(i=0;i<MAX_KEY_BT_NUM;i++)
		{
			if((nKeypadstate & (1<<i)))
			{	
				switch(i){
					case 0:
						digitalWrite(LED_PIN_1,LED_OFF);
						digitalWrite(LED_PIN_2,LED_OFF);
						digitalWrite(LED_PIN_3,LED_OFF);
						MotorStop();
						break;
					case 1:
						if(rotate == LEFT_ROTATE)
						{
							MotorStop();
							delay(2500);
							rotate=RIGHT_ROTATE;

						}
							digitalWrite(LED_PIN_1,LED_ON);
							digitalWrite(LED_PIN_2,LED_OFF);
							digitalWrite(LED_PIN_3,LED_OFF);
							MotorControl(15,rotate);
							a = RIGHT_ROTATE;
						break;
					case 2:
						if(rotate == LEFT_ROTATE)
						{
							MotorStop();
							delay(2500);
							rotate=RIGHT_ROTATE;

						}
						digitalWrite(LED_PIN_1,LED_ON);
						digitalWrite(LED_PIN_2,LED_ON);
						digitalWrite(LED_PIN_3,LED_OFF);
						MotorControl(50,rotate);
						a = RIGHT_ROTATE;
						break;
					case 3:
						if(rotate == LEFT_ROTATE)
						{
							MotorStop();
							delay(2500);
							rotate=RIGHT_ROTATE;

						}
						digitalWrite(LED_PIN_1,LED_ON);
						digitalWrite(LED_PIN_2,LED_ON);
						digitalWrite(LED_PIN_3,LED_ON);
						MotorControl(100,rotate);
						a = RIGHT_ROTATE;
						break;
					case 4:
						if(rotate == LEFT_ROTATE)
						{
							digitalWrite(LED_PIN_1,LED_ON);
							digitalWrite(LED_PIN_2,LED_OFF);
							digitalWrite(LED_PIN_3,LED_OFF);
							MotorStop();
							delay(2000);
							MotorControl(15,RIGHT_ROTATE);
							rotate = RIGHT_ROTATE;
							a = rotate;
						}							
						else
						{
							digitalWrite(LED_PIN_1,LED_ON);
							digitalWrite(LED_PIN_2,LED_ON);
							digitalWrite(LED_PIN_3,LED_OFF);
							MotorStop();
							delay(2000);
							MotorControl(50,LEFT_ROTATE);
							rotate = LEFT_ROTATE;
							a = rotate;
						}						
						break;
				}
			}
		}
	}
}
