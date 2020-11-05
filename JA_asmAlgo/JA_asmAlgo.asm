;Bitmap Steganography, version 0.0
;Author: Adam Burzyñski
;Subject: Jezyki Assemblerowe
;Year: 2020/2021
;Compiler: MASM

.data 
BMP_BYTE_COUNT_FOR_ONE_CHAR BYTE 24		;stala okreslajaca ilosc bajtow obrazu BMP potrzebnych do zakodowania jednego znaku (bajtu) wiadomosci.
										;poniewaz zamieniamy LSB co trzeci bajt, poniewaz kodujemy tylko na kolorze zielonym, potrzeba 24 bajty -
										;1 bit kodujemy w b[G]r = 3 bajty.
.code									;czesc .code odpowiada za kod 
;======================== DECODING ========================
decoding PROC
;_____ Parametry przekazywane do procedury: RCX - bmpArr | RDX - msgArr | R8 - mEnd _____


push RBP		; zapisuje adresy rejestrow RBP,RDI,RSP, aby po wykonaniu procedury, zachowac spojnosc w pamieci
push RDI				
push RSP				


mov R9, RDX		;zapisuje wskaznik na tablice z wiadomoscia, aby moc uzywac instrukcji mul, ktora potrzebuje rejestru RDX do poprawnego dzialania
xor RBX, RBX	;licznik wewnetrznej petli (inner_for) zapisany w rejestrze al, ktora w cpp wyglada tak: for(int i=1; i<=24; i+=3)
inc BL			;inkrementujê rejestr al, aby zgodnie z zalozeniami byl rowny 1

xor R15,R15		;licznik zewnetrznej petli (outer_for). Petla w cpp wyglada tak: for(int i=0; i<mEnd; i++), gdzie mEnd to wartosc przechowywana w rejestrze R15
	
;---------------- OUTER FOR ----------------		
outer_for:	;etykieta zewnetrznej etykiety-petli for
	;---------------- INNER FOR ----------------		
xor R13, R13	;przygotowuje rejestr w ktorym bede skladowal odkodowane dane, budujac z nich znaki (1 bajt).

	inner_for:	;etykieta wewnetrznej etykiety-petli for 
				;petla wyglada w cpp tak: for(int i=1; i<=24; i+=3) poniewaz kodujemy co trzeci bajt pliku BMP -> 1 bajt wiadomosci = 24 bajty pliku bmp.
	;obliczam index tablicy z bitmapa	
	movzx RAX, BMP_BYTE_COUNT_FOR_ONE_CHAR ;w rejestrze eax znajdowac sie bedzie obliczony indeks tablicy, do ktorego chce zaladowac porzadany bit(r13b).
	mul R15d		;zgodnie z algorytmem, przemnazam iterator zewnetrznej petli przez stala | EAX = (BMP_BYTE_COUNT_FOR_ONE_CHAR * zewnetrzny_iterator) 
	add EAX, EBX	;zgodnie z algorytmem, dodaje wartosc wewnetrznego iteratora	(EAX + wewnetrzny_iterator) 

	mov R12b, [RCX + RAX]	;pobieram z rejestru RCX w którym znajduje siê tablica z bitmap¹ okreslone miejsce dzieki rejestrowi EAX
	and R12b, 1				;wykonuje operacje mod2, aby sprawdzic wartosc bitu - jesli =1, trzeba dodac 128

							;poniewaz kodowalem od prawej do lewej, tak teraz dekodujac musze to robic od lewej do prawej.
							;dlatego jesli zdekodowany bit = 1, nalezy dodac 128(10000000) do wynikowego rejestru, a nastepnie przesunac go binarnie o 1 w prawo
							;nalezy zwrocic uwage, ze do zdekodowania 8 bitow, potrzeba 7 przesuniec
	and R13b, 01111111b		;zeruje pierwszy bit przy pomocy maski 01111111, aby przygotowac miejsce dla odkodowanego bitu
	mov AL, 10000000b		;obliczam dzieki rejestrowi AL warosc, ktora nastepnie zostanie dodana 
	mul R12d				;przemnazam zdekodowany bit razy 128(10000000), aby zdecydowac, czy do bajtu wynikowego bede dodawal 1 czy 0
	add R13d, EAX			;dodaje na poczatek albo 1 albo 0, zaleznie od tego jaka wartosc ma zdekodowany bit.

	cmp BL, 22				;sprawdzam, czy wewnetrzny iterator nie jest rowny 22, poniewaz potrzeba 7 przesuniec do odkodowania 8 bitow.
	jne bit_right_shift		;skok do etykiety przesuwajacego o 1 w prawo bity rejestru zawierajacego dekodowane slowo 

	continue:				;etykieta pozwalajaca wrocic z etykiety przesuwajacej bity w prawo o 1
					
	add BL, 3								;inkrementuje licznik wewnetrznej etykiety o 3, poniewaz dekodujemy co 3 bajt. 
											;Ta wartosc jest uzywana w okresleniu indeksu tablicy z ktorej pobieramy dane do dekodowania.
	cmp BL, BMP_BYTE_COUNT_FOR_ONE_CHAR		;sprawdzam warunek konczacy petle (iterator <= 24 (BMP_BYTE_COUNT_FOR_ONE_CHAR))
	jle inner_for							;po sprawdzeniu warunku skoku wewnetrznej petli, skaczemy do kolejnej iteracji, badz tez wychodzimy z niej, przechodzac do kolejnej iteracji zewnetrznej petli
	jmp inner_loop_end						;jesli warunek nie zostal spelniony, wychodzimy z wewnetrznej petli, przechodzac do zewnetrznej, gdzie nastepnie zostanie sprawdzany warunek petli.

	bit_right_shift:		;etykieta ktorego zadaniem jest przesuniecie bitowe dekodowanego slowa o 1 w prawo
	shr r13b, 1				;operacja przesuniecia bitowego dekodowanego slowa o 1 w prawo
	jmp continue			;powrot do etykiety wewnetrznej petli

;---------------- INNER FOR ----------------
	inner_loop_end:
mov [R9 + R15], R13b	;laduje zdekodowane slowo do tablicy z wiadomoscia pod odpowiednim indeksem

xor RBX, RBX		;resetuje licznik wewnetrznej etykiety-petli for (inner_for)
inc BL				;nastepnie ustawiam go na 1, zgodnie z zalozeniami petli
inc R15d			;inkrementuje licznik zewnetrznej etykiety-petli for (outer_for)
cmp R15d, R8d		;porownuje z mEnd
jne outer_for	;skacze jesli mniejsze
	
pop rsp		;z koncem programu ustawiam spowrotem wartosci rejestrow pobierajac je ze stosu.
pop rdi						
pop rbp						
	
mov eax, 0	;ustawiam wartosc, ktora zwroci procedura
ret			;koniec procedury
;---------------- OUTER FOR ----------------	


decoding ENDP
;======================== DECODING ========================

;======================== ENCODING ========================
encoding PROC	
;_____ Parametry przekazywane do procedury: RCX - bmpArr | RDX - msgArr | R8 - mEnd _____
	
push RBP		; zapisuje adresy rejestrow RBP,RDI,RSP, aby po wykonaniu procedury, zachowac spojnosc w pamieci
push RDI				
push RSP	

mov R9, RDX		;zapisuje wskaznik na tablice z wiadomoscia, aby moc uzywac instrukcji mul, ktora potrzebuje rejestru RDX do poprawnego dzialania
xor RBX, RBX	;licznik wewnetrznej petli (inner_for) zapisany w rejestrze al, ktora w cpp wyglada tak: for(int i=1; i<=24; i+=3)
inc BL			;inkrementujê rejestr al, aby zgodnie z zalozeniami byl rowny 1

xor R15,R15		;licznik zewnetrznej petli (outer_for). Petla w cpp wyglada tak: for(int i=0; i<mEnd; i++), gdzie mEnd to wartosc przechowywana w rejestrze R15
;---------------- OUTER FOR ----------------		
outer_for:	
;---------------- INNER FOR ----------------		
mov R14b, [R9 + R15]	;czytam przy pomocy iteratora zewnetrznej petli (RBX) jeden znak (bajt) z wiadomosci.
	inner_for:			;etykieta wewnetrznej etykiety-petli for 
						;petla w cpp: for(int i=1; i<=24; i+=3) poniewaz kodujemy co trzeci bajt pliku BMP -> 1 bajt wiadomosci = 24 bajty pliku bmp.				
	mov R13b, R14b			;zapisuje r13, aby nie stracic bajtu z r14. Bajt zostanie potraktowany przesunieciem bitowym w lewo o 1 z koncem petli. 
	and R13b, 1				;wykonujê operacjê mod2, aby odczytaæ bit o najmniejszym znaczeniu bajtu, który nastêpnie zostanie zakodowany w obrazie. 

	;obliczam index tablicy z bitmapa
	movzx RAX, BMP_BYTE_COUNT_FOR_ONE_CHAR	;w rejestrze eax znajdowac sie bedzie obliczony indeks tablicy, do ktorego chce zaladowac porzadany bit(r13b).
	mul R15d		;zgodnie z algorytmem, przemnazam iterator zewnetrznej petli przez stala | EAX = (BMP_BYTE_COUNT_FOR_ONE_CHAR * zewnetrzny_iterator) 
	add EAX, EBX	;zgodnie z algorytmem, dodaje wartosc wewnetrznego iteratora	(EAX + wewnetrzny_iterator) 

	mov R12b, [RCX + RAX]	;pobieram z rejestru RCX w którym znajduje siê tablica z bitmap¹ okreslone miejsce dzieki rejestrowi EAX
	and R12b, 1				;wykonuje operacje mod2, aby sprawdzic, czy bit musi zostac podmieniany

	cmp r13b, r12b			;porownuje dwa bity, jesli sa takie same, mozna pominac zamiane.
	jne do_operations		;skok do etykiety ktorej zadaniem jest wykonanie operacji podmiany bitow
					
	continue:				;etykieta sluzaca powrotowi do petli
	shr R14b, 1				;przesuwam bity w rejestrze, zawierajacym znak do zakodowania. 
					
	add BL, 3								;inkrementuje licznik wewnetrznej etykiety o 3, poniewaz dekodujemy co 3 bajt. 
											;Ta wartosc jest uzywana w okresleniu indeksu tablicy do zakodowania znakow.
	cmp BL, BMP_BYTE_COUNT_FOR_ONE_CHAR		;sprawdzam warunek konczacy petle (iterator <= 24 (BMP_BYTE_COUNT_FOR_ONE_CHAR))
	jle inner_for							;po sprawdzeniu warunku skoku wewnetrznej petli, skaczemy do kolejnej iteracji, badz tez wychodzimy z niej, przechodzac do kolejnej iteracji zewnetrznej petli
	jmp inner_loop_end						;jesli warunek nie zostal spelniony, wychodzimy z wewnetrznej petli, przechodzac do zewnetrznej, gdzie nastepnie zostanie sprawdzany warunek petli.

	
	do_operations:	;etykieta kodujaca wiadomosc w bitmapie. Wykorzystuje tutaj operacje wektorowe.
					;ps - caly wektor | ss - skalar(pierwszy element) | cvtsi2ss-konwersja z calkowitego na zmiennoprz

	cvtsi2ss xmm0, R13d				;laduje do rejestr xmm0 najmniej znaczacy bit znaku wiadomosci. Daze do ustawienia w buforze 0|0|LSmB|!LScB (LScB - last significant char bit)
	pshufd xmm0, xmm0, 11100000b	;kopiuje wartosc przy pomocy maski 11100000, otrzymujac zawartosc rejestru xmm0: 0|0|R13d|R13d
					
	cmp R13b, 0				;sprawdzam czy bitem bylo zero, aby moc zanegowac wartosc rejestru R13b, czyli LSB bajtu do zakodowania. 
							;w przypadku gdy byla jedynka, neguje wartosc rejestru R13b, w osobnej etykiecie.
	je was_zero				;skacze do etykiety, ktora ustawia wartosci rejestru na jego negacje, czyli zero -> jeden.
							;ponizej neguje wartosc rejestru, ktora dziala tylko w przypadku, gdy w rejestrze bylo 1. 
					
	not r13b				;w przypadku, gdy w rejestrze jest 1, neguje binarnie zapisany bit 00000001 -> 11111110, a nastepnie przesuwam binarnie o 7 miejsc
							;niestety operacja 00000000 -> 11111111 nie pozwala na uzyskanie 1 (operacja daje 128), wiec zmuszony zostalem skorzystac z osobnej etykiety do tego przeznaczonej (was_zero)
	shl r13b, 7				;przesuwam o 7 miejsc otrzymujac negacje bitu z rejestru r13b o wartosci 1
	continue_encoding:		;etykieta sluzaca do powrotu z etykiety was_zero. 
	cvtsi2ss xmm0, r13d		;laduje zanegowany bit do rejestru uzyskujac porzadane ustawienie rejestru xmm0: 0|0|R13d|!R13d

	;= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
	;ZAMYSL KODUJACYCH OPERACJI WEKTOROWYCH:
	;rejestr xmm0 zawiera: 0|0|	   LSB		|	    !LSB		|;	LSB - last significant bit znaku do zakodowania
	;rejestr xmm1 zawiera: 0|0| bmp[index] + 1	|   bmp[index]-1	|
	;jestli LSB = 1 (a wiemy z warunku sprawdzajacego rownosc ostatnich bitow bajtow, ze bmp[index] ma na koncu 0), 
	;to oznacza, ze zakodowany bajt musi byc rowny bmp[index]+1.
	;Nastepnie przemnazamy instrukcj¹ MULPS rejestry xmm1 oraz xmm0 otrzymujac wartosc tablicy do zakodowania. 
	;W wyniku mnozenia dostajemy: 
	;(gdy LSB = 1)-> xmm1 : 0|0| bmp[index] + 1	|        0
	;(gdy LSB = 0)-> xmm1 : 0|0|	    0		| bmp[index] - 1 
	;Na koniec sumujemy wartosci rejestru xmm1 instrukcja HADDPS, otrzymujac: xmm0: 0|0|0|bmp[index] +/- 1
	;= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

	xor R11, R11			;czyszcze calkowicie rejestr R11, aby pozbyc sie jakichkolwiek napisanych w nim danych - potencjalnych powodow bledow.
							;teraz bede przygotowywal bajty tablicy dodajac 1 lub odejmujac 1
	mov R11b, [RCX + RAX]	;laduje bajt tablicy do zakodowania, w celu przygotowania go do zaladowania do rejestru xmm1
	add R11b, 00000001b		;dodaje do niego 1, aby w nastepnym kroku odjac od niego 1. 
	cvtsi2ss xmm1, R11d		;laduje do rejestru xmm1 wartosc tablicy powiekszone o 1 (bmp[index] + 1)
	pshufd xmm1, xmm1, 11100000b	;kopiuje wartosci przy pomocy maski 11100000, otrzymujac w rezultacie zawartosc rejestru xmm1: 0|0|	bmp[index] + 1|bmp[index] + 1

	mov R11b, [RCX + RAX]	;ponownie sczytuje bajt tablicy, do zakodowania, z tym ze tym razem bede odejmowal od niego 1. 
	sub R11b, 00000001b		;odejmuje od bajtu 1 orzymujac zawarosc rejestru R11b: bmp[index] - 1
	cvtsi2ss xmm1, R11d		;laduje dane do rejestru xmm1, otrzymujac jego zawartosc rowna: 0|0|	bmp[index] + 1|bmp[index] - 1
					
	mulps xmm1, xmm0		;nastepnie wymnazam przez siebie rejestry otrzymujac porzadana wartosc zakodowanego bajtu
	haddps xmm1, xmm1		;dodaje do siebie wartosci rejestru xmm1

	CVTSS2SI R11d, xmm1		;przekazuje wyliczona wartosc, do rejestru R11d.
	mov [RCX + RAX], R11b	;laduje zakodowane dane do odpowiedniego indeksu tablicy bitmapy
	jmp continue			;wracam do wewnetrznej petli(inner_for)
		
	was_zero:	;etykieta sluzaca do negacji wartosci rejestru R13b, w przypadku, gdy byla w nim wartosc = 0			
	mov R13b, 1		;"sztucznie" neguje wartosc rejestru, wiecac, ze bylo w nim 0
	jmp continue_encoding	;wracam do procesu kodowania

;---------------- INNER FOR ----------------
	inner_loop_end:

xor RBX, RBX		;resetuje licznik wewnetrznej etykiety-petli for (inner_for)
inc BL				;nastepnie ustawiam go na 1, zgodnie z zalozeniami petli
inc R15d			;inkrementuje licznik zewnetrznej etykiety-petli for (outer_for)
cmp R15d, R8d		;porownuje z mEnd
jne outer_for	;skacze jesli mniejsze
	
pop RSP		;z koncem programu ustawiam spowrotem wartosci rejestrow pobierajac je ze stosu.
pop RDI						
pop RBP						
	
mov eax, 0	;ustawiam wartosc, ktora zwroci procedura
ret			;koniec procedury
;---------------- OUTER FOR ----------------	

encoding ENDP
;======================== ENCODING ========================
END