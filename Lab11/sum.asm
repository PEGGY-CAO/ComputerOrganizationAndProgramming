;;===========================
;; Name: YUQI CAO
;;===========================

;;Problem
;;-------
;;Sum the values from the memory locations labelled X and Y, then print a message
;;and their sum to the user, in the console using printing traps. For example, if
;;the sum is 5, it should print: "The sum is: 5". For simplicity, the sum will
;;only be between 0 and 9, inclusive.

;;Pseudocode
;;----------
;;void sum() {
;;    int x = 2;
;;    int y = 3;
;;    int sum = x + y;
;;    const char *msg = "The sum is: ";
;;    printf(msg); // Uses the address of the first character to print msg
;;                 // Which LC3 instruction can you use to get the address of a
;;                 // label? Take a look at the ISA (appendix A)
;;    printf("%c", sum + 48); // The OUT trap prints the number as ASCII!
;;}
;;
;;
;;Hints
;;-----
;;You may find the PUTS trap to be helpful for printing strings. Read about the
;;PUTS trap in the ISA to see how it works!

;;You may find the OUT trap to be helpful for printing numbers. Read about the
;;OUT trap in the ISA to see how it works!

;;Note that OUT prints the number as an ASCII
;;value, so if your sum is 7 and you try to print it, you'll get garbage values
;;in your output. You must add the ASCII offset for '0' (which is 48).

;;The line const char *msg = "The sum is: "; declares a variable called "msg"
;;that contains the address of the first character of a null-terminated string
;;containing the characters "The sum is: ".

;;Strings in C and LC3 are null-terminated, meaning that there is a value of
;;zero at the end of the string. Printing functions like printf and PUTS know to
;;stop printing characters when they reach this value.


.orig x3000
;;your code here!

LEA R0, HELLO ; GET STARTING
PUTS ; Print to concole
LD R1, X
LD R2, Y
ADD R0, R1, R2
LD R1, TOASCII
ADD R0, R0, R1
OUT




    HALT
HELLO   .stringz "The sum is: "
X   .fill 2
Y   .fill 3
TOASCII .fill 48

.end

