.def temp = r17
.def temp1 = r18
.def temp2 = r19

.dseg

.cseg
.org 0

start:
	sbi DDRB, PB1
	cbi DDRB, PB0
	ldi r17, 0

on:
	sbis PINB, PB0
	rjmp on
	sbi PORTB, PB1
	call check
	rjmp off

off:
	sbis PINB, PB0
	rjmp off
	cbi PORTB, PB1
	call check

	rjmp on

check:
	rcall wait
	sbic PINB, PB0
	rjmp check
	ret

wait:
	ldi temp, 255
	ldi temp1, 255
	ldi temp2, 15
wait1:
	dec temp
	brne wait1
	dec temp1
	brne wait1
	dec temp2
	brne wait1
	ret