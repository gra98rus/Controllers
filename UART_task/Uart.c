#include <avr/io.h>
#include <avr/interrupt.h>

ISR( USART_RX_vect ){
	cli();
	int rxbyte = UDR0;
	if(rxbyte==0x30) PORTB = 0;
	if(rxbyte==0x31) PORTB = 32;
	sei();
	reti();
}

void main(void){
	USART_Init();
	DDRB = DDRB | (1 << 5);
	DDRD = 0x00;
	
	while(1)
	{
		if(PIND & (1<<2)){
			buttonFunc();
			while(PIND & (1<<2)){}
			for(int i = 0; i<10000;i++);
		}
	}
}

void USART_Init(){
	UBRR0H = 0;
	UBRR0L = 104;
	UCSR0A = (0<<U2X0);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|1<<RXCIE0|0<<TXCIE0;
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	sei();
	}

void UART_Send_Str(char str[]){
	unsigned char i = 0;

	while (str[i]!='\0')
	{
		USART_Transmit(str[i]);
		i++;
	}
	USART_Transmit('\n');
}

void USART_Transmit(char data){
	while (!(UCSR0A & (1<<UDRE0))){};
	UDR0 = data;
}

int USART_Receive(void){
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void USART_Resend(void){
	while (!(UCSR0A & (1<<RXC0)));
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = UDR0;
}

void buttonFunc(){
	if(PORTB & (1<<5)) UART_Send_Str("led on ");
	else UART_Send_Str("led off ");
}
