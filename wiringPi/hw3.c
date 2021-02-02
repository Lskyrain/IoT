

#include <wiringPi.h>
#include <softPwm.h>
#include <softTone.h>
#include <lcd.h>

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

#define BUZZER_PIN 7

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
const char numpad[12] = {'1','2','3','4','5','6','7','8','9','>','0','E'};

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
	int disp;

	if(wiringPiSetupGpio() == -1)
		return 1;

	Buzzer_Init();
	softPwmCreate(SERVO,0,200);
	disp = lcdInit(2,16,4,18,23,12,16,20,21,0,0,0,0);
	
	lcdClear(disp);

	int nKeypadstate, i, j=0, inkey, pastkey=-1, er_cnt=0, cnt=0;
	char pw[1][5]={'0','0','0','0',}, num[6]={'*','*','*','*','*',};

	int input_mode=0, set_mode=0, change_mode=0, sound_mode=0, pg=0, k=0, c=0, s=1;
	char as_pw[1][5]={'*','*','*','*',}, be_pw[1][5]={'*','*','*','*',};

	for(i=0;i<MAX_KEY_BT_NUM;i++)
		pinMode(KeypadTable[i],INPUT);

	while(1)
	{
		nKeypadstate = KeypadRead();
		inkey = -1;
		
		for(i=0;i<MAX_KEY_BT_NUM;i++)
			if((nKeypadstate & (1<<i)))
				inkey = i;

		if((pastkey != inkey) && (inkey != -1))
		{
			if((pg==0) && (inkey==9))
				pg=1;
			else if((pg==1) && (inkey==9))
				pg=0;

			if((pg==0) && (inkey==0))
			{
				input_mode=1;
				pg=-1;
			}
			else if((pg==0) && (inkey==1))
			{
				set_mode=1;
				pg=-1;
			}
			else if((pg==1) && (inkey==2))
			{
				change_mode=1;
				pg=-1;
			}
			else if((pg==1) && (inkey==3))
			{
				sound_mode=1;
				pg=-1;
			}
			delay(50);
		}
		else if((pastkey != inkey) && (inkey == -1))
			delay(50);

		if((pastkey != inkey) && (inkey != -1))
		{
			SET:
			if(set_mode == 1)
			{
				lcdClear(disp);
				lcdPosition(disp,0,0);
				lcdPuts(disp,"2.Set PW");
				delay(50);

				num[j] = numpad[inkey];
				j++;								

				if(j==6)
				{
					if(inkey==11)
					{						
						set_mode=0;
						pg=0;
						j=0;

						lcdClear(disp);
						lcdPosition(disp,0,0);
						lcdPuts(disp,"Done");
						
						Change_FREQ(SI*s);
						delay(100);
						STOP_FREQ();

						for(i=0;i<4;i++)
							pw[0][i]=num[i+1];
						for(i=0;i<4;i++)
						{
							lcdPosition(disp,i,1);
							lcdPuts(disp,&pw[0][i]);
							delay(50);
						}

						delay(2000);

					}
					else
					{
						Change_FREQ(FA*s);
						delay(100);
						STOP_FREQ();

						lcdClear(disp);
						lcdPosition(disp,0,0);
						lcdPuts(disp,"Invalid password");
						delay(2000);
						j=0;
						goto SET;
					}

				}
				else if(inkey==9)
					j=1;
				else if(inkey==11)
					j=1;

			}

			else if(change_mode == 1)
			{
				lcdClear(disp);
				lcdPosition(disp,0,0);
				lcdPuts(disp,"3.Change PW");
				lcdPosition(disp,0,1);
				lcdPuts(disp,"Input past PW");
				delay(50);

				num[c] = numpad[inkey];
				c++;

				if(cnt==1)
				{
					if(c==6)
					{					
						if(inkey==11)
						{
							lcdClear(disp);
							lcdPosition(disp,0,0);
							lcdPuts(disp,"As is: ");
							
							lcdPosition(disp,0,1);
							lcdPuts(disp,"To be: ");	

							for(i=0;i<4;i++)
							{
								lcdPosition(disp,i+7,0);
								lcdPuts(disp,&as_pw[0][i]);
								delay(50);
							}	

							for(i=0;i<4;i++)
								be_pw[0][i]=num[i+1];
							for(i=0;i<4;i++)
								{
									lcdPosition(disp,i+7,1);
									lcdPuts(disp,&be_pw[0][i]);
									delay(50);
								}
								delay(2000);

							for(i=0;i<4;i++)
								pw[0][i]=be_pw[0][i];

								Change_FREQ(RA*s);
								delay(100);
								STOP_FREQ();

								lcdClear(disp);
								lcdPosition(disp,0,0);
								lcdPuts(disp,"Done");
								
								delay(2000);

								change_mode=0;
								pg=0;
								cnt=0;
								c=0;
						}
						else 
						{
							lcdClear(disp);
							lcdPosition(disp,0,0);
							lcdPuts(disp,"Invalid password");

							Change_FREQ(FA*s);
							delay(100);
							STOP_FREQ();

							delay(2000);
						}
						c=1;
					}
					else if(inkey==9)
						c=1;
					else if(inkey==11)
						c=1;

				}
				else if(c==6)
				{
					if(inkey==11)
					{
						for(i=0;i<4;i++)
							as_pw[0][i]=num[i+1];

						for(i=0;i<4;i++)
							if(pw[0][i] != as_pw[0][i])
							{
								i=-1; c=0;

								Change_FREQ(FA*s);
								delay(100);
								STOP_FREQ();

								lcdClear(disp);
								lcdPosition(disp,0,0);
								lcdPuts(disp,"Invalid password");

								delay(2000);

								lcdPosition(disp,0,1);
								lcdPuts(disp,"Retry: Press E");
								delay(50);

								break;
							}

						if(i==4)
						{					
							lcdClear(disp);	
							lcdPosition(disp,0,0);
							lcdPuts(disp,"Correct!");
							lcdPosition(disp,0,1);
							lcdPuts(disp,"Input New PW");

							Change_FREQ(SI*s);
							delay(100);
							STOP_FREQ();

							c=1;
							cnt=1;
						}

					}
				}

				delay(50);
			}

			else if(sound_mode == 1)
			{
				if(s==1)
				{
					s=0;
					sound_mode=0;
					pg=0;
					lcdClear(disp);
					lcdPosition(disp,0,0);
					lcdPuts(disp,"Sound off");
					delay(2000);
				}
				else if(s==0)
				{
					s=1;
					sound_mode=0;
					pg=0;
					lcdClear(disp);
					lcdPosition(disp,0,0);
					lcdPuts(disp,"Sound on");
					delay(2000);
				}
			}

			else if(input_mode == 1)
			{
				lcdClear(disp);
				lcdPosition(disp,0,0);
				lcdPuts(disp,"1.Input PW");
				delay(50);

				num[k]=numpad[inkey];
				k++;

				if(k==6)
				{
					if(inkey==11)
					{
						for(i=0;i<4;i++)
							if(pw[0][i] != num[i+1])
							{
								i=-1; er_cnt++;
								lcdClear(disp);
								lcdPosition(disp,0,0);
								lcdPuts(disp,"Invalid password");

								Change_FREQ(FA*s);
								delay(100);
								STOP_FREQ();

									if(er_cnt == 3)
									{

										lcdClear(disp);
										lcdPosition(disp,0,0);
										lcdPuts(disp,"No more!");

										lcdPosition(disp,0,1);
										lcdPuts(disp,"Retry after 10s");
										delay(100);

										Change_FREQ(SOL*s);
										delay(400);
										STOP_FREQ();
										Change_FREQ(RA*s);
										delay(400);
										STOP_FREQ();
										Change_FREQ(SI*s);
										delay(400);
										STOP_FREQ();

										for(i=0;i<10;i++)
										{
											delay(1000);
											Change_FREQ(RA*s);
											delay(100);
											STOP_FREQ();
										}
										delay(900);
										Change_FREQ(RE*s);
										delay(100);
										STOP_FREQ();

										er_cnt=0;
										input_mode=0;
										pg=0;
										k=0;
									}

								delay(2000);

								lcdPosition(disp,0,1);
								lcdPuts(disp,"Retry: press E");
								delay(50);								

								k=1;

								break;
							}
						if(i==4)
						{
							Change_FREQ(DO_H*s);
							delay(100);
							STOP_FREQ();
							Mcont('R');	

							lcdClear(disp);
							lcdPosition(disp,0,0);
							lcdPuts(disp,"Door opened");

							delay(2000);

							Change_FREQ(DO_H*s);
							delay(100);
							STOP_FREQ();
							Mcont('C');

							input_mode=0;	
							er_cnt=0;
							pg=0;
							k=0;
						}
						else
							k=1;
					}
					else if(inkey!=11)
					{
						k=1;
						lcdClear(disp);
						lcdPosition(disp,0,0);
						lcdPuts(disp,"Invalid password");
						delay(50);

						Change_FREQ(FA*s);
						delay(100);
						STOP_FREQ();

						delay(2000);

						lcdPosition(disp,0,1);
						lcdPuts(disp,"Retry: press E");
						delay(50);
					}

				
				}
				else if(inkey==9)
					k=1;
				else if(inkey==11)
					k=1;

				delay(50);
			}

		}
		else if((pastkey != inkey) && (inkey == -1))
			delay(50);		

		if(pg == 0)
		{
			lcdClear(disp);
			lcdPosition(disp,0,0);
			lcdPuts(disp,"1.Input PW");
			lcdPosition(disp,0,1);
			lcdPuts(disp,"2.Set PW       >");
			delay(100);
		}
		else if(pg == 1)
		{
			lcdClear(disp);
			lcdPosition(disp,0,0);
			lcdPuts(disp,"3.Change PW");
			lcdPosition(disp,0,1);
			lcdPuts(disp,"4.Sound on/off >");
			delay(100);
		}

		pastkey=inkey;
		STOP_FREQ();
	}

	return 0;
}
