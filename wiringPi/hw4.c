/*
	*자동차 후방감지기 만들기
	
	*사용 모듈
	초음파 센서, buzzer, Text LCD
	
	*기능적 요구 사항
	아래 영상과 같은 시스템을 구현하라
	
	거리는 cm단위로 T-LCD에 출력
*/

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <softTone.h>
#include <lcd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BUZZER_PIN 18

#define FREQ 1500

#define CS_MCP3208	8
#define SPI_CHANNEL	0
#define SPI_SPEED	1000000

#define TP 12
#define EP 16

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
int ReadMcp3208ADC(unsigned adcChannel)
{
	unsigned char buff[3];
	int nAdcValue = 0;
	buff[0] = 0x06 | ((adcChannel & 0x07) >> 2);
	buff[1] = ((adcChannel & 0x07) << 6);
	buff[2] = 0x00;
	digitalWrite(CS_MCP3208, 0);
	wiringPiSPIDataRW(SPI_CHANNEL, buff, 3);
	buff[1] = 0x0F & buff[1];
	nAdcValue = (buff[1] << 8) | buff[2];
	digitalWrite(CS_MCP3208, 1);
	return nAdcValue;
}
float getDistance()
{
	float fDistance;
	int nStartTime, nEndTime;

	digitalWrite(TP,LOW);
	delayMicroseconds(2);
	digitalWrite(TP,HIGH);
	delayMicroseconds(10);
	digitalWrite(TP,LOW);

	while(digitalRead(EP)==LOW);
	nStartTime = micros();

	while(digitalRead(EP)==HIGH);
	nEndTime = micros();

	fDistance = (nEndTime - nStartTime)*0.034 / 2.;

	return fDistance;
}


int main()
{
	if(wiringPiSetupGpio() == -1)
	{
		fprintf(stdout, "Not start wiringPi: %s\n",strerror(errno));
		return 1;
	}
	if(wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED) == -1)
	{
		fprintf(stdout, "wiringPiSPISetup Failed: %s\n",strerror(errno));
		return 1;
	}

	Buzzer_Init();
	pinMode(TP,OUTPUT);
	pinMode(EP,INPUT);

	float fDistance;
	char dist[10];
	int disp;

	disp = lcdInit(2,16,4,2,4,17,27,22,10,0,0,0,0);

	while(1)
	{
		fDistance = getDistance();

		if((fDistance <= 100) && (fDistance > 70))
		{
			Change_FREQ(FREQ);
			delay(100);
			STOP_FREQ();
			delay(900);
		}
		else if((fDistance <= 70) && (fDistance > 50))
		{
			Change_FREQ(FREQ);
			delay(100);
			STOP_FREQ();
			delay(400);
		}
		else if((fDistance <= 50) && (fDistance > 30))
		{
			Change_FREQ(FREQ);
			delay(100);
			STOP_FREQ();
			delay(150);
		}
		else if(fDistance <= 30)
		{
			Change_FREQ(FREQ);
			delay(50);
			STOP_FREQ();
			delay(1);
		}

		sprintf(dist, "%f", fDistance);
		lcdClear(disp);
		lcdPosition(disp,0,0);
		lcdPuts(disp,dist);

	}

	return 0;
}
