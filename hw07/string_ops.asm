;;====================================
;; CS 2110 - Fall 2018
;; Homework 7
;; string_ops.asm
;;====================================
;; Name: Yuqi Cao
;;====================================

; Little reminder from your pals: don't run this directly by pressing
; ``Run'' in complx, since (look below) there's nothing put at address
; x3000. Instead, load it and use ``Debug'' -> ``Simulate Subroutine
; Call'' and choose the ``strlen'' label.

.orig x3000

halt

strlen
ADD R6, R6, -3   ;initialize space for everything
STR R7, R6, 1    ;store RA
STR R5, R6, 0    ;store old FP
ADD R6, R6, -4   ;initialize space for registers
ADD R5, R6, 3    ;place new FP
STR R0, R6, 3    ;store R0 on the stack
STR R1, R6, 2    ;store R1 on the stack
STR R2, R6, 1    ;store R2 on the stack
STR R3, R6, 0    ;store R3 on the stack

LDR R0, R5, 4    ;load the address of string into R0
AND R1, R1, 0    ;reset R1 to 0, it'll be the length

LOOPOVER
ADD R2, R0, R1   ;R2 -> address of current character
LDR R2, R2, 0    ;R2 -> value of current character

BRZ ENDOPT    

ADD R1, R1, 1    ;increment string length
BR LOOPOVER

ENDOPT
STR R1, R5, 3    ;store strlen at the return value spot

LDR R3, R6, 0   ;reload R3
LDR R2, R6, 1   ;reload R2
LDR R1, R6, 2   ;reload R1
LDR R0, R6, 3   ;reload R0
ADD R6, R6, 4   ;add the stack pointer
LDR R5, R6, 0   ;reload old FP
LDR R7, R6, 1   ;reload return address
ADD R6, R6, 2   ;put the stack pointer at the rv
RET



count_occurrence

; [ implement count_occurrence() here ]
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

LDR R0, R5, 4 ; given string s starting address
LDR R1, R5, 5 ; given char
NOT R1, R1
ADD R1, R1, 1

ADD R6, R6, -1
STR R0, R6, 0
JSR strlen

LDR R3, R6, 0 ; string length
ADD R6, R5, -4
AND R2, R2, 0 ; count
LOOP
ADD R4, R3, 0
BRNZ STOP
LDR R4, R0, 0; get current char
ADD R4, R4, R1
BRNP NEW
ADD R2, R2, 1
BR NEW

NEW 
ADD R3, R3, 1
ADD R0, R0, 1
BR LOOP

STOP STR R2, R5, 3

;; Teardown
LDR R4, R6, 0   ;reload R4
LDR R3, R6, 1   ;reload R3
LDR R2, R6, 2   ;reload R2
LDR R1, R6, 3   ;reload R1
LDR R0, R6, 4   ;reload R0
ADD R6, R6, 5   ;add the stack pointer
LDR R5, R6, 0   ;reload old FP
LDR R7, R6, 1   ;reload return address
ADD R6, R6, 2   ;put the stack pointer at the rv
RET

; Needed by Simulate Subroutine Call in complx
STACK .fill xF000
.end

; You should not have to LD from any label, take the
; address off the stack instead :)
