/*****************************************************
* Datei: huff.c
* Desc.: Implementiert die Huffman-Codierung
* Autor: Thomas Schmiedel
* Datum: 01.11.2010
* Vers.: 1.0
* 18.07.2011 Strutz assert( maxval > 0)
******************************************************/
#include <string.h>
#include <assert.h>
#include "huff.h"
#include "fibocode.h"

/* Knotenelement erzeugen */
HuffNode *CreateNode(uint weight)
{	
	HuffNode *node = NULL;
	ALLOC(node, 1u, HuffNode);
	memset(node, 0, sizeof(HuffNode));
	node->weight = weight;	
	return node;
}


/* Baumstruktur beginnend mit dem Parent-Knoten löschen */
void DeleteTree(HuffNode *parent)
{
	HuffNode *left  = NULL,
			 *right = NULL;
	if (parent == NULL) return;		
	left  = parent->leftchild;
	right = parent->rightchild;
	FREE(parent);
	DeleteTree(left);
	DeleteTree(right);
}


/* Vergleichsfunktion für qsort,
um kanonische Codes zu erzeugen müssen die Symbole
nach Codewortlängen sortiert werden,
sind diese gleich, wird der Symbolwert herangezogen */
int compare(const void *a, const void *b)
{
	if (((HuffSymbol*)a)->len != ((HuffSymbol*)b)->len)
	{
		return ((HuffSymbol*)a)->len - ((HuffSymbol*)b)->len;
	}
	return ((HuffSymbol*)a)->val - ((HuffSymbol*)b)->val;
}


/* Erzeugt eine assoziative Zuordnung der Huffmann-Bitlänge zum entsprechenden Symbol,
Angenommen das Resultat von GetBitLen wäre bitlenarr, dann würde gelten:
Bitlänge = bitlenarr[Symbol],
maxval ist der größte vorkommende Nachrichtenwert */
uint *GetBitLen(uint *arr, uint len, uint maxval)
{
	/* Vars */	
	uint i, idxmin1, idxmin2, worksize;		 
	uint *weights = NULL,
		 *symlen  = NULL;	
	HuffNode  **nodetable = NULL,
			  **workbench = NULL,
			   *parent    = NULL,
			   *walker    = NULL;		
	
	worksize = maxval + 1u;
	
	/* Symbolhäufigkeiten erfassen */
	ALLOC( weights, worksize, uint);
	memset( weights, 0, worksize * sizeof(uint));
	for (i = 0; i < len; i++) {	weights[arr[i]]++; }	
	
	/* Basisknoten mit berechneten Gewichten 
	erzeugen und in nodetable einordnen */
	ALLOC( nodetable, worksize, HuffNode*);
	memset( nodetable, 0, worksize * sizeof(HuffNode*));
	for (i = 0; i < worksize; i++)
	{
		if (weights[i] != 0) nodetable[i] = CreateNode(weights[i]);	
	}
	FREE(weights);
	
	/* Kopie des nodetable als Arbeitsbereich erzeugen */
	ALLOC( workbench, worksize, HuffNode*);
	memcpy( workbench, nodetable, worksize * sizeof(HuffNode*));
	
	/* Baumstruktur erzeugen */
	for ( ; ; )
	{
		/* Indexwerte der Knoten mit geringstem Gewicht holen,
		zuerst idxmin1 und idxmin2 auf unmöglichen Wert setzen,
		somit kann getestet werden, ob sie verändert worden sind */
		idxmin1 = idxmin2 = worksize;
		for (i = 0; i < worksize; i++)
		{		
			if (workbench[i] == NULL) continue;			
			if (idxmin1 == worksize ||
				workbench[i]->weight < workbench[idxmin1]->weight) idxmin1 = i;			
		}
		for (i = 0; i < worksize; i++)
		{		
			if (workbench[i] == NULL || i == idxmin1) continue;			
			if (idxmin2 == worksize  ||
				workbench[i]->weight < workbench[idxmin2]->weight) idxmin2 = i;			
		}		
		/* Hat der 2. Indexwert keinen Eintrag mehr abbekommen,
		   ist nur noch ein Element im Arbeitsbereich --> Abbruch */
		if (idxmin2 == worksize) break;		
	
		/* Elternknoten erzeugen und diesen in den Arbeitsbereich übernehmen */
		parent = CreateNode(workbench[idxmin1]->weight + workbench[idxmin2]->weight);
		workbench[idxmin1]->parent = parent;
		workbench[idxmin2]->parent = parent;
		parent->leftchild  = workbench[idxmin1];
		parent->rightchild = workbench[idxmin2];		
		workbench[idxmin1] = parent;
		workbench[idxmin2] = NULL;	
	}
	FREE(workbench);	
	
	/* Codelängen holen */
	ALLOC( symlen, worksize, uint);
	memset( symlen, 0, worksize * sizeof(uint));
	/* Unterste Kindknoten durchlaufen */
	for (i = 0; i < worksize; i++)
	{	
		/* Nicht vorhandene Zeichen besitzen keinen Knoten */
		if (nodetable[i] == NULL) continue;		
		/* Schritte	bis zur Baumwurzel zählen, das ist die Codewortlänge */
		for (walker = nodetable[i];
			 walker->parent != NULL;
			 walker = walker->parent, symlen[i]++);			
	}		
	FREE(nodetable);
	DeleteTree(parent);
	//Strutz
	if (worksize == 1) symlen[0] = 1;
	/* Codewortlängenarray zurückliefern */
	return symlen;
}


/* Benötigt eine assoziative Arrayzuordnung von Symbolwert 
und Codewortlänge in bitlenarr und die Größe des Array in len,
dabei ist len um 1 größer, als der Wert des größten Nachrichtensymbols,
liefert eine sortierte kanonische Liste der Codewörter,
außerdem werden nicht vorhandenen Symbole aussortiert */
HuffSymbol *SortAndShrink( uint *bitlenarr, uint *len)
{
	uint i, k, symnum;
	HuffSymbol *symtable = NULL;			   
	
	/* Nicht alle Symbole sind belegt, zähle die Belegten */
	for (i = 0, symnum = 0; i < *len; i++)
	{
		if (bitlenarr[i] != 0) symnum++;
	}
	
	/* Passgenaues Array mit den Codelängen und ihrem Symbolwert anlegen */
	ALLOC( symtable, symnum, HuffSymbol);	
	for (i = 0, k = 0; i < *len; i++)
	{
		if (bitlenarr[i] != 0)
		{
			symtable[k].len  = bitlenarr[i];
			symtable[k].val  = i;						
			k++;	
		}
	}	
	
	/* Sortieren, siehe compare Funktion */
	qsort( symtable, symnum, sizeof(HuffSymbol), compare);
	
	/* Arraygröße aktualisieren */
	*len = symnum;
	return symtable;
}


/* Erzeugt die kanonischen Codewörter */
void InsertCanonicalCodes(HuffSymbol *symbols, uint len)
{	
	uint i;	
	/* Kanonische Codes zuordnen,
	erstes Element besteht nur aus 0len,
	dann immer 1 aufaddieren und sofern
	sich die Codewortlänge geändert hat,
	das aktuelle Muster nach links shiften */
	symbols[0].bits = 0;
	for (i = 1u; i < len; i++)
	{
		symbols[i].bits = (symbols[i - 1u].bits + 1u) << (symbols[i].len - symbols[i - 1u].len);			
	}	
}


/* Wandelt die sortierte Codewortliste in eine assoziative Struktur um,
wobei mit Result[Symbol] direkt auf das Codewort zugegriffen werden kann */
HuffSymbol *ExpandCodes(HuffSymbol *symbols, uint len, uint maxval)
{
	HuffSymbol *result = NULL;
	uint i;
	
	/* Codes wieder in assoziative Struktur überführen */
	ALLOC( result, maxval + 1u, HuffSymbol);
	memset( result, 0, (maxval + 1u) * sizeof(HuffSymbol));
	for (i = 0; i < len; i++)
	{
		result[symbols[i].val].len  = symbols[i].len;
		result[symbols[i].val].bits = symbols[i].bits;	
	}
	return result;
}


/* Speichert die kanonischen Codes,
Symbole werden durch Differenzbildung gespeichert,
ist eine Differenz negativ, wird sie durch eine 0 markiert,
diese kommt natürlicherweise nicht vor, da alle Symbole unterschiedlich sind */
void StoreCodes(HuffSymbol *symbols, uint len, BitBuffer *bitbuf)
{
	uint i;
	int diff;	
		
	/* Erstes Symbol ungekürzt mit Wert und Codewortlänge speichern */
	WriteFiboToStream( bitbuf, symbols[0].val);
	WriteFiboToStream( bitbuf, symbols[0].len);		
	
	for (i = 1u; i < len; i++)
	{
		/* Differenz aus aktuellen und letzten Smbolwert bilden */
		diff = symbols[i].val - symbols[i - 1u].val;
		/* Ist sie negativ, mit 0 markieren */
		if (diff < 0) WriteFiboToStream( bitbuf, 0);
		/* Betrag der Differenz speichern */
		WriteFiboToStream( bitbuf, (uint)abs(diff));
		/* Codewortlängendifferenz speichern */		
		WriteFiboToStream( bitbuf, symbols[i].len - symbols[i - 1u].len);
	}
}


/* Liest die Längenzuordnung aus der Datei */
HuffSymbol *LoadCodes(uint count, BitBuffer *bitbuf)
{
	uint i, val;
	HuffSymbol *result = NULL;
	
	/* Speicher belegen und initiale Werte holen */
	ALLOC( result, count, HuffSymbol);
	result[0].val = GetFiboFromStream( bitbuf);
	result[0].len = GetFiboFromStream( bitbuf);
	
	for (i = 1u; i < count; i++)
	{
		val = GetFiboFromStream( bitbuf);
		if (val == 0) /* Negative Differenz folgt */
		{
			result[i].val = result[i - 1u].val - GetFiboFromStream( bitbuf);	
		}
		else
		{
			result[i].val = result[i - 1u].val + val;
		}
		/* Codewortlänge ermitteln */
		result[i].len = GetFiboFromStream( bitbuf) + result[i - 1u].len;
	}
	return result;
}


/* Nachricht Codieren */
void HuffEncode(uint *arr, uint len, BitBuffer *bitbuf)
{
	uint *lenarray = NULL;
	HuffSymbol *shrinked  = NULL,
			   *finalcode = NULL;
	uint tmpsize, i, tmplen, maxval, lmax;
	int k;
	
	/* Größtes Zeichen finden */
	for (i = 0, maxval = 0; i < len; i++)
	{
		if (arr[i] > maxval) maxval = arr[i];
	}
	/* Codelängen der vorkommenden Symbole bestimmen */
	lenarray = GetBitLen( arr, len, maxval);
	tmpsize  = maxval + 1u;
	/* Nicht vorkommende Symbole ausschließen und 
	die Übrigen nach Codewortlänge sortieren,
	tmpsize wird angepasst */
	shrinked = SortAndShrink( lenarray, &tmpsize);
	FREE( lenarray);		
	
	/* get lmax */
	lmax = shrinked[tmpsize - 1u].len;
	//fprintf( stderr, "(lmax=%u)", lmax);
	
	/* Anzahl der Elemente und Längenzuordnung speichern */	
	WriteFiboToStream( bitbuf, tmpsize);	
	StoreCodes( shrinked, tmpsize, bitbuf);
	/* Kanonische Codes erzeugen */
	InsertCanonicalCodes( shrinked, tmpsize);
	/* ...und wieder in assoziative Struktur überführen */
	finalcode = ExpandCodes( shrinked, tmpsize, maxval);
	FREE( shrinked);
	
	/* Codieren */
	for (i = 0, tmplen = 0; i < len; i++)
	{
		tmplen = finalcode[arr[i]].len;
		for (k = finalcode[arr[i]].len - 1; k >= 0; k--)
		{
			BufferPutBit( bitbuf, ((finalcode[arr[i]].bits >> k) & 1u) ? TRUE : FALSE);			
		}			
	}	
	
	/* lmax auffüllen, wichtig für Dekodierung */
	for (i = 0; i < lmax - tmplen; i++)
	{
		BufferPutBit(bitbuf, FALSE);
	}
	
	FREE(finalcode);	
}


/* Dekodierung aus Bitstream */
void HuffDecode(uint *arr, uint len, BitBuffer *bitbuf)
{
	HuffSymbol *shrinked = NULL;			   
	uint i, tmpsize, lmax, mask, 
		 buffer, pos, freepos;
	uint *shiftedvals = NULL;
	int k;
	
	/* Anzahl der gespeicherten Symbol-Längen-Paare holen */
	tmpsize   = GetFiboFromStream( bitbuf);
	/* Die Menge aus Datei lesen */
	shrinked  = LoadCodes( tmpsize, bitbuf);	
	/* Kanonische Codes erzeugen */
	InsertCanonicalCodes( shrinked, tmpsize);
	/* lmax ermitteln */	
	lmax = shrinked[tmpsize - 1u].len;	
	/* mask enthält beginnend mit dem LSB 1er Bits,
	und davon soviele wie lmax angibt */
	for (mask = 0, i = 0; i < lmax; i++)
	{
		mask |= (1u << i);
	}
	
	/* Kürzere Codewörter als lmax, werden um den 
	Differenzbetrag nach links geshiftet */
	ALLOC(shiftedvals, tmpsize, uint);
	for (i = 0; i < tmpsize; i++)
	{
		shiftedvals[i] = shrinked[i].bits << (lmax - shrinked[i].len);						
	}
	
	/* Decoder Loop */
	for (i = 0, pos = 0, buffer = 0; i < len; )
	{
		/* Speicherorganisation:
		"buffer" ist der Bit-Zwischenspeicher,
		pos ist die Leseposition im Puffer,
		ist sie kleiner als lmax, werden neue Bits geladen */
		if (pos < lmax) 
		{
			/* Freie Stellen kalkulieren */
			freepos = lmax - pos;
			/* Pufferinhalt nach links verschieben,
			rechts entsteht neuer Platz,
			dann alle Stellen außerhalb lmax abschneiden */ 
			buffer <<= freepos;
			buffer &= mask;
			/* Freigewordene Stellen auffüllen */
			for (k = freepos - 1u; k >= 0; k--)
			{
				if (BufferGetBit(bitbuf) == TRUE)
				{
					buffer |= (1u << k);
				}
				else
				{
					buffer &= ~(1u << k);
				}
			}
			/* Puffer wieder voll */
			pos = lmax;		
		}	
		
		/* Liegt numerische Interpretation von "buffer" zwischen
		zwei Werten (einschließlich des unteren), die aus den 
		geshifteten Codewörtern gebildet werden können,
		so ist ein Symbol gefunden. Es wird ausgegeben und
		von der Bufferposition die reale Codewortlänge 
		des passenden Wertes abgezogen */ 		
		for (k = tmpsize - 1u; k >= 0; k--)
		{
			if (buffer >= shiftedvals[k])
			{
				arr[i++] = shrinked[k].val;
				pos -= shrinked[k].len;					
				break;
			}		
		}		
	}
	
	FREE(shiftedvals);	
	FREE(shrinked);
}

