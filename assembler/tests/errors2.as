;errors2.as handles errors in the second pass

;label was not found in symbol table
lea f, r1
.entry r8

;symbol DATA must be declared in instruction line
	jmp %DATA
DATA:	.data 4


;missing label 
.entry

;extraneous text
.entry DATA 1


;lable cannot be declared both as external and as entry

.extern WL
.entry WL

.entry WL2
.extern WL2
