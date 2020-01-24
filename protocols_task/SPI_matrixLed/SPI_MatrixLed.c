#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define SPI_MISO 4 
#define SPI_MOSI 3
#define SPI_SCK 5  
#define SPI_SS 2  

volatile uint8_t line = 1;
volatile uint32_t position = 128;
volatile uint32_t counter = 0;

volatile int x = 0;
volatile int y = 0;
volatile int xDirection = 1;
volatile int yDirection = 1;

volatile uint32_t positions[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void SPI_Init(void)
{

    DDRB |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS) | (0 << SPI_MISO);
    PORTB |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS) | (1 << SPI_MISO);

    SPCR = (1 << SPE) | (1 << MSTR) | (0 << CPOL) | (0 << CPHA) | (0 << SPR1) | (0 << SPR0);
    SPSR = (1 << SPI2X);
}

void SPI_WriteLine(uint8_t addr, uint32_t data)
{
    PORTB &= ~(1 << SPI_SS);
    for (int i = 0; i < 4; i++) {
        SPDR = addr;
        while (!(SPSR & (1 << SPIF)));
    
        SPDR = (data >> (i * 8));
        while (!(SPSR & (1 << SPIF)));
    }
    PORTB |= (1 << SPI_SS);
}

uint32_t power(uint32_t num, uint32_t power){
    uint32_t result = num;
	if(power == 0) return 1;
    for(int i = 0; i != power-1; i++)
        result *= num;
    return result;
}

void MAX7219_Init() {
    for (int i = 1; i < 9; ++i) {
        SPI_WriteLine(i, 0x00000000);
    }
    SPI_WriteLine(0x09, 0x00000000);
    SPI_WriteLine(0x0A, 0x0F0F0F0F);
    SPI_WriteLine(0x0B, 0x07070707);
    SPI_WriteLine(0x0C, 0x01010101);
}

ISR(TIMER1_OVF_vect){
	cli();

	setXY(0, 0);
	if(x > 31) xDirection = -2;
	if(y == 7) yDirection = -1;
	if(x < 1) xDirection = 1;
	if(y == 0) yDirection = 1;
	x = x + xDirection;
	y = y + yDirection;
	setXY(x, y);
	sei();
	return;
}

void setXY(int xC, int yC){
	int temp = xC/8;
	int xCoord = 8*temp + 7 - xC%8;
	line = yC + 1;
	positions[yC] |= power(2, xCoord);
	SPI_WriteLine(line, positions[yC]);
}

int main(){
    SPI_Init();
    MAX7219_Init();
    
    TCCR1B = 3 << CS10;
    TIMSK1 = 1 << TOIE1;
	sei();
	//setXY(9, 5);
    while(1){}
    return 0;
}
