.def temp = r16
.def temp1 = r17
.def dig0 = r18
.def dig1 = r19
.def dig2 = r20
.def dig3 = r21
.def number = r22
.def numberH = r26
.def character = r23
.def testnum = r24
.def testnumH = r25
.def verytemp = r27



.equ num0 = 0b00111111
.equ num1 = 0b00000110
.equ num2 = 0b01011011
.equ num3 = 0b01001111
.equ num4 = 0b01100110
.equ num5 = 0b01101101
.equ num6 = 0b01111101
.equ num7 = 0b00000111
.equ num8 = 0b01111111
.equ num9 = 0b01101111

.dseg
 
.cseg
.org 0
rjmp start
.org 0x0012
jmp TIM2_OVF
.org 0x001a	
jmp TIM1_OVF
start:

	 ldi R16,LOW(RamEnd)
	 out SPL,R16
	ldi R16,HIGH(RamEnd)
	out SPH,R16

	ldi temp, 0xFF
    out DDRD, temp

	ldi temp, 0xFF
	out DDRB, temp

	ldi temp, 0x00
	out PORTB, temp

	ldi dig0, 0x00
	ldi dig1, 0x00
	ldi dig2, 0x00
	ldi dig3, 0x00

	ldi temp1, 0x10

	ldi temp, 0b00000100
	sts TCCR1B, temp	

	ldi temp, 0b00000001
	sts TIMSK1, temp		

	ldi temp, 0x08
	sts TCNT1H, temp
	sts TCNT1L, temp

	ldi temp, 0b00000010
	sts TCCR2B, temp	

	ldi temp, 0b00000001
	sts TIMSK2, temp		

	ldi temp, 0x00
	sts TCNT2, temp

	ldi numberH, 0
	ldi number, 0

	sei

Loop:
	rjmp Loop

TIM2_OVF:
	cli
	cpi temp1, 0x10
	breq reset
next:
	lsl temp1
    mov temp, temp1 
	com temp
	out PORTB, temp

	sbrs temp, 1
	out PORTD, dig0
	sbrs temp, 2
	out PORTD, dig1	
	sbrs temp, 3
	out PORTD, dig2	
	sbrs temp, 4
	out PORTD, dig3

	sei
	reti

reset:
	inc temp1
	rjmp next

TIM1_OVF:
	cli

	cpi number, 255
	breq incH
	inc number
	jmp parser
	sei
	reti

incH:
	inc numberH
	inc number
	jmp parser
	sei
	reti

parser:
	mov testnum, number
	mov testnumH, numberH
	ldi character, 0
parserThousand:
	cpi testnumH, 3	
	brsh checkThousand
endThousand:
	call setCharacter
	mov dig3, character
	ldi character, 0
parserHundred:
	cpi testnumH, 1
	brsh minHundred1
	cpi testnum, 100
	brsh minHundred2
	call setCharacter
	mov dig2, character
	ldi character, 0
parserTen:
	cpi testnum, 10
	brsh minTen
	call setCharacter
	mov dig1, character
	ldi character, 0
parserOne:
	cpi testnum, 1
	brsh minOne
	call setCharacter
	mov dig0, character
	ldi character, 0
	sei
	reti



minOne:
	subi testnum, 1
	inc character
	jmp parserOne


minTen:
	subi testnum, 10
	inc character
	jmp parserTen


minHundred1:
	cpi testnum, 100
	brsh minHundred2
	ldi verytemp, 156
	add testnum, verytemp
	dec testnumH
	inc character
	jmp parserHundred

minHundred2:
	subi testnum, 100
	inc character
	jmp parserHundred


checkThousand:
	cpi testnumH, 3
	breq equalHThousand
	jmp moreHThousand

moreHThousand:
	cpi testnum, 0xe8
	brsh minThousand1
	jmp minThousand2

equalHThousand:
	cpi testnum, 0xe8
	brsh minThousand1
	jmp endThousand
		
minThousand1:
	subi testnumH, 3
	subi testnum, 0xe8
	inc character
	jmp parserThousand

minThousand2:
	ldi verytemp, 24
	subi testnumH, 4
	add testnum, verytemp
	inc character
	jmp parserThousand
	


setCharacter:
	cpi character, 0x00
	breq set0
	cpi character, 0x01
	breq set1	
	cpi character, 0x02
	breq set2	
	cpi character, 0x03
	breq set3	
	cpi character, 0x04
	breq set4	
	cpi character, 0x05
	breq set5	
	cpi character, 0x06
	breq set6	
	cpi character, 0x07
	breq set7	
	cpi character, 0x08
	breq set8	
	cpi character, 0x09
	breq set9

set0: 
	ldi character, num0
	ret

set1: 
	ldi character, num1
	ret

set2: 
	ldi character, num2
	ret

set3: 
	ldi character, num3
	ret

set4: 
	ldi character, num4
	ret

set5: 
	ldi character, num5
	ret

set6: 
	ldi character, num6
	ret

set7: 
	ldi character, num7
	ret

set8: 
	ldi character, num8
	ret

set9: 
	ldi character, num9
	ret
