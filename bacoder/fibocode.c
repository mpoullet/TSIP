/*****************************************************
* Datei: fibocode.c
* Autor: Thomas Schmiedel
* Datum: 01.11.2010
* Desc.: Implementierung des Fibonacci-Coders
* Vers.: 1.0
******************************************************/


#include "fibocode.h"
#include "tstypes.h"
#include "bitbuf.h"
#include <string.h>


/* Bildet den Fibonacci-Code einer Ganzzahl */
FiboCode *WriteFibo(uint number)
{
	FiboCode *fico = NULL;
	uint i, sum, old, *lookup, looklen;	
		
	number++; /* Null nicht erlaubt */	
	
	sum = 1u;
	old = 1u;
	looklen = 0;	
	
	/* Anzahl der benötigten Glieder der
	Fibonacci-Folge bis zur geg. Zahl ermitteln */
	while (sum <= number)
	{	
		looklen++;
		sum += old;
		old = sum - old;
	}
	
	/* LookUp Table allokieren, sie nimmt
	die Werte der F.-Folge auf */
	ALLOC( lookup, looklen, uint);	
	sum = 1u;
	old = 1u;
	i   = 0;
	
	/* LookUp Table befüllen, die 1 kommt nur einmal vor */
	while (sum <= number)
	{	
		lookup[i++] = sum;
		sum += old;
		old =  sum - old;
	}
	
	/* Rückgabecontainer anlegen und mit 0 befüllen */
	ALLOC( fico, 1u, FiboCode);
	ALLOC( fico->code, looklen + 1u, cbool);
	memset( fico->code, 0x00, (looklen + 1u) * sizeof(cbool));
	
	/* Endekennzeichen in Form eines 1er Bits setzen */
	fico->code[looklen] = TRUE;
	fico->count = looklen + 1u;
	
	/* Codewort erzeugen, dazu:
	- größte Fibonacci Zahl suchen, die in "number" passt
	- das Bit auf TRUE setzen, welches dem Index dieser 
	Fibonacci-Zahl in der Lookup Table entspricht
	- die gefundenen F.-Zahl von "number" subtrahieren
	- das Verfahren auf den übrigen Wert wiederholen */
	for (i = looklen - 1u; i >= 0 && number > 0; i--)
	{	
		if (lookup[i] <= number)
		{
			number -= lookup[i];
			fico->code[i] = TRUE;		
		}		
	}
	
	FREE( lookup);
	
	return fico;
}


/* Fibonacci-Code in vorzeichenlose Ganzzahl zurückwandeln */
uint RestoreFiboNum(FiboCode *fico)
{
	uint i, sum, old, result;
	
	sum = 1u;
	old = 1u;
	result = 0;
	i = 0;
	
	/* Solange nicht zwei 1er aufeinandefolgen:
	- Fibonacci-Folge bilden
	- Ist ein Bit TRUE, so den aktuellen F.-Wert
	auf "result" aufaddieren
	- Zwei 1er markieren das Ende des Codewortes */	
	while (TRUE)
	{	
		if (fico->code[i] == TRUE)
		{
			result += sum;
			if (fico->code[i + 1u] == TRUE)
			{
				break;
			}
		}
		sum += old;
		old = sum - old;		
		i++;
	}
	
	result--; /* Analog zur Codierung, da 0 nicht erlaubt */
	return result;
}


/* Schreibt das Fibonacci-Codewort in den Bitstrom */
void WriteFiboToStream( BitBuffer *bitbuf, uint number)
{
	uint i;

	FiboCode *fico = NULL;	
	fico = WriteFibo( number);	
		
	for (i = 0; i < fico->count; i++)
	{
		BufferPutBit( bitbuf, fico->code[i]);	
	}

	DeleteFibo( fico);
}


/* Holt eine Fibonacci-Codierte Ganzzahl aus dem Bitstrom */
uint GetFiboFromStream( BitBuffer *bitbuf)
{
	uint  sum, old, result;
	cbool lastbit, curbit;
	
	sum = 1u;
	old = 1u;
	result = 0;	
	lastbit = FALSE;
	
	/* Solange nicht zwei 1er aufeinandefolgen:
	- Fibonacci-Folge bilden
	- Ist ein Bit TRUE, so den aktuellen F.-Wert
	auf "result" aufaddieren
	- Zwei 1er markieren das Ende des Codewortes */	
	while (TRUE)
	{	
		curbit = BufferGetBit(bitbuf);
		if (curbit == TRUE)
		{
			if (lastbit == TRUE)
			{
				break;
			}
			result += sum;			
		}		
		lastbit = curbit;
		sum += old;
		old = sum - old;		
	}
	
	result--; /* Analog zur Codierung, da 0 nicht erlaubt */
	return result;
}


/* Container für Fibonacci-Code freigeben */
void DeleteFibo( FiboCode *fico)
{
	FREE( fico->code);
	FREE( fico);
}


