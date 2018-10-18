
;;====================================================
;; CS 2110 - Fall 2018
;; Timed Lab 3
;; timedlab3.asm
;;====================================================
;; Name: Yuqi Cao
;;====================================================

.orig x3000

;; YOUR CODE HERE :D

LD R0, STR_ADDR ;; *p1 = &string
LD R1, SPACE	;; r1 = 32 -> ' '
LD R2, NINE		;; r2 = 57 -> '9'
NOT R2, R2
ADD R2, R2, #1	;; r2 = -57

LOOP
LDR R3, R0, #0 ;; char a1 = *(p1)

BRz STOP ;; if *(p1) == 0 terminate loop
AND R4, R4, #0 
ADD R4, R3, R2 ;; a4 = a1 - '9'
BRp	SETSPACE ;; if a4 > 0 , *(p1) = ' ' 
ICREP
ADD R0, R0, #1 ;; else p1++, then go for loop
BRnzp LOOP
SETSPACE
STR R1, R0, #0
BR ICREP
STOP
HALT

STR_ADDR .fill x5000
SPACE	.fill 32
NINE 	.fill 57
.end

.orig x5000
  .stringz "asdfasdfasdf"
.end



