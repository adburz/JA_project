;Bitmap steganography, version 0.0
;Author: Adam Burzyñski
;Subject: Jezyki Assemblerowe
;Year: 2020/2021
;Compiler: MASM
.data 
LSB		BYTE	0	;last significant bit value of message byte or G byte from bmp. It depends of encoding/decoding
char	BYTE	0	;
for_j	DWORD	0	;for(int j=0;...)
outerStoper DWORD 0
charByteSpace BYTE 24
	
.code
;============ DECODING ============ 
decoding PROC
mov eax, ecx
decoding ENDP



;============ ENCODING ============
;RCX, RDX R8, R9 s¹ u¿ywant dla integerow i pointer arguments od lewej do prawej
;xmm0, 1, 2 i 3 s¹ u¿ywane dla floating point arguments


encoding PROC	;(char* bmpArr, int aBegin, char* msg, int mBegin, int mEnd)

;nie wiem jak sciagnac ze stosu piaty argument
; 		;zrzucam ze stosu piaty argument = msgEnd	

	;RCX - bmpArr
	;RDX - aBegin
	;R8 - msg
	;R9 - mBegin
	;x - mEnd
	push	rbp
	mov     rbp, rsp
	mov     BYTE PTR [rbp-1], al
    mov     DWORD PTR [rbp-5], edi
    mov     BYTE PTR [rbp-6], r9b
	mov     SDWORD PTR [rbp-6], r11
    mov     DWORD PTR [rbp-14], edx
    ;mov     DWORD PTR [rbp-28], r8d


	xor al, al;#2to licznik inner fora, wiec =1
	inc al
	xor r15d,r15d	 ;#?to licznik outer fora, wiec ustawiam go na 0
	AND rdx,1
outer_for:	
			inner_for:
					mov r8,rcx		;#?loading first msg byte
					mov r13b, [r8]
					and	r13b, 1 
					mov LSB,r13b



				add al, 3	;#2adding 3 for loop
				cmp al, charByteSpace
				jle inner_for	;#2jump if less or equal

	xor al, al;#2resertuje licznik inner fora
	inc r15d			;#1inkrementuje licznik
	cmp r15d, outerStoper	;#?porownuje ze stoperem
	jne outer_for	;#2skacze jesli mniejsze
	

encoding ENDP
END