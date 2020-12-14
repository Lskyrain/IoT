#include <wiringPi.h>
#include <softTone.h>

#define BUZZER_PIN 18

#define DO_L	523
#define RE	587
#define MI	659
#define FA	698
#define SOL	784
#define RA	880
#define SI	987
#define DO_H	1046

unsigned int SevenScale(unsigned char scale)
{
	unsigned int _ret = 0;
	switch(scale)
	{
		case 0:
			_ret = DO_L;
			break;
		case 1:
			_ret = RE;
			break;
		case 2:
			_ret = MI;
			break;
		case 3:
			_ret = FA;
			break;
		case 4:
			_ret = SOL;
			break;
		case 5:
			_ret = RA;
			break;
		case 6:
			_ret = SI;
			break;
		case 7:
			_ret = DO_H;
			break;
	}
	return _ret;
}
void Change_FREQ(unsigned int freq)
{
	softToneWrite(BUZZER_PIN,freq);
}
void STOP_FREQ(void)
{
	softToneWrite(BUZZER_PIN,0);
}
void Buzzer_Init()
{
	softToneCreate(BUZZER_PIN);
	STOP_FREQ();
}

int main()
{
	if(wiringPiSetupGpio() == -1)
		return 1;

	Buzzer_Init();

	int i;

	for(i=0; i<8; i++)
	{
		Change_FREQ(SevenScale(i));
		delay(500);
		STOP_FREQ();
	}

	while(1){}

	return 0;
}
