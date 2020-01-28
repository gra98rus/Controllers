#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define led0 0
#define led1 1
#define led2 2
#define led3 3

#define num0 2
#define num1 3
#define num2 4
#define num3 5

#define ONOFFREG PORTD
#define NUMREG PORTB

#define BUTTONREG PORTC
#define BUTTONPIN PINC
#define BUTTON1_PIN 0
#define BUTTON2_PIN 2

#define POINT_REG PORTB
#define POINT1_PIN 4
#define POINT2_PIN 5

#define SUMMER_REG PORTC
#define SUMMER_PIN 3

#define SIZE_BUF 16

volatile uint8_t hours = 18;
volatile uint8_t mins = 33;
volatile uint8_t seconds = 0;

volatile uint8_t day = 11;
volatile uint8_t mounth = 12;
volatile uint16_t year = 2019;

volatile uint8_t isAlarm = 0;

volatile uint8_t dayOfWen = 3;
volatile uint8_t temperature = 0;

volatile uint8_t startStopwatch = 0;
volatile uint8_t stopwatchMSec = 0;
volatile uint8_t stopwatchSec = 0;
volatile uint8_t stopwatchMin = 0;
volatile uint8_t stopwatchHour = 0;
volatile uint8_t stopwatchMode = 0;  //0 - min-sec-msec; 1- hour-min-sec

volatile int settingAlarmMode = -1;
volatile uint8_t alarmOnOff = 0;
volatile int alarmSec = -1;
volatile int alarmMin = -1;
volatile int alarmHour = -1;

volatile uint8_t currentLed = 0;

volatile uint8_t mode = 0;   //0 - time, 1 - date, 2 - dayOfWen-temperature, 3 - stopwatch, 4 - alarm
volatile uint8_t setting = 0;
volatile uint8_t settingMode = 0; //0 - sec, 1 - min; 2 - h, 3 - year, 4 - mounth, 5 - day, 6 - dayOfWen
volatile int command = 0;  //h - hours, m - min, s - sec, y - year, M - mounth, d - day, t - mode, w - dayOfWen

volatile uint8_t settingStrob = 0;
volatile uint8_t strobDelay = 0;

volatile uint8_t uartBuf[SIZE_BUF];
volatile uint8_t bufTail = 0;
volatile uint8_t bufHead = 0;
volatile uint8_t bufCount = 0;


void i2cInit(){
	TWBR = 5;
	TWSR = (0<<TWPS1)|(0<<TWPS0);
}

void i2cStop(){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void i2cStart(){
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));}

char i2cReciveLast() {
	TWCR = (1<<TWINT|1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

char i2cRecive(){
	TWCR = (1<<TWINT|1<<TWEN|1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

void i2cSend(char send){
	TWDR = send;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
}

uint8_t ds3231_byte(uint8_t data){
	uint8_t temp = 0;
	while(data > 9){
		data -= 10;
		temp++;
	}
	return (data | (temp << 4));
}

void ds3231_write_time(uint8_t hour, uint8_t min, uint8_t sec){
	i2cStart();
	i2cSend(0xD0);
	i2cSend(0x00);
	i2cSend(ds3231_byte(sec));
	i2cSend(ds3231_byte(min));
	i2cSend(ds3231_byte(hour));
	i2cStop();
}

void ds3231_write_hour(uint8_t hour){
	i2cStart();
	i2cSend(0xD0);
	i2cSend(0x00);
	i2cSend(ds3231_byte(hour));
	i2cStop();
}
void ds3231_write_mins(uint8_t min){
	i2cStart();
	i2cSend(0xD0);
	i2cSend(0x00);
	i2cSend(ds3231_byte(min));
	i2cStop();
}

void ds3231_write_date(uint8_t year, uint8_t month, uint8_t date, uint8_t day){
	i2cStart();
	i2cSend(0xD0);
	i2cSend(0x03);
	i2cSend(ds3231_byte(day));
	i2cSend(ds3231_byte(date));
	i2cSend(ds3231_byte(month));
	i2cSend(ds3231_byte(year));
	i2cStop();
}

void ds3231_read_time(){
	uint8_t temp[3];
	
	i2cStart();
	i2cSend(0xD0);
	i2cSend(0x00);
	i2cStop();
	i2cStart();
	i2cSend(0xD1);
	temp[0] = i2cRecive();
	temp[1] = i2cRecive();
	temp[2] = i2cReciveLast();
	i2cStop();
	
	seconds = ((temp[0] & 0x0F)+((temp[0] >> 4) * 10));
	mins = ((temp[1] & 0x0F)+((temp[1] >> 4) * 10));
	hours = ((temp[2] & 0x0F)+((temp[2] >> 4) * 10));
}

void ds3231_read_date(){
	uint8_t temp[4];
	
	i2cStart();
	i2cSend(0xD0);
	i2cSend(0x03);
	i2cStop();
	i2cStart();
	i2cSend(0xD1);
	temp[0] = i2cRecive();
	temp[1] = i2cRecive();
	temp[2] = i2cRecive();
	temp[3] = i2cReciveLast();
	i2cStop();

	dayOfWen = temp[0];
	day = ((temp[1] & 0x0F)+(((temp[1] >> 4) & 0x07) * 10));
	mounth = ((temp[2] & 0x0F)+(((temp[2] >> 4) & 0x07) * 10));
	year = ((temp[3] & 0x0F)+(((temp[3] >> 4) & 0x07) * 10));
}

void ds3231_read_temp(){	
	i2cStart();
	i2cSend(0xD0);
	i2cSend(0x11);
	i2cStop();
	i2cStart();
	i2cSend(0xD1);
	temperature = i2cReciveLast();
	i2cStop();
}

void onOffPoint(uint8_t cs, uint8_t state){
	if (state) POINT_REG |= (1<<POINT1_PIN + cs);
	else  POINT_REG &= ~(1<<POINT1_PIN + cs);
}

uint8_t getPrintedFour(uint16_t value){
	uint8_t temp = 0;
	if(currentLed == 0)
		temp = value % 10;
	else if(currentLed == 1)
		temp = (value / 10) % 10;
	else if(currentLed == 2)
		temp = (value / 100) % 10;
	else if(currentLed == 3)
		temp = value / 1000;
	else if(currentLed == 4)
		ONOFFREG &= (0<<currentLed+2);
	else if(currentLed == 5)
		ONOFFREG &= (0<<currentLed+2);
	return temp;
}

uint8_t getPrintedTwoOfTwo(uint8_t value1, uint8_t value2){
	uint8_t temp = 0;
	if(currentLed == 0)
		temp = value2 % 10;
	else if(currentLed == 1)
		temp = value2 / 10;
	else if(currentLed == 2)
		temp = value1 % 10;
	else if(currentLed == 3)
		temp = value1 / 10;
	else if(currentLed == 4)
		ONOFFREG &= (0<<currentLed+2);
	else if(currentLed == 5)
		ONOFFREG &= (0<<currentLed+2);
		
	else if(currentLed == 6)
	onOffPoint(0, 0);
	else if(currentLed == 7)
	onOffPoint(1, 1);
	
	return temp;
}

uint8_t getPrintedThreeOfTwo(int value1, int value2, int value3){
	uint8_t temp = 0;
	if(currentLed == 0){
		if (value3 == -1) ONOFFREG &= (0<<currentLed+2);
		else temp = value3 % 10;
	}
	else if(currentLed == 1){
		if (value3 == -1) ONOFFREG &= (0<<currentLed+2);
		else temp = value3 / 10;
	}
	else if(currentLed == 2){
		if (value2 == -1) ONOFFREG &= (0<<currentLed+2);
		else temp = value2 % 10;
	}
	else if(currentLed == 3){
		if (value2 == -1) ONOFFREG &= (0<<currentLed+2);
		else temp = value2 / 10;
	}
	else if(currentLed == 4){
		if (value1 == -1) ONOFFREG &= (0<<currentLed+2);
		else temp = value1 % 10;
	}
	else if(currentLed == 5){
		if (value1 == -1) ONOFFREG &= (0<<currentLed+2);
		else temp = value1 / 10;
	}

	else if(currentLed == 6)
		onOffPoint(0, 1);
	else if(currentLed == 7)
		onOffPoint(1, 1);
	
	return temp;
}

ISR(TIMER0_OVF_vect){
		cli();
		
 		if (!settingStrob) settingStrob = ~settingStrob;
		else if (strobDelay > 1){
			settingStrob = ~settingStrob;
			strobDelay = 0;
		}
		else strobDelay++;
		
		sei();
		return;
}

ISR(TIMER2_OVF_vect){
	cli();

	ONOFFREG &= 0b00000011;
	NUMREG &= 0b11000000;
	if(currentLed < 6)
		if ((setting) && (settingStrob)){
			if(((settingMode == 0) || (settingMode == 3)) && ((currentLed == 0) || (currentLed == 1))) ONOFFREG |= (1<<currentLed+2);
			else if(((settingMode == 1) || (settingMode == 4) || (settingMode == 6)) && ((currentLed == 2) || (currentLed == 3))) ONOFFREG |= (1<<currentLed+2);
			else if(((settingMode == 2) || (settingMode == 5)) && ((currentLed == 4) || (currentLed == 5))) ONOFFREG |= (1<<currentLed+2);
		}
		else if((mode == 4) && (settingAlarmMode != -1) && (settingStrob)){
			if((settingAlarmMode == 0) && ((currentLed == 0) || (currentLed == 1))) ONOFFREG |= (1<<currentLed+2);
			if((settingAlarmMode == 1) && ((currentLed == 2) || (currentLed == 3))) ONOFFREG |= (1<<currentLed+2);
			if((settingAlarmMode == 2) && ((currentLed == 4) || (currentLed == 5))) ONOFFREG |= (1<<currentLed+2);
		}
		else if((mode == 4) && (!alarmOnOff) && (settingStrob))
			ONOFFREG &= ~(1<<currentLed+2);
		else ONOFFREG |= (1<<currentLed+2);
	if	   (mode == 0)	NUMREG |= getPrintedThreeOfTwo(hours, mins, seconds);
	else if(mode == 1)	NUMREG |= getPrintedThreeOfTwo(day, mounth, year);
	else if(mode == 2)	NUMREG |= getPrintedTwoOfTwo(dayOfWen, temperature);
	else if(mode == 3){
		if(stopwatchMode)
			NUMREG |= getPrintedThreeOfTwo(stopwatchHour, stopwatchMin, stopwatchSec);
		else
			NUMREG |= getPrintedThreeOfTwo(stopwatchMin, stopwatchSec, stopwatchMSec);
	}
	else if(mode == 4) NUMREG |= getPrintedThreeOfTwo(alarmHour, alarmMin, alarmSec);

	currentLed++;
	if(currentLed == 8)
		currentLed = 0;
		
	TCNT2 = 200;
	sei();
	return;
}

int power(int x, int y) {
	int i;
	int rez = 1;
	
	for(i = 0; i < y; ++i) {
		rez *= x;
	}
	return rez;
}

ISR(TIMER1_OVF_vect){
	cli();
	ds3231_read_time();
	ds3231_read_date();
	ds3231_read_temp();
	if (startStopwatch)
		stopwatchMSec++;
	if (stopwatchMSec == 100){
		stopwatchMSec = 0;
		stopwatchSec++;
		if (stopwatchSec == 60){
			stopwatchSec = 0;
			stopwatchMin++;
			if (stopwatchMin == 60){
				stopwatchMin = 0;
				stopwatchMode = 1;
				stopwatchHour++;
				if (stopwatchHour == 100){
					stopwatchHour = 0;
					stopwatchMode = 0;
				}
			}
		}
	}
	if ((alarmOnOff) && (hours == alarmHour) && (mins == alarmMin) && (seconds == alarmSec))
		isAlarm = 1;
	if((mode == 3) || (startStopwatch))   TCNT1 = 46145;  //46140-46150
	else TCNT1 = 0;
		
	sei();
	return;
}


ISR( USART_RX_vect ){
	cli();
	int temp = UDR0;
	if (bufCount < SIZE_BUF){                   
		 uartBuf[bufTail] = temp;        
		 bufCount++;                             
		 bufTail++;  
		 if (bufTail == SIZE_BUF) bufTail = 0;
	}                                  
	sei();
	return;
}

void uartInit(){
	UBRR0H = 0;
	UBRR0L = 104;
	UCSR0A = (1<<U2X0);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|1<<RXCIE0|0<<TXCIE0;
	UCSR0C = (0<<USBS0)|(3<<UCSZ00);
	sei();
}

void uartTransmit(char data){
	while (!(UCSR0A & (1<<UDRE0))){};
	UDR0 = data;
}

void uartSendStr(char str[]){
	unsigned char i = 0;

	while (str[i]!='\0')
	{
		uartTransmit(str[i]);
		i++;
	}
	uartTransmit('\n');
}

void incMode(){
	if(mode != 4)
		mode++;
	else mode = 0;
}

void incSettingMode(){
	if(settingMode == 6) settingMode = 0;
	else settingMode++;
	if(settingMode < 3) mode = 0;
	else if (settingMode < 6) mode = 1;
	else if (settingMode == 6) mode = 2;
}

void incTime(){
		if(settingMode == 0){
			if(seconds != 59) ds3231_write_time(hours, mins, seconds + 1);
			else ds3231_write_time(hours, mins, 0);
		}
		else if(settingMode == 1){
			if(mins != 59) ds3231_write_time(hours, mins + 1, seconds);
			else ds3231_write_time(hours, 0, seconds);
		}
		else if(settingMode == 2) {
			if(hours != 23) ds3231_write_time(hours + 1, mins, seconds);
			else ds3231_write_time(0, mins, hours);
		}
		else if(settingMode == 3){
			if(year != 99) ds3231_write_date(year + 1, mounth, day, dayOfWen);
			else ds3231_write_date(0, mounth, day, dayOfWen);
		}
		else if(settingMode == 4){
			if(mounth != 12) ds3231_write_date(year, mounth + 1, day, dayOfWen);
			else ds3231_write_date(year, 1, day, dayOfWen);
		}
		else if(settingMode == 5){
			 if(((mounth == 1) || (mounth == 3) || (mounth == 5) || (mounth == 7) || (mounth == 8) || (mounth == 10) || (mounth == 12)) && (day == 31))
				ds3231_write_date(year, mounth, 1, dayOfWen);
			else if (((mounth == 4) || (mounth == 6) || (mounth == 9) || (mounth == 11)) && (day == 30))
				ds3231_write_date(year, mounth, 1, dayOfWen);
			else if ((mounth == 2) && (year % 4 == 0) && (day == 29))
				ds3231_write_date(year, mounth, 1, dayOfWen);
			else if ((mounth == 2) && (year % 4 != 0) && (day == 28))
				ds3231_write_date(year, mounth, 1, dayOfWen);
			else ds3231_write_date(year, mounth, day + 1, dayOfWen);

		}
		else if(settingMode == 6){
			if(dayOfWen != 7) ds3231_write_date(year, mounth, day, dayOfWen + 1);
			else ds3231_write_date(year, mounth, day, 1);
		}
}

void incAlarm(){
	if(settingAlarmMode == 0){
		if(alarmSec == 59) alarmSec = 0;
		else alarmSec++;
	}
	else if(settingAlarmMode == 1){
		if(alarmMin == 59) alarmMin = 0;
		else alarmMin++;
	}
	else if(settingAlarmMode == 2) {
		if(alarmHour == 23) alarmHour = 0;
		else alarmHour++;
	}
}

void beep(){
	SUMMER_REG |= (1<<SUMMER_PIN);
	_delay_ms(1);
	SUMMER_REG &= ~(1<<SUMMER_PIN);
}

void alarm(){
	while(isAlarm){
		SUMMER_REG |= (1<<SUMMER_PIN);
		_delay_ms(700);
		SUMMER_REG &= ~(1<<SUMMER_PIN);
		_delay_ms(1000);
		SUMMER_REG |= (1<<SUMMER_PIN);
		_delay_ms(700);
		SUMMER_REG &= ~(1<<SUMMER_PIN);
		_delay_ms(5000);
		if((BUTTONPIN & (1<<BUTTON1_PIN)) || (BUTTONPIN & (1<<BUTTON2_PIN))){
			isAlarm = 0;
			while(BUTTONPIN & (1<<BUTTON1_PIN)){}
			while(BUTTONPIN & (1<<BUTTON2_PIN)){}
			_delay_ms(500);
		}
	}
}

int main(void)
{
	uint16_t longClickCount = 0;
	int temp = 0;
	
	uint32_t resetModeCount = 0;
	uint32_t resetSettingCount = 0;
	
	uint32_t resetAlarmCount = 0;
	
	DDRD = 255;
	PORTD = 0;
	
	DDRB = 0b00111111;
	PORTB = 0;
	
	DDRC = 0b11111000;
	
	TCCR0B = 3 << CS00;
	TIMSK0 = 1 << TOIE0;
		
	TCCR1B = 2 << CS10;
	TIMSK1 = 1 << TOIE1;
	
	TCCR2B = 6 << CS20;
	TIMSK2 = 1 << TOIE2;
	TCNT2 = 250;
	
	uartInit();
	i2cInit();
	
	//ds3231_write_time(18, 33, 0);
	//ds3231_write_date(19, 12, 11, 3);
	
	sei();
    while (1) 
    {
		alarm();
		if (mode != 0) resetModeCount++;
		if (resetModeCount == 8000000) {
			mode = 0;
			resetModeCount = 0;
		}
		if(BUTTONPIN & (1<<BUTTON1_PIN)){   
			resetModeCount = 0;
			beep();
			incMode();			//   inc mode
			while(BUTTONPIN & (1<<BUTTON1_PIN)){}
			_delay_ms(100);
		}
		
		if(BUTTONPIN & (1<<BUTTON2_PIN)){
			beep();
			resetModeCount = 0;
			if(mode == 3){			//   start/stop stopwatch
				startStopwatch = ~startStopwatch;
				while(BUTTONPIN & (1<<BUTTON2_PIN)){
					_delay_ms(200);
					longClickCount++;
					if(longClickCount > 200){
						startStopwatch = 0;
						stopwatchMin = 0;
						stopwatchSec = 0;
						stopwatchMSec = 0;
						stopwatchHour = 0;
						stopwatchMode = 0;
					}
				}
				longClickCount = 0;
			}
			else if(mode == 4){			//   setting alarm
				if((alarmHour == -1) || (alarmMin == -1) || (alarmSec == -1)){
					alarmHour = 0;
					alarmMin = 0;
					alarmSec = 0;
				}
				while(BUTTONPIN & (1<<BUTTON2_PIN))  longClickCount++;
					
				if(longClickCount > 5000){
					alarmOnOff = 1;
					longClickCount = 0;
					_delay_ms(500);
					beep();
				}
				else{
					while(1){
						if(BUTTONPIN & (1<<BUTTON2_PIN)){
							beep();
							resetAlarmCount = 0;
							if (settingAlarmMode == 2) settingAlarmMode = 0;
							else settingAlarmMode++;
							while(BUTTONPIN & (1<<BUTTON2_PIN)){}
							_delay_ms(500);
						}
						if(BUTTONPIN & (1<<BUTTON1_PIN)){
							beep();
							resetAlarmCount = 0;
							incAlarm();
							while(BUTTONPIN & (1<<BUTTON1_PIN)){}
							_delay_ms(200);
						}
						resetAlarmCount++;
						if (resetAlarmCount == 5000000){
							resetAlarmCount = 0;
							setting = 0;
							settingMode = 0;
							settingAlarmMode = -1;
							
							break;
						}
					}
				}
			}
			else{			//    Setting time mode
			setting = 1;
				mode = 0;
				while(BUTTONPIN & (1<<BUTTON2_PIN)){}
				_delay_ms(500);
				while(1){
					if(BUTTONPIN & (1<<BUTTON2_PIN)){
						beep();
						resetSettingCount = 0;
						incSettingMode();
						while(BUTTONPIN & (1<<BUTTON2_PIN)){}
						_delay_ms(500);
					}
					if(BUTTONPIN & (1<<BUTTON1_PIN)){
						beep();
						resetSettingCount = 0;
						incTime();
						while(BUTTONPIN & (1<<BUTTON1_PIN)){}
						_delay_ms(200);
					}
					resetSettingCount++;
					if (resetSettingCount == 5000000){
						resetSettingCount = 0;
						setting = 0;
						settingMode = 0;
						break;
						 
					}
				}
			}
		}

		if(bufCount > 0){ // uart processing
			command = uartBuf[bufHead];
			bufCount--;
			bufHead++;
			if (bufHead == SIZE_BUF) bufHead = 0;
			_delay_ms(100);
			
			while(bufCount > 0){
				temp += (uartBuf[bufHead] - 48) * power(10, bufCount-1);
				bufCount--;
				bufHead++;
				if (bufHead == SIZE_BUF) bufHead = 0;
			}
		}
		if(command)	uartTransmit(command);
		if(command == 's') ds3231_write_time(hours, mins, temp);// hours = temp;
		else if(command == 'h') ds3231_write_time(temp, mins, seconds);// hours = temp;
		else if(command == 'm') ds3231_write_time(hours, temp, seconds);// mins = temp;
		else if(command == 'w') ds3231_write_date(year, mounth, day, temp);//dayOfWen = temp;
		else if(command == 'y') ds3231_write_date(temp, mounth, day, dayOfWen);//year = temp;
		else if(command == 'M') ds3231_write_date(year, temp, day, dayOfWen);// mounth = temp;
		else if(command == 'd') ds3231_write_date(year, mounth, temp, dayOfWen);//day = temp;
		else if(command == 't') mode = temp;
		command = 0;
		temp = 0;
	}
}

