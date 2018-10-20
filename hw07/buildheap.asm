;;====================================
;; CS 2110 - Fall 2018
;; Homework 7
;; buildheap.asm
;;====================================
;; Name: Yuqi Cao
;;====================================

; Little reminder from your pals: don't run this directly by pressing
; ``Run'' in complx, since (look below) there's nothing put at address
; x3000. Instead, load it and use ``Debug'' -> ``Simulate Subroutine
; Call'' and choose the ``strlen'' label.

.orig x3000

halt

heapify

; [ implement heapify here ]
ADD R6, R6, -3   ;initialize space 
STR R7, R6, 1    ;store RA
STR R5, R6, 0    ;store old FP
ADD R6, R6, -5   ;initialize space for registers
ADD R5, R6, 4    ;place new FP
STR R0, R6, 4    ;store R0 in stack
STR R1, R6, 3    ;store R1 in stack
STR R2, R6, 2    ;store R2 in stack
STR R3, R6, 1    ;store R3 in stack
STR R4, R6, 0    ;store R4 in stack

LDR R0, R5, 6 ; largest
LDR R1, R5, 5 ; n, total num
NOT R1, R1
ADD R1, R1, 1 ; R1 -> -R1
LDR R2, R5, 6 ; i, root
ADD R4, R2, R2 ; left child = 2i + 1
ADD R4, R4, 1

ADD R4, R4, R1 
BRZP LABELR
ADD R4, R2, R2
ADD R4, R4, 1
LDR R3, R5, 4
ADD R4, R4, R3 ; arr[leftchild]
LDR R4, R4, 0
ADD R1, R0, 0
ADD R1, R1, R3 ; arr[largest]
LDR R1, R1, 0
NOT R4, R4
ADD R4, R4, 1
ADD R4, R4, R1 ; if arr[largest] < arr[leftchild]
BRZP LABELR
;; largest = left
LDR R2, R5, 6
ADD R0, R2, R2
ADD R0, R0, 1
BR LABELR

LABELR
LDR R2, R5, 6
ADD R4, R2, R2
ADD R4, R4, 2
LDR R1, R5, 5
NOT R1, R1
ADD R1, R1, 1
ADD R4, R4, R1 ; if right < n
BRZP OPR

ADD R4, R2, R2
ADD R4, R4, 2
LDR R3, R5, 4
ADD R4, R4, R3 ; arr[rightchild]
LDR R4, R4, 0
ADD R1, R3, R0 ; arr[largest]
LDR R1, R1, 0
NOT R4, R4
ADD R4, R4, 1
ADD R4, R4, R1 ; if arr[largest] < a[rightchild]
BRZP OPR

;; largest = right
LDR R2, R5, 6
ADD R0, R2, R2
ADD R0, R0, 2
BR OPR


OPR 
LDR R2, R5, 6
NOT R2, R2
ADD R2, R2, 1
ADD R2, R2, R0
BRZ FINISH
BRNP EXCHANGE

EXCHANGE 
LDR R1, R5, 4
LDR R2, R5, 6
ADD R2, R2, R1 
ADD R3, R0, R1 

LDR R1, R2, 0 
LDR R4, R3, 0 

STR R1, R3, 0
STR R4, R2, 0

LDR R1, R5, 4
LDR R2, R5, 5
ADD R6, R6, -3
STR R1, R6, 0
STR R2, R6, 1
STR R0, R6, 2
JSR heapify

LDR R1, R6, 0
ADD R6, R6, 4
BR FINISH

FINISH 
LDR R4, R6, 0
LDR R3, R6, 1
LDR R2, R6, 2
LDR R1, R6, 3
LDR R0, R6, 4
ADD R6, R6, 4
LDR R5, R6, 1
LDR R7, R6, 2
ADD R6, R6, 3
RET


buildheap

; [ implement buildheap here ]
ADD R6, R6, -3   ;initialize space for everything
STR R7, R6, 1    ;store RA
STR R5, R6, 0    ;store old FP
ADD R6, R6, -5   ;initialize space for registers
ADD R5, R6, 4    ;place new FP
STR R0, R6, 4    ;store R0 in stack
STR R1, R6, 3    ;store R1 in stack
STR R2, R6, 2    ;store R2 in stack
STR R3, R6, 1    ;store R3 in stack
STR R4, R6, 0    ;store R4 in stack

LDR R0, R5, 4 
LDR R1, R5, 5 
ADD R2, R1, 0

LOOP 
ADD R2, R2, 0
BRN PARSE

ADD R6, R6, -3
STR R0, R6, 0
STR R1, R6, 1
STR R2, R6, 2
JSR heapify
LDR R3, R6, 0
ADD R6, R6, 4
ADD R2, R2, -1
BR LOOP


PARSE 
LDR R4, R6, 0
LDR R3, R6, 1
LDR R2, R6, 2
LDR R1, R6, 3
LDR R0, R6, 4
ADD R6, R6, 4
LDR R5, R6, 1
LDR R7, R6, 2
ADD R6, R6, 3
RET

; Needed by Simulate Subroutine Call in complx
STACK .fill xF000
.end

; You should not have to LD from any label, take the
; address off the stack instead :)
