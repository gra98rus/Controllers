.def temp = r17
.def temp1 = r18
.def temp2 = r19

.dseg

.cseg
.org 0

start:
	sbi DDRD, PD4
	sbi DDRD, PD5
	sbi DDRD, PD6
	sbi DDRD, PD7

	cbi DDRB, PB0
	cbi DDRB, PB1
	sbi DDRB, PB2
	sbi DDRB, PB3

	sbi PORTB, PB0
	sbi PORTB, PB1

loop:
	sbi DDRB, PB2
	sbi PORTB, PB2
	cbi DDRB, PB3

	sbic PINB, PB0
	rcall f0

	sbic PINB, PB1
	rcall f2
	
	cbi DDRB, PB2
	sbi DDRB, PB3
	sbi PORTB, PB3

	sbic PINB, PB0
	rcall f1
	
	sbic PINB, PB1
	rcall f3

	rjmp loop
	
f0:
	rcall led0
	call check0
	ret

f1:
	rcall led1
	call check0
	ret


f2:
	rcall led2
	call check1
	ret

f3:
	rcall led3
	call check1
	ret


led0:
	sbis PORTD, PD4 
	jmp on0
	cbi PORTD, PD4 
	ret

on0:
	sbi PORTD, PD4
	ret

led1:
	sbis PORTD, PD5
	jmp on1
	cbi PORTD, PD5 
	ret

on1:
	sbi PORTD, PD5
	ret

led2:
	sbis PORTD, PD6
	jmp on2
	cbi PORTD, PD6 
	ret

on2:
	sbi PORTD, PD6
	ret

led3:
	sbis PORTD, PD7
	jmp on3
	cbi PORTD, PD7
	ret

on3:
	sbi PORTD, PD7
	ret

check0:
	rcall wait
	sbic PINB, PB0
	rjmp check0
	ret

check1:
	rcall wait
	sbic PINB, PB1
	rjmp check1
	ret
	
wait:
	ldi temp, 255
	ldi temp1, 255
	ldi temp2, 1
wait1:
	dec temp
	brne wait1
	dec temp1
	brne wait1
	dec temp2
	brne wait1
	ret