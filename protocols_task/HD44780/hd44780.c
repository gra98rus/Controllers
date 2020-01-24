#include <avr/io.h>
#include <util/delay.h>

#define E 3
#define RW 2
#define RS 1

void strob(void)            
{
	PORTB|=(1<<E);
	_delay_ms(5);
	PORTB&=~(1<<E);
}

void init(){

	uint8_t DL = 4;
	uint8_t N = 3;
	uint8_t F = 2;
	
	PORTB&=~(1<<E);
	PORTD = 0;

	PORTD |= (1<<5)|(1<<DL)|(1<<N)|(0<<F);
	strob();
	PORTD = 0;
	
	PORTD |= (1<<1);
	strob();
}

void clear(){
	PORTD = 0b00000001;
	strob();
	_delay_ms(100);
}

void moveCursor(uint8_t dir){
	PORTB&=~(1<<E);
	uint8_t SC = 3;
	uint8_t RL = 2;
	PORTD = 0;
	
	PORTD |= (1<<4)|(0<<SC)|(dir<<RL);
	strob();

}

void printASCII(char sumbol){
	PORTB = 0b00000010;
	PORTD = (int) sumbol;
	strob();
	PORTB&=~(1<<1);
}

void printASCIIbyNum(uint8_t num){
	PORTB = 0b00000010;
	PORTD = num;
	strob();
	PORTB&=~(1<<1);
}

void printASCIITo(char sumbol, uint8_t str, uint8_t coord){
	resetCursor();
	for(uint8_t i = 0; i<coord; i++)
		moveCursor(1);
	if (str)
		jumpCoursorString();
	PORTB = 0b00000010;
	PORTD = (int) sumbol;
	strob();
	PORTB&=~(1<<1);
}

void printASCIIbyNumTo(uint8_t num, uint8_t str, uint8_t coord){
	resetCursor();
	for(uint8_t i = 0; i<coord; i++)
	moveCursor(1);
	if (str)
	jumpCoursorString();
	PORTB = 0b00000010;
	PORTD = num;
	strob();
	PORTB&=~(1<<1);
}

void printString(char str[]){
	unsigned char i = 0;

	while (str[i]!='\0')
	{
		printASCII(str[i]);
		i++;
	}
}

void addNewSumbolAngs(){
	PORTD = 0b01001000;
	strob();
	PORTB = 0b00000010;
	
	PORTD = 0b00000000;
	strob();
	PORTD = 0b00000000;
	strob();
	PORTD = 0b00000000;
	strob();
	PORTD = 0b00011100;
	strob();
	PORTD = 0b00000100;
	strob();
	PORTD = 0b00000100;
	strob();
	PORTD = 0b00000100;
	strob();
	PORTD = 0b00000100;
	strob();
	
	PORTB = 0b00000000;
		
	PORTD = 0b10000000;
	strob();
	////////////////////////////////////
	PORTD = 0b01010000;
	strob();
	PORTB = 0b00000010;
	
	PORTD = 0b00000100;
	strob();
	PORTD = 0b00000100;
	strob();
	PORTD = 0b00000100;
	strob();
	PORTD = 0b00000111;
	strob();
	PORTD = 0b00000000;
	strob();
	PORTD = 0b00000000;
	strob();
	PORTD = 0b00000000;
	strob();
	PORTD = 0b00000000;
	strob();
	
	PORTB = 0b00000000;
	
	PORTD = 0b10000000;
	strob();
	////////////////////////////////////
	PORTD = 0b01011000;
	strob();
	PORTB = 0b00000010;
	
	PORTD = 0b00000100;
	strob();
	PORTD = 0b00000100;
	strob();
	PORTD = 0b00000100;
	strob();
	PORTD = 0b00011100;
	strob();
	PORTD = 0b00000000;
	strob();
	PORTD = 0b00000000;
	strob();
	PORTD = 0b00000000;
	strob();
	PORTD = 0b00000000;
	strob();
	
	PORTB = 0b00000000;
	
	PORTD = 0b10000000;
	strob();
	/////////////////////////////
	PORTD = 0b01100000;
	strob();
	PORTB = 0b00000010;
	
	PORTD = 0b00000000;
	strob();
	PORTD = 0b00000000;
	strob();
	PORTD = 0b00000000;
	strob();
	PORTD = 0b00000111;
	strob();
	PORTD = 0b00000100;
	strob();
	PORTD = 0b00000100;
	strob();
	PORTD = 0b00000100;
	strob();
	PORTD = 0b00000100;
	strob();
	
	PORTB = 0b00000000;
	
	PORTD = 0b10000000;
	strob();
	
}

void jumpCoursorString(){
	for(int i = 0; i <40; i++)
		moveCursor(1);
}

void moveOptions(uint8_t inc, uint8_t screen){
	uint8_t ID = 1;
	uint8_t S = 0;
	PORTD = 0;
	PORTD |= (1<<2)|(inc<<ID)|(screen<<S);
	strob();
}

void visibleOptions(uint8_t onDisp, uint8_t onCursor, uint8_t sqCursor){
	uint8_t D = 2;
	uint8_t C = 1;
	uint8_t B = 0;
	PORTD = 0;
	PORTD |= (1<<3)|(onDisp<<D)|(onCursor<<C)|(sqCursor<<B);
	strob();
}

uint8_t readData(){
	
	DDRD = 0;
	PORTD = 0;
	PORTB = 0b00000110;
	_delay_ms(100);
	strob();
	uint8_t val = PIND;
	DDRD = 255;
	PORTB = 0;
	moveCursor(0);
	return val-1;
}

void resetCursor(){
	PORTD = 0b00000010;
	strob();
}

int main(void)
{
	_delay_ms(100);
	
	DDRB = 255;
	DDRD = 255;
	PORTB = 0;
	
	init();
	clear();
	moveOptions(1, 0);
	
	visibleOptions(1, 0, 0);
	
	addNewSumbolAngs();
	
	printString("privet, mir!");
	
	uint8_t symbol = readData();
	
	jumpCoursorString();
	
	printASCIIbyNum(symbol);
	
	_delay_ms(100000);
		
	clear();
	
	resetCursor();
/////////////////////////////////////////   snake   /////////////////////////////////////////////
	uint8_t xTail = 0;
	uint8_t yTail = 0;
	uint8_t xHead = 0;
	uint8_t yHead = 0;
	uint8_t x;
	uint8_t y;
	uint8_t element;
	uint8_t lenght = 6;
	while(1){
		for(int i = 0; i < 32; i++){
			x = (uint8_t) i/2;
			y = (i+1)&2;
			element = (i%4)+1;
			
			printASCIIbyNumTo(element, y, x);
			
			xHead = i!=31 ? (i+1)/2 : 0;
			yHead = (i+1+1)&2;
			printASCIIbyNumTo(35, yHead, xHead);
			
			yTail = y;
			xTail = x>=lenght ? x-lenght : 16 - lenght + x;
			
			printASCIIbyNumTo(16, yTail, xTail);
			_delay_ms(5000);
		}		
	}
}
