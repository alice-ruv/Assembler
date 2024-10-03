;test3.as contains all types of the instructions

;instructions with two operands
	mov #4,A
	mov A, r1
	mov r6 ,r7
	cmp #-4 , r5
	cmp #8 , #+8
	cmp r2, A
	cmp r3, r4
	add A, r0
	add r1, r2
	add #1, A
	add A, val 
Line1:	sub #3,r1
	sub A ,val
	sub r3, r4
	lea A, r6
	lea A, val

;instructions with one operand
	clr A
	clr r2
	not A
	not r1
	inc val
	inc r3
	dec val
	dec r4

; at the end data image will be represented after code image
  
	.string "string#1"
val:    .data 5,-10
	.data 5000

	jmp Line1
	jmp %Line1
	bne Line2
	bne %Line2
	jsr Line3
	jsr %Line3

XYZ:	.string "string#2"
	.string "string"#3"

Line2:	red r1
	red A
	prn #49
	prn A
	prn r4

;instructions with no operands
Line3:	rts
	stop

A:     .data -7, 17, +20, 9

       .string "" 
