/*****************************************************
* Datei: huff.h
* Autor: Thomas Schmiedel
* Datum: 01.11.2010
* Desc.: Header für Huffman-Codierung
* Vers.: 1.0
******************************************************/

#ifndef _HUFF_H_
#define _HUFF_H_

#include "tstypes.h"
#include "bitbuf.h"

/* Knoten im Huffmanbaum,
enthält sein Wertigkeit und
Zeiger auf Kindelemente und Elternnoten */
typedef struct _node
{
	uint weight;
	struct _node *parent,
				 *leftchild,
				 *rightchild;
} HuffNode;

/* Container für ein Zeichen
inklusive seines Huffman-Codes
und seiner Bitlänge */
typedef struct
{
	uint len, val;
	uint bits;
} HuffSymbol;

/* Knoten erzeugen und kompletten Baum löschen */
HuffNode *CreateNode(uint);
void DeleteTree(HuffNode *);

/* Codieren und Decodieren einer Nachricht */
uint *GetBitLen(uint *, uint, uint);
HuffSymbol *SortAndShrink(uint *, uint *);
void HuffEncode(uint *, uint, BitBuffer *);
void HuffDecode(uint *, uint, BitBuffer *);
void InsertCanonicalCodes(HuffSymbol *, uint);
HuffSymbol *ExpandCodes(HuffSymbol *, uint, uint);
void StoreCodes(HuffSymbol *, uint, BitBuffer *);
HuffSymbol *LoadCodes(uint, BitBuffer *);

#endif

