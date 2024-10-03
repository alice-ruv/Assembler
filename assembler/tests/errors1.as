;errors1.as handles errors in the first pass

;illegal name of label
 :		mov r7, M	
0L:		add #5, r2
L100%:		dec r5
HE:LLO:		inc A
add:		add r3, #14
data:		.data 7
r1:		rts
abcdefghijklmnopqrstuvwxyz0123456789: .data 31	
 
;not valid number
		.data 1.5

		prn #2A

;not valid operand 
 		mov 5f, r1 
		sub 2, A		
 		not #15
 		clr %CL
		jmp #LBL
		bne #41
		
		prn #
		jmp %

		prn # 22
		jmp % L		

;illegal name of instruction
		jump %U

;illegal name of directive
		.directive 31
DIRECT:		.msg "error"

;missing text
		clr
		mov #1
		.data
		.string
		.extern
LABEL:

;extraneous text		
		stop now
		inc r0 , r3
		.data 1,
		.extern E 1
		
;missing comma		
		sub #1 r4
		.data 1 2

;multiple consecutive commas
		cmp r5,,r6
		.data 5 ,,6
		
;not valid string
		.string abc
		.string a"bc"
		.string "ab"c

;double declaration of labels
double:		.data 1
double:		.data 2
double:	.string "hello"
double:	stop


;extern declarations
r8:		inc r7
.extern r8

;label r8 declared earlier and cannot be declared as extern


.extern R7
R7:		inc r1

;label R7 cannot be declared again because was declared as extern
 
.extern Add
Add:		add #4 , Add

.extern Str
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  
Str:	     .string "def"

Extern:	.extern XL
;;;;;;;;;;;;;;;;;;;;;;;;;;;
XL:	.data 1
