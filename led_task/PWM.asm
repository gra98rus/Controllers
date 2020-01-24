.def temp1 = r17
.def time_ris1 = r20

.def temp_true = r18
.def temp_false = r19

.dseg

.cseg
.org 0

start:
	;ldi r16, high(RAMEND)
	;out SPH, r16
	;ldi r17, low(RAMEND)
	;out SPL, r17

	sbi DDRB, 1

	ldi temp_true, 0
	ldi temp_false, 255

	ldi time_ris1, 255
	
on:	
	sbi PORTB, 1
	rcall Delaytrue
	cbi PORTB, 1
	rcall Delayfalse
	dec time_ris1
	brne on

	inc temp_true
	dec temp_false
	brne on
	rjmp off

off:	
	sbi PORTB, 1
	rcall Delaytrue
	cbi PORTB, 1
	rcall Delayfalse
	dec time_ris1
	brne off

	inc temp_false
	dec temp_true
	brne off
	rjmp on	

Delaytrue:
	ldi temp1, 1
	add temp1, temp_true
PDelaytrue:
	dec temp1
	brne PDelaytrue
	ret

Delayfalse:
	ldi temp1, 1
		add temp1, temp_false
PDelayfalse:
	dec temp1
	brne PDelayfalse
	ret
