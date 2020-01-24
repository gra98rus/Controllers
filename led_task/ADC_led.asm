.def temp1 = r17
.def temp = r16
.def ADCval = r21
.def duty = r19

.dseg

.cseg
.org 0

start:

    ldi temp, high(RAMEND)
    out sph, temp
    ldi temp, low(RAMEND)
    out spl, temp

	;ldi temp, 0b11100011
	ldi temp, 1<<ADEN|1<<ADSC|1<<ADATE|1<<ADPS2|1<<ADPS1
	sts  ADCSRA, temp 
	;ldi temp, 0b01100000
	ldi temp, 1<<REFS0|1<<ADLAR
	sts  ADMUX, temp

	sbi DDRB, PB1
	sbi DDRB, PB2

    ;ldi temp, 0b11000010
	ldi temp, 1<<COM1A1|1<<COM1A0|1<<WGM11
	sts TCCR1A, temp

;	ldi temp, 0b00011001
	ldi temp, 1<<WGM13|1<<WGM12|1<<CS10
	sts TCCR1B, temp

    ldi temp, 0xff
    sts ICR1H, temp
    ldi temp, 0xff
	sts ICR1L, temp


    ldi duty, 0x01
    sts OCR1AH, duty
    ldi duty, 0xff
    sts OCR1AL, duty


loop:
	lds duty, ADCH
    sts OCR1AH, duty  
	ldi temp, ADCL
	sts OCR1AL, temp

	rjmp loop
