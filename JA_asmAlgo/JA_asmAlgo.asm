;Bitmap steganography, version 0.0
;Author: Adam Burzyñski
;Subject: Jezyki Assemblerowe
;Year: 2020/2021
;Compiler: MASM
.data 
BMP_BYTE_COUNT_FOR_ONE_CHAR BYTE 24
	
.code
;============ DECODING ============ 
decoding PROC
	
	;RCX - bmpArr
	;RDX - msgArr
	;R8 - mEnd

	push rbp					; zapisuje adres rejestru RBP
	push rdi					; zapisuje adres rejestru RDI
	push rsp					; zapisuje adres rejestru RSP

	mov R9, RDX		;zapisuje wskaznik na tablice z wiadomoscia, aby moc uzywac instrukcji mul, ktora potrzebuje rejestru RDX do poprawnego dzialania
	xor rbx, rbx	;licznik "i" zapisany w rejestrze al wewnetrznej petli for, ktora w cpp wyglada tak: for(int i=1; i<=24; i+=3)
	inc bl			;inkrementujê rejestr al, aby docelowo byl rowny 1

	xor r15,r15		;licznik zewnetrznej petli for. Petla w cpp wyglada tak: for(int i=0; i<mEnd; i++), gdzie mEnd to wartosc zaladowana do rejestru r15
	
;---------------- OUTER FOR ----------------		
outer_for:	
;---------------- INNER FOR ----------------		
					xor r13, r13	;przygotowuje rejestr w ktorym bede skladowal odkodowane dane, budujac z nich znaki.

			inner_for:				;petla wyglada w cpp tak: for(int i=1; i<=24; i+=3) poniewaz kodujemy co trzeci bajt pliku BMP -> 1 bajt wiadomosci = 24 bajty pliku bmp.
						;obliczam index tablicy z bitmapa	
					movzx rax, BMP_BYTE_COUNT_FOR_ONE_CHAR;w rejestrze eax znajdowac sie bedzie offset do miejsca w tablicy, do ktorego chce zaladowac porzadany bit(r13b).
					mul r15d							;zgodnie z algorytmem, przemnazam iterator zewnetrznej petli przez stala potrzebna do ustalenia miejsca w tablicy do zakodowania bitu(BMP_BYTE_COUNT_FOR_ONE_CHAR * zewnetrzny_iterator)
					add eax, ebx						;zgodnie z algorytmem, dodaje wartosc wewnetrznego iteratora(EAX + wewnetrzny_iterator) 

					mov r12b, [RCX + RAX]				;pobieram z rejestru RCX w którym znajduje siê tablica z bitmap¹ okreslone miejsce dzieki rejestrowi eax
					and r12b, 1							;wykonuje operacje mod2, aby sprawdzic, aby sprawdzic wartosc bitu - jesli =1, trzeba dodac 127

					cmp r12b, 1						;sprawdzam czy odkodowany bit jest rowny 1, jesli jest, trzeba bedzie oprocz przesuniecia bitowego, dodac 10000000b, a dopiero potem przesunac
					je do_operations				;skok do podprogramu dekodujacego 
					
					cmp bl, 22				;sprawdzam, czy wewnetrzny iterator nie jest przypadkiem rowny 22, poniewaz aby odkodowac bitow potrzebujemy 7 przesuniec bitowych, a nie 8
					jne bit_right_shift_no
					
					continue:
					
					add bl, 3								;inkrementuje licznik wewnetrznej funkcji
					cmp bl, BMP_BYTE_COUNT_FOR_ONE_CHAR
					jle inner_for	;#2jump if less or equal
					jmp inner_loop_end	;skacze na koniec inner petli


					do_operations:						;proces dekodowania
					;&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
											;poniewaz kodowalem przesuwajac bity znaku w lewo, teraz kiedy dekoduje, musze robic to w przeciwnym kierunku, czyli w prawo.
					add r13b, 10000000b	;odkodowany bit jest rowny 1, dlatego musze dodac 1 na poczatek bajtu
					cmp bl, 22				;sprawdzam, czy wewnetrzny iterator nie jest przypadkiem rowny 22, poniewaz aby odkodowac bitow potrzebujemy 7 przesuniec bitowych, a nie 8
					jne bit_right_shift		;skok do podprogramu przesuwajacego o 1 w prawo bity rejestru zawierajacego dekodowane slowo 
					continue_doing_operations:
					;sub r13b, 10000000b		;musze odjac 1, po przesunieciu, poniewaz liczba 1...b po przesunieciu o 1 w prawo da 11...b. Nie wiadomo czy nastepny bit bedzie rowny 1 czy 0, dlatego tez pozbywam sie 1
					jmp continue

					bit_right_shift:		;podprogram ktorego zadaniem jest przesuniecie bitowe dekodowanego slowa o 1 w prawo
					shr r13b, 1				;operacja przesuniecia bitowego dekodowanego slowa o 1 w prawo
					jmp continue_doing_operations	;powrot 

					bit_right_shift_no:
					shr r13b, 1
					jmp continue
					;&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
;---------------- INNER FOR ----------------
	inner_loop_end:
	mov [R9 + R15], r13b


	xor rbx, rbx			;resetuje licznik wewnetrznej petli for (inner_for)
	inc bl				;nastepnie ustawiam go na 1, zgodnie z zalozeniami petli
	inc r15d			;inkrementuje licznik zewnetrznej petli for (outer_for)
	cmp r15d, R8D		;porownuje z mEnd
	jne outer_for	;skacze jesli mniejsze

	
	pop rsp						; Return rsp from stack
	pop rdi						; Return rdi from stack 
	pop rbp						; Return rbp from stack
	
	mov eax, 0
	ret
;---------------- OUTER FOR ----------------	
decoding ENDP



;============ ENCODING ============
;RCX, RDX R8, R9 s¹ u¿ywane dla integerow i pointer arguments od lewej do prawej
;xmm0, 1, 2 i 3 s¹ u¿ywane dla floating point arguments


encoding PROC	;(char* bmpArr, char* msg, int mEnd)

	;RCX - bmpArr
	;RDX - msgArr
	;R8 - mEnd

	;uzywane zmienne 
	;RCX(bmpArr) | RDX (msgArr) | R8(mEnd) | r14(LSB) | r8 (jest wolne po przypisaniu do r15) | r13 | r12 | RBX(iterator wewnetrznej)

	
	push rbp					; Save address of previous stack frame
	push rdi					; Save register destination index
	push rsp					; Save stack pointer

	mov R9, RDX		;zapisuje wskaznik na tablice z wiadomoscia, aby moc uzywac instrukcji mul, ktora potrzebuje rejestru RDX do poprawnego dzialania
	xor rbx, rbx		;licznik "i" zapisany w rejestrze al wewnetrznej petli for, ktora w cpp wyglada tak: for(int i=1; i<=24; i+=3)
	inc bl			;inkrementujê rejestr al, aby docelowo byl rowny 1

	xor r15,r15	;licznik zewnetrznej petli for. Petla w cpp wyglada tak: for(int i=0; i<mEnd; i++), gdzie mEnd to wartosc zaladowana do rejestru r15
;---------------- OUTER FOR ----------------		
outer_for:	
;---------------- INNER FOR ----------------		
					mov r14b, [R9 + R15]	;czytam przy pomocy iteratora zewnetrznej petli (RBX) jeden znak (bajt) z wiadomosci.
			inner_for:				;petla wyglada w cpp tak: for(int i=1; i<=24; i+=3) poniewaz kodujemy co trzeci bajt pliku BMP -> 1 bajt wiadomosci = 24 bajty pliku bmp.
					mov r13b, r14b			;zapisuje r13, aby nie stracic bajtu z r14. Bajt zostanie potraktowany przesunieciem bitowym w lewo o 1 z koncem petli. 
					and r13b, 1				;wykonujê operacjê mod2, aby odczytaæ bit o najmniejszym znaczeniu bajtu, który nastêpnie zostanie zakodowany w obrazie. 

														;obliczam index tablicy z bitmapa	
					movzx rax, BMP_BYTE_COUNT_FOR_ONE_CHAR;w rejestrze eax znajdowac sie bedzie offset do miejsca w tablicy, do ktorego chce zaladowac porzadany bit(r13b).
					mul r15d							;zgodnie z algorytmem, przemnazam iterator zewnetrznej petli przez stala potrzebna do ustalenia miejsca w tablicy do zakodowania bitu(BMP_BYTE_COUNT_FOR_ONE_CHAR * zewnetrzny_iterator)
					add eax, ebx						;zgodnie z algorytmem, dodaje wartosc wewnetrznego iteratora(EAX + wewnetrzny_iterator) 

					mov r12b, [RCX + RAX]				;pobieram z rejestru RCX w którym znajduje siê tablica z bitmap¹ okreslone miejsce dzieki rejestrowi eax
					and r12b, 1							;wykonuje operacje mod2, aby sprawdzic, czy bit musi zostac podmieniany

					cmp r13b, r12b						;porownuje dwa bity, jesli sa takie same, mozna pominac zamiane.
					jne do_operations					;skok pomijajacy operacje podmiany bitu
					
					continue:
					shr r14b, 1
					
					add bl, 3							;inkrementuje licznik wewnetrznej funkcji
					cmp bl, BMP_BYTE_COUNT_FOR_ONE_CHAR
					jle inner_for	;#2jump if less or equal
					jmp inner_loop_end	;skacze na koniec inner petli

					was_zero:
						mov r13b, 1						;sztucznie neguje wartosc rejestru
						jmp continue_encoding			;wracam do procesu kodowania

					do_operations:						;proces kodowania
					;&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
					;ps - caly wektor | ss - skalar(pierwszy element) | cvtsi2ss-konwersja z calkowitego na zmiennoprz

					cvtsi2ss xmm0, r13d					;laduje do rejestr xmm0 najmniej znaczacy bit znaku wiadomosci. Daze do ustawienia w buforze 0|0|LSmB|!LScB (LScB - last significant char bit)
					pshufd xmm0, xmm0, 11100000b		;kopiuje wartosc przy pomocy maski 
					
					cmp r13b, 0							;sprawdzam czy bitem bylo zero, aby ewentualnie moc go zanegowac. w przypadku gdy byla jedynka, negacja jest przedstawiona ponizej
					je was_zero							;skacze do podprogramu, ktory ustawia mi wartosci rejestru na negacje zera.
					
					not r13b							;neguje binarnie zapisany bit 00000001 -> 11111110, niestety operacja 00000000 -> 11111111 nie pozwala na zachowanie 1, wiec zmuszony zostalem skorzystac z podprogramu
					shl r13b, 7							;przesuwam o 7 miejsc otrzymujac negacje bitu z rejestru r13b o wartosci 1
					continue_encoding:
					cvtsi2ss xmm0, r13d					;laduje zanegowany bit do rejestru uzyskujac ustawienie

						xor r11, r11
						;teraz bede przygotowywal okreslony bajt tablicy dodajac 1 lub odejmujac 1
					mov r11b, [RCX + RAX]				;laduje bajt tablicy do zakodowania, w celu przygotowania go do zaladowania do rejestru xmm1
					add r11b, 00000001b
					cvtsi2ss xmm1, r11d
					pshufd xmm1, xmm1, 11100000b		;kopiuje wartosci przy pomocy maski	

					mov r11b, [RCX + RAX]				;to samo co wczesniej ale -1
					sub r11b, 00000001b	
					cvtsi2ss xmm1, r11d
					
					mulps xmm1, xmm0
					haddps xmm1, xmm1

					CVTSS2SI r11d, xmm1
					mov [RCX + RAX], r11b
					jmp continue
					;&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
;---------------- INNER FOR ----------------
	inner_loop_end:

	xor rbx, rbx			;resetuje licznik wewnetrznej petli for (inner_for)
	inc bl				;nastepnie ustawiam go na 1, zgodnie z zalozeniami petli
	inc r15d			;inkrementuje licznik zewnetrznej petli for (outer_for)
	cmp r15d, R8D		;porownuje z mEnd
	jne outer_for	;skacze jesli mniejsze

	
	pop rsp						; Return rsp from stack
	pop rdi						; Return rdi from stack 
	pop rbp						; Return rbp from stack
	
	mov eax, 0
	ret
;---------------- OUTER FOR ----------------	

encoding ENDP
END