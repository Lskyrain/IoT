/*
	*디지털 도어락 만들기
	
	*사용 모듈
	4x4 keypad, buzzer, DC 서버모터
	
	*기능적 요구 사항
	1. 비밀번호 4자리 설정 방법
		#누르고→ 숫자4자리 입력 → #누르기
	2. 비밀번호 입력 방법
		숫자4자리 입력 → *누르기
	3. 만약 비번이 맞다면,
		경쾌한 소리와 함께 서보모터 90도 회전 → 2초 후 원상태로 복귀
	4. 비번이 틀리면
		특정 소리와 함께 화면에 메시지 출력
	5. 3번 이상 틀리면
		특정 소리와 함께 화면에 메시지 출력 → 10초 후부터 다시입력 가능
*/

#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <softTone.h>

#define KEYPAD_BP_1	2	// 1 button
#define KEYPAD_BP_2	3	// 2 button
#define KEYPAD_BP_3	4	// 3 button
#define KEYPAD_BP_5	17	// 4 button
#define KEYPAD_BP_6	27	// 5 button
#define KEYPAD_BP_7	22	// 6 button
#define KEYPAD_BP_9	10	// 7 button
#define KEYPAD_BP_10	9	// 8 button
#define KEYPAD_BP_11	11	// 9 button
#define KEYPAD_BP_13	5	// * button
#define KEYPAD_BP_14	6	// 0 button
#define KEYPAD_BP_15	13	// # button

#define MAX_KEY_BT_NUM 12

#define BUZZER_PIN 18

#define DO_L	523
#define RE	587
#define MI	659
#define FA	698
#define SOL	784
#define RA	880
#define SI	987
#define DO_H	1046

#define SERVO 24


const int KeypadTable[12] = {	KEYPAD_BP_1,KEYPAD_BP_2,KEYPAD_BP_3,
				KEYPAD_BP_5,KEYPAD_BP_6,KEYPAD_BP_7,
				KEYPAD_BP_9,KEYPAD_BP_10,KEYPAD_BP_11,
				KEYPAD_BP_13,KEYPAD_BP_14,KEYPAD_BP_15};
const char numpad[12] = {'1','2','3','4','5','6','7','8','9','*','0','#'};

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
void STOP_FREQ()
{
	softToneWrite(BUZZER_PIN,0);
}
void Buzzer_Init()
{
	softToneCreate(BUZZER_PIN);
	STOP_FREQ();
}
void Mcont(unsigned char str)
{
	switch(str)
	{
		case 'C':
			softPwmWrite(SERVO,15); // 0 degree
			break;
		case 'R':
			softPwmWrite(SERVO,25); // 90 degree
			break;
		case 'L':
			softPwmWrite(SERVO,5); // -90 degree
			break;
		case 'Q':
			break;
	}
}
int main()
{
	if(wiringPiSetupGpio() == -1)
		return 1;

	Buzzer_Init();
	softPwmCreate(SERVO,0,200);

	int nKeypadstate, i, j=0, inkey, pastkey=-1, set_cnt=0, er_cnt=0;
	char pw[4]={}, num[5]={};

	for(i=0;i<MAX_KEY_BT_NUM;i++)
		pinMode(KeypadTable[i],INPUT);

	while(1)
	{
		nKeypadstate = KeypadRead();
		inkey = -1;
		for(i=0;i<MAX_KEY_BT_NUM;i++)
			if((nKeypadstate & (1<<i)))
				inkey = i;
		
		// keypad가 눌렸을 때만 작동
		if((pastkey != inkey) && (inkey != -1))
		{
			num[j] = numpad[inkey];
			printf(" %c \n",num[j]);
			j++;
			if(set_cnt==1)
			{
				if(j==5)
				{
					if(inkey==11)
					{
						printf("password set complete:");
						for(i=0;i<4;i++)
							pw[i]=num[i];
						for(i=0;i<4;i++)
							printf(" %c",pw[i]);
						printf("\n");
						set_cnt=0;
						Change_FREQ(FA);
					}
					else
					{
						printf("password must be 4seat. \n");
						Change_FREQ(SI);
					}
					j=0;
					printf("\n");
				}
				else if(inkey==9)
				{
					printf("password change cancel #\n");
					j=0;
				}
				else if(inkey==11)
				{
					printf("reset password2\n");
					j=0;
					set_cnt=1;
				}
			}

			else if(j==5)
			{
				if(inkey==9)
				{
					for(i=0;i<4;i++)
						if(num[i]!=pw[i])
						{
							i=-1; er_cnt++;
							printf("password incorrect \n");
							Change_FREQ(FA);
							delay(100);
							STOP_FREQ();
							if(er_cnt == 3)
							{
								Change_FREQ(SOL);
								delay(500);
								STOP_FREQ();
								Change_FREQ(RA);
								delay(500);
								STOP_FREQ();
								Change_FREQ(SI);
								delay(500);
								STOP_FREQ();
								printf("password incorrect 3times. \n");
								for(i=0;i<10;i++)
								{
									delay(1000);
									printf("wait %d seconds \n", 10-i);
									Change_FREQ(RA);
									delay(100);
									STOP_FREQ();
								}
								delay(900);
								printf("Try again\n");
								Change_FREQ(RA);
								delay(100);
								STOP_FREQ();
								er_cnt=0;
							}
							break;
						}
					if(i==4)
					{
						j=0;
						printf("password correct \n");
						Change_FREQ(DO_H);
						delay(100);
						STOP_FREQ();
						Mcont('R');	
						delay(2000);
						Mcont('C');
						Change_FREQ(DO_H);
						delay(200);
					}
					else
						j--;
				}

				else if(inkey==11)
				{
					Change_FREQ(MI);
					printf("reset password\n");
					j=0;
					set_cnt=1;
				}
				else
				{
					for(i=0;i<4;i++)
						num[i]=num[i+1];
					j--;
				}
			}

			else if(inkey==11)
			{
				Change_FREQ(MI);
				printf("reset password\n");
				j=0;
				set_cnt=1;
			}
			delay(50);
		}
	
		else if((pastkey != inkey) && (inkey == -1))
			delay(50);

		pastkey=inkey;
		STOP_FREQ();
	}
	return 0;
}
