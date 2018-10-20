;;====================================
;; CS 2110 - Fall 2018
;; Homework 7
;; handshake.asm
;;====================================
;; Name: Yunqi Liu
;;====================================

; Little reminder from your pals: don't run this directly by pressing
; ``Run'' in complx, since (look below) there's nothing put at address
; x3000. Instead, load it and use ``Debug'' -> ``Simulate Subroutine
; Call'' and choose the ``strlen'' label.

.orig x3000

halt

handshake

; [ implement handshake here ]
    ADD R6, R6, -3   ;initialize space for everything
    STR R7, R6, 1    ;store RA
    STR R5, R6, 0    ;store old FP
    ADD R6, R6, -5   ;initialize space for registers
    ADD R5, R6, 4    ;place new FP
    STR R0, R6, 4    ;store R0 on the stack
    STR R1, R6, 3    ;store R1 on the stack
    STR R2, R6, 2    ;store R2 on the stack
    STR R3, R6, 1    ;store R3 on the stack
    STR R4, R6, 0    ;store R4 on the stack

	LDR R0, R5, 4
	ADD R1, R0, -1
	BRNZ BASE

	ADD R6, R6, -1
	STR R1, R6, 0
	JSR handshake

	LDR R2, R6, 0 ;store the return value from calling mode
	ADD R2, R2, R1
	ADD R6, R5, -4

	STR R2, R5, 3
	BR STACK_BREAKDOWN

BASE
	AND R1, R1, 0
	STR R1, R5, 3
	BR STACK_BREAKDOWN

STACK_BREAKDOWN
	LDR R4, R5, -4
	LDR R3, R5, -3
	LDR R2, R5, -2
	LDR R1, R5, -1
	LDR R0, R5, 0
	ADD R6, R5, 0
	LDR R5, R6, 1
	LDR R7, R6, 2
	ADD R6, R6, 3
	RET

; Needed by Simulate Subroutine Call in complx
STACK .fill xF000
.end

; You should not have to LD from any label, take the
; address off the stack instead :)
