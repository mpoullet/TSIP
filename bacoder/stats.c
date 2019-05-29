/*****************************************************
* File..: stats.c
* Desc.: Implementiert Statistik-Funktionen
* Author: Thomas Schmiedel
* Date.: 02.11.2010, 31.01.2011 (Strutz)
* 30.05.2011 Strutz: PCA
* 08.07.2011 new: PicEntropy_P() including simple prediction
* 23.01.2012 GetCorrelation() and GetRoughness() speed optimised
* 07.02.2012 new: getMonotony()
* 09.02.2012 new: GetNumColours() with maxCount parameter
* 10.02.2012 new: heap_sort_ull()
* 02.08.2013 new HowSynthetic()
* 07.11.2013 bugfix in getMonotony() steps must be > 0
* 04.02.2014 bugfix in GetCorrelation() avoid negative result
* 25.02.2014 new: getColourSaturation()
* 25.02.2014 new: getColourContrast()
* 09.07.2014 exclude possible alpha channel from computation
 * 10.10.2014 new getHaarFeatures()
 * 21.11.2014 bugfix in getHaarFeatures() prevent division by zero
 * 06.05.2015 Indexed2RGBColours() and GetIndexedColours()
 *            moved to palette_coding.c
 * 16.11.2015 new: heap_sort_ll()
 * 04.05.2016 in GetNumColours() compute colour entropy
 * 28.08.2016 new GetPatEntropy() 
******************************************************/
#include <math.h>
#include <string.h>
#include <assert.h>

#include "stats.h"
#include "med.h"	/* for CLIP()	*/
#include "colour4.h"
#include "paeth.h"
#include "matrix_utils.h"
#include "prototypes.h"
#include "eig3.h"
#include "codec.h"	/* needed for max()	*/
#include "urle.h"
#include "ubwt.h"

#ifndef M_PI
	#define M_PI        3.14159265358979323846
#endif
#define LOG2	0.69314718	/* used for entropy calculation	*/
#define invLOG2	1.442695041  /* 1 / ln(2)  */

unsigned long blocklen_long  = 0;
unsigned long 	 *blockptr_long = NULL;

PicData *CopyPicData(PicData *);

/*---------------------------------------------------------------
 *	RLCEncode() only for getRepeatness()
 *---------------------------------------------------------------*/
unsigned long RLCEncode( unsigned long *arr, unsigned long len, 
												uint marker, uint max_run)
{
	unsigned long *tmp = NULL;
	unsigned long i, rle, pos;

	ALLOC( tmp, len, unsigned long);
	/* Zeichen der Nachricht durchgehen */	
	for (i = 0, pos = 0; i < len; )
	{
		/* Lauflänge zählen */
		for (rle = 1u;			 
			 i + rle < len &&	
			 rle < UINT_MAX && 		
			 (rle < max_run) &&  //this is surprisingly worse than  '< marker'
			 arr[i + rle] == arr[i];
			 rle++);
		if (rle >= 4u) /* RLE verwenden */
		{
			/* Markierung, Symbol und Anzahl schreiben */
			tmp[pos++] = marker;			
			tmp[pos++] = arr[i]; /* symbol before run length: important for BWT */
			tmp[pos++] = rle - 4u;
			/* Position in der Nachricht um "rle" weiterrücken */		
			i += rle;
		}
		else
		{
			/* Symbol(e) unverändert übernehmen */
			do
			{
				tmp[pos++] = arr[i];
				rle--;
				i++; /* Ein Symbol weiter, bitte */
			}while(rle);
		}	
	}
	/* tmp in arr rückkopieren */
	memcpy( arr, tmp, pos * sizeof(unsigned long));
	FREE( tmp);
	return pos;	
}

/* Vergleichsfunktion für zwei beliebige Rotationen
der Originalnachricht.
Beide werden mit dem ersten Zeichen (ihrer Rotation) beginnend
verglichen. Sind sie gleich, wird das nächst-rechte
Zeichen herangezogen usw.*/
int comparesym_long( const void *a, const void *b)
{	 
	register unsigned long count;
  register unsigned long *pa, *pb;

	pa = &blockptr_long[*(unsigned long *)a];
	pb = &blockptr_long[*(unsigned long *)b];
	count = blocklen_long;
	while( (*pa == *pb) && count)
	{
		 pa++;
		 pb++;
		 count--;
	}
	if (*pa > *pb)	return 1;
	else if (*pa < *pb)	return -1;
	return 0; /* happens only if all symbols in blockptr are equal */
}


/*---------------------------------------------------------------
 *	BWTEncode_long() only for getRepeatness()
 *---------------------------------------------------------------*/
unsigned long BWTEncode_long( unsigned long *arr, unsigned long len)
{
	unsigned long i, original;
	unsigned long 	 *tmp = NULL;	
	unsigned long *lineindex = NULL;
		 
	ALLOC( tmp, 2u * len, unsigned long);
	memcpy( tmp, arr, len * sizeof(unsigned long));
	memcpy( &tmp[len], arr, len * sizeof(unsigned long));
	
	/* lineindex ist ein Array, welches
	die Indizes der Startzeichen
	der einzelnen Rotationen enthält,
	die Zeichen selbst sind in der
	Originalnachricht */
	ALLOC( lineindex, len, unsigned long);
	for (i = 0; i < len; i++)
	{
		lineindex[i] = i;
	}	

	/* Siehe oben, globale Var. für 
	Compare-Funktion */
	blocklen_long = len;
	blockptr_long = tmp;	
		
	fprintf( stderr, "\n start qsort()");
	/* lineindex, also die Rotationen sortieren */	
	qsort( lineindex, len, sizeof(unsigned long), comparesym_long);
	
	/* Index der Originalnachricht
	im Rotationsblock finden */
	for (original = 0;		 
		 lineindex[original] != 0;
		 original++);			 
	
	/* Jeweils die letzten Zeichen jeder
	Rotation in arr übernehmen */
	for (i = 0; i < len; i++)
	{
		arr[i] = tmp[lineindex[i] + len - 1u]; 
	}			
	FREE( tmp);
	FREE( lineindex);
	/* Index der Originalnachricht im Block zurückliefern */	
	return original;
}


#ifdef NIXX
/*---------------------------------------------------------------
 *	getHaarFeatures()
 *---------------------------------------------------------------*/
void getHaarFeatures( PicData *pd, HAARFEAT *features)
{
	long x, y, v1, v2; /* x, y must be signed due to h-1 and w-1 for very small images	*/
	double	sum_AD, sum_DA, sum_DD, fac;
	unsigned int numChan, w, h, comp;
	unsigned long pos, py, posA, pyA;
	unsigned long H_DA1[256], H_DD1[256], H_AD1[256];

	PicData *pd2 = NULL;
	
	pd2 = CopyPicData( pd);


	/* exclude possible alpha channel	*/
	if (pd->channel == 2) numChan = 1;
	else if (pd->channel == 4) numChan = 3;
	else numChan = pd->channel;

	w = pd->width;
	h = pd->height;


	for ( x = 0; x < 256; x++)
	{
		H_DA1[x] = H_DD1[x] = H_AD1[x] = 0;
	}

	/* first transform	*/
	sum_AD = sum_DA = sum_DD = 0;
	for (comp = 0; comp < numChan; comp++)
	{
		/* first horizontal transform	*/
		/* for all rows excluding last	*/
		for (y = 0, py = 0; y < (signed)h-1; y++, py += pd->width)
		{
			/* for every second column excluding last	*/
			for (x = 0, pos = py; x < (signed)w - 1; x+=2, pos+=2)
			{
				v1 = pd->data[comp][pos];
				v2 = pd->data[comp][pos+1];
				pd2->data[comp][pos] = (v1 + v2) >> 1;
				pd2->data[comp][pos+1] = (v1 - v2);
			}
		}

		/* first vertical transform	*/
		/* for every second row excluding last	*/
		for (y = 0, py = pyA = 0; y < (signed)h-1; 
					y+=2, pyA += pd->width, py+=2*pd->width)
		{
			/* for every second column excluding last	*/
			for (x = 0, pos = py, posA = pyA; x < (signed)w - 1; x++, pos++, posA++)
			{
				v1 = pd2->data[comp][pos];
				v2 = pd2->data[comp][pos+pd->width];
				if (x%2)
				{
					v1 = (v1 + v2) >> 1;
					v2 = (v1 - v2);
					sum_DA += abs( v1); /* DA signal	*/
					sum_DD += abs( v2); /* DD signal	*/
					v1 = CLIP(v1 + 128,0,255);
					H_DA1[v1]++;
					v2 = CLIP(v2 + 128,0,255);
					H_DD1[v2]++;
				}
				else
				{
					/* AA signal, keep for next decomposition	*/
					pd2->data[comp][posA] = (v1 + v2) >> 1; 
					sum_AD += abs( v1 - v2); /* AD signal	*/
					v1 = CLIP( v1 - v2 + 128, 0, 255);
					H_AD1[v1]++;
				}
			}
		} /* for y */
	} /* for comp */

	w = w >> 1;
	h = h >> 1;
	/* arithmetic mean normalised to a max. value of 200	*/
	fac = 1000. / (numChan * w * h * (1 << pd->bitperchannel) );
	features->sum_AD1 = (int)min( 200, floor( sum_AD * fac)); 
	features->sum_DD1 = (int)min( 200, floor( sum_DD * fac)); 
	features->sum_DA1 = (int)min( 200, floor( sum_DA * fac)); 

			/* do some statistics	*/
	{
		double p, H1, H2, H3;		/* entropy of prediction errors	*/ 
		long i;

		fac = 1. / (numChan * w * h);
		H1 = H2 = H3 = 0.0;
		for ( i = 0; i < 256; i++)
		{
			if (H_DA1[i])
			{
				p = (double)H_DA1[i] * fac;
				H1 -= (p * log( p) / LOG2);
			}
			if (H_DD1[i])
			{
				p = (double)H_DD1[i] * fac;
				H2 -= (p * log( p) / LOG2);
			}
			if (H_AD1[i])
			{
				p = (double)H_AD1[i] * fac;
				H3 -= (p * log( p) / LOG2);
			}
		}
		features->entropy_DA1 = (unsigned int)min( 200, 10 * H1);
		features->entropy_DD1 = (unsigned int)min( 200, 10 * H2);
		features->entropy_AD1 = (unsigned int)min( 200, 10 * H3);
	}

	/* second transform	*/
	sum_AD = sum_DA = sum_DD = 0;
	for (comp = 0; comp < numChan; comp++)
	{
		/* second horizontal transform	*/
		/* for all rows excluding last	*/
		for (y = 0, py = 0; y < (signed)h-1; y++, py+=pd->width)
		{
			/* for every second column excluding last	*/
			for (x = 0, pos = py; x < (signed)w - 1; x+=2, pos+=2)
			{
				v1 = pd2->data[comp][pos];
				v2 = pd2->data[comp][pos+1];
				pd2->data[comp][pos] = (v1 + v2) >> 1;
				pd2->data[comp][pos+1] = (v1 - v2);
			}
		}

		/* second vertical transform	*/
		sum_AD = sum_DA = sum_DD = 0;
		/* for every second row excluding last	*/
		for (y = 0, py = pyA = 0; y < (signed)h - 1; 
					y+=2, pyA+=pd->width, py+=2*pd->width)
		{
			/* for every second column excluding last	*/
			for (x = 0, pos = py, posA = pyA; x < (signed)w - 1; x++, pos++, posA++)
			{
				v1 = pd2->data[comp][pos];
				v2 = pd2->data[comp][pos+pd->width];
				if (x%2)
				{
					sum_DA += abs( (v1 + v2) >> 1); /* DA signal	*/
					sum_DD += abs( v1 - v2); /* DD signal	*/
				}
				else
				{
					/* AA signal, keep for next decomposition	*/
					pd2->data[comp][posA] = (v1 + v2) >> 1; 
					sum_AD += abs( v1 - v2); /* AD signal	*/
				}
			}
		} /* for y */
	} /* for comp */

	w = w >> 1;
	h = h >> 1;

	if (w && h) /* avoid division by zero	*/
	{
		/* arithmetic mean normalised to a max. value of 200	*/
		fac = 200. / (numChan * w * h * (1 << pd->bitperchannel) );
	}
	else fac = 0;
	features->sum_AD2 = (int)floor( sum_AD * fac); 
	features->sum_DD2 = (int)floor( sum_DD * fac); 
	features->sum_DA2 = (int)floor( sum_DA * fac); 


	/* third transform	*/
	sum_AD = sum_DA = sum_DD = 0;
	for (comp = 0; comp < numChan; comp++)
	{
		/* third horizontal transform	*/
		/* for all rows excluding last	*/
		for (y = 0, py = pyA = 0; y < (signed)h-1; y++, py+=pd->width)
		{
			/* for every second column excluding last	*/
			for (x = 0, pos = py; x < (signed)w - 1; x+=2, pos+=2)
			{
				v1 = pd2->data[comp][pos];
				v2 = pd2->data[comp][pos+1];
				pd2->data[comp][pos] = (v1 + v2) >> 1;
				pd2->data[comp][pos+1] = (v1 - v2);
			}
		}

		/* third vertical transform	*/
		sum_AD = sum_DA = sum_DD = 0;
		/* for every second row excluding last	*/
		for (y = 0, py = 0; y < (signed)h - 1; 
					y+=2, pyA+=pd->width, py+=2*pd->width)
		{
			/* for every second column excluding last	*/
			for (x = 0, pos = py, posA = pyA; x < (signed)w - 1; x++, pos++, posA++)
			{
				v1 = pd2->data[comp][pos];
				v2 = pd2->data[comp][pos+pd->width];
				if (x%2)
				{
					sum_DA += abs( (v1 + v2) >> 1); /* DA signal	*/
					sum_DD += abs( v1 - v2); /* DD signal	*/
				}
				else
				{
					/* AA signal, keep for next decomposition	*/
					pd2->data[comp][posA] = (v1 + v2) >> 1; 
					sum_AD += abs( v1 - v2); /* AD signal	*/
				}
			}
		} /* for y */
	} /* for comp */

	w = w >> 1;
	h = h >> 1;
	if (w && h) /* avoid division by zero	*/
	{
		/* arithmetic mean normalised to a max. value of 200	*/
		fac = 200. / (numChan * w * h * (1 << pd->bitperchannel) );
	}
	else fac = 0.;
	features->sum_AD3 = (int)floor( sum_AD * fac); 
	features->sum_DD3 = (int)floor( sum_DD * fac); 
	features->sum_DA3 = (int)floor( sum_DA * fac); 
	

	DeletePicData( pd2);
}
#endif

#ifdef OBSOLETE
/*---------------------------------------------------------------
 *	getRepeatness()
 *---------------------------------------------------------------*/
unsigned int getRepeatness( PicData *pd)
{
	bool flag;
	float ftemp;
	unsigned int r, i, idx, ch, bits, shift[3];
	unsigned int numChan;
	unsigned int	*selected_rows;
	unsigned int row_count;	/* number of rows to be checked	*/
	unsigned int repeatness;
	unsigned long *buffer, *buffer2;
	unsigned long bufferSize, len1, len2;
	unsigned long x, y, pos, py;


	/* exclude possible alpha channel	*/
	if (pd->channel == 2) numChan = 1;
	else if (pd->channel == 4) numChan = 3;
	else numChan = pd->channel;

	/* check, how many bits per pixel are needed	*/
	bits = 0;
	for ( ch = 0; ch < numChan; ch++)
	{
		bits += pd->bitperchan[ch];
		shift[ch] = 0; /* no shift required	*/
	}
	/* if number of bits too large, then values must be shifted	*/
	if (bits > 30)
	{
		ch = 0;
		do
		{
			shift[ch++]++;
			bits--;
			ch = ch % 3;
		}while (bits > 30);
	}

	row_count = (int)floor( log( (double)pd->size) * 10); /* take a reasonable portion	*/
	bufferSize = row_count * pd->width;

	/* array for row numbers, avoid double selection of rows	*/
	ALLOC( selected_rows, row_count, unsigned int);
	/* array for all randomly selected rows	*/
	ALLOC( buffer, bufferSize, unsigned long);
	ALLOC( buffer2, bufferSize, unsigned long); /* copy of buffer	*/

	if (row_count < pd->height)
	{
		/* select rows randomly	*/
		for( y = 0; y < row_count; y++)
		{
			r = rand () % pd->height;
			flag = false;

			/* check, whether row has been already selected	*/
			for( i = 0; i < row_count; i++)
			{
				if (selected_rows[i] == r)	
				{
					flag = true;
					break;
				}
			}

			//Wenn nicht vorhanden, hinzufügen; wenn nicht, verhindern das i hochgezählt wird
			if (flag)	y--; /* make another trial	*/
			else	selected_rows[y] = r; /* save this row number	*/
		}
	}
	else
	{	/* take all rows	*/
		row_count = pd->height;
		for( r = 0; r < row_count; r++)
		{
			selected_rows[r] = r;
		}
		bufferSize = row_count * pd->width; /* must be corrected	*/
	}

	/* copy all rows into buffer	*/
	idx = 0;
	/* for all selected rows	*/
	for (i = 0; i < row_count; i++)
	{
		py = selected_rows[i] * pd->width;
		/* for all columns	*/
		for (x = 0, pos = py; x < pd->width; x++, pos++)
		{
			buffer[idx] = 0;
			for ( ch = 0; ch < numChan; ch++)
			{
				/* combine all RGB values in a single value	*/
				buffer[idx] +=  (pd->data[ch][pos] >> shift[ch]) << (ch * 10);
			} /* for ch	*/
			idx++;
		} /* x */
		buffer[idx-1]++; /* modify last value preventing slow qsort in blocksorting	*/
	} /* y */

	/* make a copy of the buffer	*/
	memcpy( buffer2, buffer, sizeof(unsigned int) * bufferSize);

  /* call run-length encoding routine, checking compressibility	*/
	len1 = RLCEncode( buffer, bufferSize, 0, pd->width);

/* test */

	/* call block sorting	*/
	i = BWTEncode_long( buffer2, bufferSize);
  /* call again run-length encoding routine, checking compressibility	*/
	len2 = RLCEncode( buffer2, bufferSize, 0, pd->width);

	ftemp = 200.F * ((float)len1 / len2 - 1);
	repeatness = min( 200, max( 0, (int)floor( ftemp)) );/* ==0 for len1<= len2	*/

	FREE( buffer); 
	FREE( buffer2); 
	FREE( selected_rows); 
	return	repeatness;
}
#endif

#ifdef OBSOLETE /* now in getColourContSatVar()	*/

/*---------------------------------------------------------------
 *	getColourContrast()
 *---------------------------------------------------------------*/
unsigned int getColourContrast( PicData *pd)
{
	int diff;
	unsigned int xstep = 1, ystep=1;
	unsigned long x, y, pos, py, sum, cnt;

	


	xstep = max( 1, pd->width / 100);
	ystep = max( 1, pd->height / 100);


	sum = 0;
	cnt = 0;
	if (pd->channel < 3)
	{
		/* for all rows excluding first	*/
		for (y = 1, py = pd->width; y < pd->height; y+=ystep, py+=pd->width*ystep)
		{
			/* for all columns excluding first	*/
			for (x = 1, pos = py+1; x < pd->width; x+=xstep, pos+=xstep)
			{
				diff = pd->data[0][pos] - pd->data[0][pos-1];
				sum += diff * diff; 

				diff = pd->data[0][pos] - pd->data[0][pos - pd->width]; 
				sum += diff * diff; 
				cnt++;
			} /* x */
		} /* y */
		if (cnt)  /* avoid division by zero	*/
			return 	_GETMIN( 200, (unsigned int)floor(  9*100.0 * sum / (255*255*cnt) + 0.5));
		else
			return 0;
	}
	else
	{
		/* for all rows excluding first	*/
		for (y = 1, py = pd->width; y < pd->height; y+=ystep, py+=pd->width*ystep)
		{
			/* for all columns excluding first	*/
			for (x = 1, pos = py+1; x < pd->width; x+=xstep, pos+=xstep)
			{
				diff = pd->data[0][pos] - pd->data[0][pos-1];
				sum += diff * diff; 
				diff = pd->data[1][pos] - pd->data[1][pos-1]; 
				sum += diff * diff; 
				diff = pd->data[2][pos] - pd->data[2][pos-1]; 
				sum += diff * diff; 

				diff = pd->data[0][pos] - pd->data[0][pos - pd->width]; 
				sum += diff * diff; 
				diff = pd->data[1][pos] - pd->data[1][pos - pd->width]; 
				sum += diff * diff; 
				diff = pd->data[2][pos] - pd->data[2][pos - pd->width]; 
				sum += diff * diff; 

				cnt++;
			} /* x */
		} /* y */
		if (cnt)  /* avoid division by zero	*/
			return 	_GETMIN( 200, (unsigned int)floor(  100.0 * sum / (255.*255*cnt) + 0.5));
		else
			return 0;
	}
	// differences of +-255 are very unlikely
	//return 	(unsigned int)floor(  200.0 * sum / (6 * 255 * cnt) + 0.5);
	/* use safe guard for limited range	*/
}

/*---------------------------------------------------------------
 *	getColourSaturation()
 *---------------------------------------------------------------*/
unsigned int getColourSaturation( PicData *pd)
{
	unsigned int xstep = 1, ystep=1;
	unsigned long x, y, pos, py, sum, cnt;

	xstep = max( 1, pd->width / 100);
	ystep = max( 1, pd->height / 100);

	sum = 0;
	cnt = 0;
	if (pd->channel < 3) 
		return 0;
	/* for all rows	*/
	for (y = 0, py = 0; y < pd->height; y+=ystep, py+=pd->width*ystep)
	{
		/* for all columns	*/
		for (x = 0, pos = py; x < pd->width; x+=xstep, pos+=xstep)
		{
			sum += labs( pd->data[0][pos] - pd->data[1][pos]); 
			sum += labs( pd->data[0][pos] - pd->data[2][pos]); 
			sum += labs( pd->data[2][pos] - pd->data[1][pos]); 

			cnt++;
		} /* x */
	} /* y */
	
	if (cnt)  /* avoid division by zero	*/
		return 	_GETMIN( 200, (unsigned int)floor(  200.0 * sum / (512 * cnt) + 0.5));
	else
		return 0;
}
#endif

/*---------------------------------------------------
 * Ermittelt die Entropie eines Arrays aus uints
 *---------------------------------------------------*/
double GetEntropy( uint *arr, uint len)
{
	uint *helparr = NULL;
	uint maxsize, i;
	double entropy, logN;
	
	/* Maximalen Wert holen */
	for (i = 0, maxsize = 0; i < len; i++)
	{
		if (arr[i] > maxsize) maxsize = arr[i];
	}
	
	/* Array von Countern für jedes Symbol anlegen */
	ALLOC( helparr, maxsize + 1u, uint);
	memset( helparr, 0, (maxsize + 1u) * sizeof(uint));
	
	/* Symbolhäufigkeiten erfassen */
	for (i = 0; i < len; i++)
	{
		//if(arr[i] < 0 || arr[i] > maxsize +1u)
		//{
		//	i = i;
		//}
		assert( arr[i] >= 0);
		helparr[arr[i]]++;
	}
	
	/* Entropie berechnen */
#ifdef OLD_ENT
	for (i = 0, entropy = 0; i <= maxsize; i++)
	{
		/* Wenn Häufigkeit 0, Symbol überspringen */ 
		if (helparr[i] == 0) continue;
		/* preg ist die reguläre Auftrittswahrscheinlichkeit,
		   und pinv = 1 / preg */
		preg = (double)helparr[i] / (double)len;
		pinv = (double)len / (double)helparr[i];
		/* Informationsgehalt des Symbols aufaddieren */
		entropy += preg * (log(pinv) / log2);
	}
#else
	logN = log( (double)len);
	for (i = 0, entropy = 0; i <= maxsize; i++)
	{
		/* Wenn Häufigkeit 0, Symbol überspringen */ 
		if (helparr[i] == 0) continue;
		/* Informationsgehalt des Symbols aufaddieren */
		entropy += helparr[i] * (logN - log( (double)helparr[i]) );
	}
	entropy = entropy / (len * LOG2);
#endif
	/* Häufigkeitsarray freigeben und Entropie zurückgeben */
	FREE( helparr);
	return entropy;
}

/*---------------------------------------------------
 * Datenmenge schätzen, die zur 
 * Speicherung des Arrays nötig ist
 *---------------------------------------------------------*/
ulong GuessSizeBytes(uint *arr, uint len)
{
	/* Mittlere Codewortlänge * Zeichenzahl / 8 */
	return (ulong)(GetEntropy(arr, len) * (double)(len)) / 8u;
}

/*---------------------------------------------------
 * Mittlere Entropie eines Bildes ermitteln 
 *---------------------------------------------------------*/
double PicEntropy( PicData *pd)
{
	uint i;
	double entropy = 0.;
	
	/* Entropien der einzelnen Kanäle aufsummieren*/
	for (i = 0; i < pd->channel; i++)
	{
		entropy += GetEntropy( pd->data[i], pd->size);
	}
	/* ...und Mittelwert bilden */
	entropy /= pd->channel;
	// Strutz: it would be better to average the entropy
	// according to the interleaving mode
	// i.e. if YY|uvuv is used, for example, then the data from U and V schould be
	// out into a single array for computing the entropie
	return entropy;
}

/*---------------------------------------------------------
 * HowSynthetic(),
 * result: 0-200,
 * the higher, the more synthetic
 *---------------------------------------------------------*/
uint HowSynthetic( PicData *pd, unsigned int syn[4])
{
	uint i, count, cnt[4], hist_size, *histo = NULL;
	unsigned long pixel_cnt[4], pixel_count;
	unsigned int comp, modal, modal_cnt, numChan;

	/* number of different values in component */
	hist_size = (1u << pd->bitperchannel);
			
	if (pd->channel >= 3) numChan = 3;	/* ignore alpha channel	*/
	else numChan = 1;

	/* create histogram  */
	ALLOC( histo, hist_size, uint);
	pixel_count = 0;
	count = 0;
	for (comp = 0; comp < numChan; comp++)
	{
		cnt[comp] = 0;
		pixel_cnt[comp] = pd->size;
		memset( histo, 0, hist_size * sizeof(uint));
		for (i = 0; i < pd->size; i++)
		{
			histo[ pd->data[comp][i] ]++;
		}
		/* Differenzen benachbarter Häufigkeiten bilden 
		und aufsummieren */
		modal = 0; modal_cnt = histo[ modal];
		for (i = 1u; i < hist_size; i++)
		{
			cnt[comp] += abs( (long)histo[i - 1u] - (long)histo[i]);	
			if (histo[i] > modal_cnt)
			{
				modal_cnt = histo[i];
				modal = i;
			}
		}
		cnt[comp] += abs( (long)histo[0] - (long)histo[hist_size-1]);
		
		/* remove influence of possible background colour */
		if (modal == 0)
		{
			int diff1, diff2;
			diff1 = abs( (long)histo[0] - (long)histo[1]);
			diff2 = abs( (long)histo[0] - (long)histo[hist_size-1]);
			cnt[comp] -= diff1;
			cnt[comp] -= diff2;
			cnt[comp] += abs( (long)histo[1] - (long)histo[hist_size-1]);
		}
		else if (modal == hist_size-1)
		{
			int diff1, diff2;
			diff1 = abs( (long)histo[modal] - (long)histo[0]);
			diff2 = abs( (long)histo[modal] - (long)histo[modal-1]);
			cnt[comp] -= diff1;
			cnt[comp] -= diff2;
			cnt[comp] += abs( (long)histo[0] - (long)histo[modal-1]);
		}
		else
		{
			int diff1, diff2;
			diff1 = abs( (long)histo[modal] - (long)histo[modal+1]);
			diff2 = abs( (long)histo[modal] - (long)histo[modal-1]);
			cnt[comp] -= diff1;
			cnt[comp] -= diff2;
			cnt[comp] += abs( (long)histo[modal+1] - (long)histo[modal-1]);
		}
		pixel_cnt[comp] -= (long)histo[modal]; /* ignore background pixels	*/

		if (cnt[comp] < 10)	/* threshold not tested	*/
			syn[comp] = 200;
		else syn[comp] = (unsigned int)(100.0 * (double)cnt[comp] / (double)pixel_cnt[comp]);
		count += cnt[comp];
		pixel_count += pixel_cnt[comp];
	} /* comp */
	
	/* Aufräumen und return */
	FREE( histo);


	/* very small cnt if very few different signal values	*/
	if (count < 10u)	/* threshold not tested	*/
		return (200);
	if (pixel_count) /* avoid division by zero	*/
		return (unsigned int)_GETMIN( 200, 100.0 * (double)count / (double)pixel_count);
	else
		return 0;
}

#ifdef OBSOLETE /* now in getRoCoMoSyNoiCon()	*/
/*---------------------------------------------------------
 * GetRoughness( )
 * determine approximate roughness along diagonals
 *---------------------------------------------------------*/
unsigned int GetRoughness( PicData *pd)
{
	unsigned int i, wp1, wm1;
	unsigned int roughness;
	unsigned int comp, numChan;
	unsigned int N;
	unsigned long p1, p2;

	N = _GETMIN( pd->width, pd->height);
	wp1 = pd->width + 1;
	wm1 = pd->width - 1;

	if (pd->channel >= 3) numChan = 3;	/* ignore alpha channel	*/
	else numChan = 1;

	roughness = 0;
	for (comp = 0; comp < numChan; comp++)
	{
		/* from top left towards bottom right	*/
		for (i = 0, p1 = 0; i < N-1; i++, p1+=wp1)
		{
			p2 = p1 + wp1;
			roughness += abs( (long)pd->data[comp][p1] - (long)pd->data[comp][p2]);
		}
		/* from top right towards bottom left	*/
		for (i = 0, p1=pd->width - 1; i < N-1; i++, p1+=wm1)
		{
			p2 = p1 + wm1;
			roughness += abs( (long)pd->data[comp][p1] - (long)pd->data[comp][p2]);
		}
	} /* comp */
	/* 0 <= roughness <= 255*2	*/
	if (N)
		return (unsigned int)floor( ( 1.0 * roughness / (numChan * (N-1))) + 0.5);
	else
		return 0;
}

/*---------------------------------------------------------
 * GetCorrelation( )
 * determine approximate correlation along diagonals
 *---------------------------------------------------------*/
unsigned int GetCorrelation( PicData *pd)
{
	double diff, mean, Rxx0, Rxx1, corrcoeff;
	unsigned int i, wp1, wm1;
	unsigned int comp, numChan;
	unsigned int N;
	unsigned long p1;


	N = _GETMIN( pd->width, pd->height);
	wp1 = pd->width + 1;
	wm1 = pd->width - 1;

	if (pd->channel >= 3) numChan = 3;	/* ignore alpha channel	*/
	else numChan = 1;

	corrcoeff = 0;
	for (comp = 0; comp < numChan; comp++)
	{
		/* 
		 * get mean of signal	
		 */
		mean = 0;
		Rxx0 = Rxx1 = 0.;
		/* from top left towards bottom right	*/
		/* from top right towards bottom left	*/
		for (i = 0, p1 = 0; i < N-1; i++, p1 += wp1)
		{
			mean += pd->data[comp][p1];
			mean += pd->data[comp][p1+wm1];
		}

		if (N) mean /= 2*(N-1);
		/* get variance = Rxx[0] and Rxx[1]	*/
		/* from top left towards bottom right	*/
		for (i = 0, p1 = 0; i < N-1; i++, p1 += wp1)
		{
			diff = pd->data[comp][p1] - mean;
			Rxx0 += diff * diff; /* == variance	*/
			Rxx1 += diff * (pd->data[comp][p1 + wm1] - mean);
		}

		/* from top right towards bottom left	*/
		for (i = 0, p1 = wm1; i < N-1; i++, p1 += wm1)
		{
			diff = pd->data[comp][p1] - mean;
			Rxx0 += diff * diff;
			Rxx1 += diff * (pd->data[comp][p1+wm1] - mean);
		}
		if (Rxx0 != 0.0 ) corrcoeff += Rxx1 / Rxx0;
	} /* comp */

	/* 0 <= return <= 100	*/

	/* avoid negativ values	*/
	return (unsigned int) max(0, floor( 100. * corrcoeff / numChan + 0.5));
}

/*---------------------------------------------------------------
 *	getMonotony()
 *---------------------------------------------------------------*/
unsigned int getMonotony( PicData *pd)
{
	unsigned int xstep = 1, ystep=1, up_flag;
	unsigned int change_count, comp, numChan;
	unsigned long x, y, pos, py, cnt;

	xstep = max( 1, pd->width / 100);
	ystep = max( 1, pd->height / 100);

	change_count = 0;
	up_flag = 0;

	if (pd->channel >= 3) numChan = 3;	/* ignore alpha channel	*/
	else numChan = 1;

	cnt = 0;
	for (comp = 0; comp < numChan; comp++)
	{
		/* for 100 rows	*/
		for (y = 0, py = 0; y < pd->height; y+=ystep, py+=pd->width*ystep)
		{
			if (pd->data[comp][py+1] > pd->data[comp][py]) up_flag = 1;
			/* for 100 columns	*/
			for (x = 1, pos = py+1; x < pd->width; x+=xstep, pos+=xstep)
			{
				if (pd->data[comp][pos+1] > pd->data[comp][pos])
				{
					if (up_flag == 0)
					{
						change_count++;
						up_flag = 1;
					}
				}
				else	if (pd->data[comp][pos+1] < pd->data[comp][pos])
				{
					if (up_flag == 1)
					{
						change_count++;
						up_flag = 0;
					}
				}
				cnt++;
			} /* x */
		} /* y */
	}
	if (cnt)
		return 	100-(unsigned int)floor( ( 100.0 * change_count / cnt) + 0.5);
	else 
		return 0;
}
#endif

/*---------------------------------------------------------------
*	heap_sort()
* sorting of values in ra[0...N-1] in ascending order
* ra[] is replaced on output by sorted values
*--------------------------------------------------------------*/
void
heap_sort(unsigned long N, unsigned long ra[])
{
  unsigned long i, ir, j, l;
  unsigned long rra;
	
  if (N < 2)
		return;
  l = (N >> 1) + 1;
  ir = N - 1;
	
  for (;;)
  {
		if (l > 1)
		{
			l--;
			rra = ra[l - 1];
		}
		else
		{
			rra = ra[ir];
			ra[ir] = ra[0];
			ir--;
			if (ir == 0)
			{
				ra[0] = rra;
				break;
			}
		}
		i = l - 1;
		
		j = l + l;
		while (j <= ir + 1)
		{
			if (j < ir + 1 && ra[j - 1] < ra[j + 1 - 1])
			{
				j++;
			}
			if (rra < ra[j - 1])
			{
				ra[i] = ra[j - 1];
				i = j - 1;
				j <<= 1;
			}
			else
			{
				j = ir + 1 + 1;	/* terminate while-loop */
			}
		}
		ra[i] = rra;
  }
}

/*---------------------------------------------------------------
*	heap_sort_ll()
* sorting of values in ra[0...N-1] in ascending order
* ra[] is replaced on output by sorted values
*--------------------------------------------------------------*/
void
heap_sort_ll(unsigned long N, unsigned long long ra[])
{
  unsigned long i, ir, j, l;
  unsigned long long rra;
	
  if (N < 2)
		return;
  l = (N >> 1) + 1;
  ir = N - 1;
	
  for (;;)
  {
		if (l > 1)
		{
			l--;
			rra = ra[l - 1];
		}
		else
		{
			rra = ra[ir];
			ra[ir] = ra[0];
			ir--;
			if (ir == 0)
			{
				ra[0] = rra;
				break;
			}
		}
		i = l - 1;
		
		j = l + l;
		while (j <= ir + 1)
		{
			if (j < ir + 1 && ra[j - 1] < ra[j + 1 - 1])
			{
				j++;
			}
			if (rra < ra[j - 1])
			{
				ra[i] = ra[j - 1];
				i = j - 1;
				j <<= 1;
			}
			else
			{
				j = ir + 1 + 1;	/* terminate while-loop */
			}
		}
		ra[i] = rra;
  }
}

/*---------------------------------------------------------------
 *	heap_sort_uc()
 * sorting of values in ra[0...N-1] and its indices in pos_ptr[] 
 * in ascending order
 * ra[] is replaced on output by sorted values
 *--------------------------------------------------------------*/
void
heap_sort_uc(unsigned long N, unsigned char ra[],
	unsigned long pos_ptr[])
{
	unsigned long i, ir, j, l, pos;
	unsigned char rra;

	/* initialize indices (positions) */
	for (i = 0; i < N; i++)
	{
		pos_ptr[i] = i;
	}

	if (N < 2)
		return;
	l = (N >> 1) + 1;
	ir = N - 1;

	for (;;)
	{
		if (l > 1)
		{
			l--;
			rra = ra[l - 1];
			pos = pos_ptr[l - 1];
		}
		else
		{
			rra = ra[ir];
			pos = pos_ptr[ir];
			ra[ir] = ra[0];
			pos_ptr[ir] = pos_ptr[0];
			ir--;
			if (ir == 0)
			{
				ra[0] = rra;
				pos_ptr[0] = pos;
				break;
			}
		}
		i = l - 1;

		j = l + l;
		while (j <= ir + 1)
		{
			if (j < ir + 1 && ra[j - 1] < ra[j + 1 - 1])
			{
				j++;
			}
			if (rra < ra[j - 1])
			{
				ra[i] = ra[j - 1];
				pos_ptr[i] = pos_ptr[j - 1];
				i = j - 1;
				j <<= 1;
			}
			else
			{
				j = ir + 1 + 1;	/* terminate while-loop */
			}
		}
		ra[i] = rra;
		pos_ptr[i] = pos;
	}
}

/*---------------------------------------------------------------
 *	heap_sort_ul()
 * sorting of values in ra[0...N-1] and its indices in pos_ptr[] 
 * in ascending order
 * ra[] is replaced on output by sorted values
 *--------------------------------------------------------------*/
void
heap_sort_ul(unsigned long N, unsigned long ra[],
	unsigned long pos_ptr[])
{
	unsigned long i, ir, j, l, pos;
	unsigned long rra;

	/* initialize indices (positions) */
	for (i = 0; i < N; i++)
	{
		pos_ptr[i] = i;
	}

	if (N < 2)
		return;
	l = (N >> 1) + 1;
	ir = N - 1;

	for (;;)
	{
		if (l > 1)
		{
			l--;
			rra = ra[l - 1];
			pos = pos_ptr[l - 1];
		}
		else
		{
			rra = ra[ir];
			pos = pos_ptr[ir];
			ra[ir] = ra[0];
			pos_ptr[ir] = pos_ptr[0];
			ir--;
			if (ir == 0)
			{
				ra[0] = rra;
				pos_ptr[0] = pos;
				break;
			}
		}
		i = l - 1;

		j = l + l;
		while (j <= ir + 1)
		{
			if (j < ir + 1 && ra[j - 1] < ra[j + 1 - 1])
			{
				j++;
			}
			if (rra < ra[j - 1])
			{
				ra[i] = ra[j - 1];
				pos_ptr[i] = pos_ptr[j - 1];
				i = j - 1;
				j <<= 1;
			}
			else
			{
				j = ir + 1 + 1;	/* terminate while-loop */
			}
		}
		ra[i] = rra;
		pos_ptr[i] = pos;
	}
}

/*---------------------------------------------------------------
 *	heap_sort_ull()
 * sorting of values in ra[0...N-1] and its indices in pos_ptr[] 
 * in ascending order
 * ra[] is replaced on output by sorted values
 *--------------------------------------------------------------*/
void
heap_sort_ull(unsigned long N, unsigned long long ra[],
	unsigned long pos_ptr[])
{
	unsigned long i, ir, j, l, pos;
	unsigned long long rra;

	/* initialize indices (positions) */
	for (i = 0; i < N; i++)
	{
		pos_ptr[i] = i;
	}

	if (N < 2)
		return;
	l = (N >> 1) + 1;
	ir = N - 1;

	for (;;)
	{
		if (l > 1)
		{
			l--;
			rra = ra[l - 1];
			pos = pos_ptr[l - 1];
		}
		else
		{
			rra = ra[ir];
			pos = pos_ptr[ir];
			ra[ir] = ra[0];
			pos_ptr[ir] = pos_ptr[0];
			ir--;
			if (ir == 0)
			{
				ra[0] = rra;
				pos_ptr[0] = pos;
				break;
			}
		}
		i = l - 1;

		j = l + l;
		while (j <= ir + 1)
		{
			if (j < ir + 1 && ra[j - 1] < ra[j + 1 - 1])
			{
				j++;
			}
			if (rra < ra[j - 1])
			{
				ra[i] = ra[j - 1];
				pos_ptr[i] = pos_ptr[j - 1];
				i = j - 1;
				j <<= 1;
			}
			else
			{
				j = ir + 1 + 1;	/* terminate while-loop */
			}
		}
		ra[i] = rra;
		pos_ptr[i] = pos;
	}
}


/*---------------------------------------------------------
 * GetNumColours()
 * counting of unique colours in the image
 *---------------------------------------------------------*/
unsigned long GetNumColours( PicData *pd, unsigned long *maxCount,
														double *colEntropy)
{
	uint i;
	unsigned long numColours, colour;
	unsigned long start_count, colCount;
	unsigned long *col_ptr = NULL;
	unsigned long *count_ptr = NULL;
	double entropy;
		
	/* array for colours, maximal number is equal to image size */
	ALLOC( col_ptr, pd->size, unsigned long);
	/* array for frequency counts	*/
	ALLOC( count_ptr, pd->size, unsigned long);

	if (pd->channel >= 3)
	{
		for (i = 0; i < pd->size; i++)
		{
			/* combine the three components in a single value	*/
			col_ptr[i] = ((unsigned long)(pd->data[2][i])<<16)+
				           ((unsigned long)(pd->data[1][i])<<8) + pd->data[0][i];
		}
	}
	else
	{
		for (i = 0; i < pd->size; i++)
		{
			col_ptr[i] = pd->data[0][i];
		}
	}

	/* sort colours */
	heap_sort( pd->size, col_ptr);

	/* count colours */
	colour = col_ptr[0];
	numColours = 0;
	start_count = 0;
	*maxCount = 0;
	for (i = 1u; i < pd->size; i++)
	{
		if (colour != col_ptr[i])
		{
			colour = col_ptr[i];
			/* determine highest frequency of a colour	*/
			colCount = i - start_count;
			if (colCount > *maxCount)
			{
				*maxCount = colCount;
			}
			start_count = i;
			count_ptr[numColours] = colCount; /* numColours =2 for first colour*/
			numColours++;
		}		
	}
	/* determine highest frequency of a colour	*/
	colCount = pd->size - start_count; /* last section	*/
	if (colCount > *maxCount)
	{
		*maxCount = colCount;
	}
	count_ptr[numColours] = colCount; /* numColours =2 for first colour*/
	numColours++;

	/* compute entropy  of colur distribution */
	entropy = 0.;
	for (i = 0u; i < numColours; i++)
	{
		double p;
		p = (double)count_ptr[i] / pd->size;
		entropy -= p * log(p) * invLOG2;
	}
  *colEntropy = entropy;

	FREE( col_ptr);
	FREE( count_ptr);
	return numColours;
}

/*---------------------------------------------------------
 * GetNumColoursBlock()
 * only colour entropy is of interest
 *---------------------------------------------------------*/
unsigned long GetNumColoursBlock( PicData *pd, 
														 unsigned int y_start, unsigned int y_end,
														 unsigned int x_start, unsigned int x_end, 
														 //unsigned long *maxCount,
														double *colEntropy)
{
	uint i;
	unsigned long numColours, colour, size;
	unsigned long start_count, colCount;
	unsigned long *col_ptr = NULL;
	unsigned long *count_ptr = NULL;
	unsigned long x, y, pos, py, pc;
	double entropy;

	assert( y_end >= y_start);
	assert( x_end >= x_start);
	assert( y_end < pd->height);
	assert( x_end < pd->width);


	size = (y_end - y_start + 1) * (x_end - x_start + 1);

	/* array for colours, maximal number is equal to image size */
	ALLOC( col_ptr, size, unsigned long);
	/* array for frequency counts	*/
	CALLOC_( count_ptr, size, unsigned long);

	if (pd->channel >= 3)
	{
		pc = 0;
		for (y = y_start, py = y_start * pd->width; y <= y_end; 
			y++, py += pd->width)
		{
			for (x = x_start, pos = py + x_start; x <= x_end; x++, pos++)
			{
				/* combine the three components in a single value	*/
				col_ptr[pc++] = ((unsigned long)(pd->data[2][pos])<<16)+
												((unsigned long)(pd->data[1][pos])<<8) + pd->data[0][pos];
			}
		}
	}
	else
	{
		pc = 0;
		for (y = y_start, py = y_start * pd->width; y <= y_end; 
			y++, py += pd->width)
		{
			for (x = x_start, pos = py + x_start; x <= x_end; x++, pos++)
			{
				col_ptr[pc++] = pd->data[0][pos];
			}
		}
	}

	/* sort colours */
	heap_sort( size, col_ptr);

	/* count colours */
	colour = col_ptr[0];
	numColours = 0;
	start_count = 0;
	//*maxCount = 0;
	for (i = 1u; i < size; i++)
	{
		if (colour != col_ptr[i])
		{
			colour = col_ptr[i];
			/* determine highest frequency of a colour	*/
			colCount = i - start_count;
			//if (colCount > *maxCount)
			//{
			//	*maxCount = colCount;
			//}
			start_count = i;
			count_ptr[numColours] = colCount; /* numColours =2 for first colour*/
			numColours++;
		}		
	}
	/* determine highest frequency of a colour	*/
	colCount = size - start_count; /* last section	*/
	//if (colCount > *maxCount)
	//{
	//	*maxCount = colCount;
	//}
	count_ptr[numColours] = colCount; /* numColours =2 for first colour*/
	numColours++;

	/* compute entropy  of colour distribution */
	entropy = 0.;
	for (i = 0u; i < numColours; i++)
	{
		double p;
		p = (double)count_ptr[i] / size;
		entropy -= p * log(p) * invLOG2;
	}

	*colEntropy = entropy;

	FREE( col_ptr);
	FREE( count_ptr);
	return numColours;
}

														
/*---------------------------------------------------------
 * GetPatEntropy()
 *---------------------------------------------------------*/
void GetPatEntropy( PicData *pd, 
										 unsigned int y_start, unsigned int y_end,
										 unsigned int x_start, unsigned int x_end,
										 double entropy[3])
{
	uint i, comp;
	unsigned long patt, size;
	unsigned long start_count, patCount, numPatterns;
	unsigned long *pat_ptr = NULL;
	unsigned long *count_ptr = NULL;
	unsigned long x, y, pos, py, pc;

	assert( y_end >= y_start);
	assert( x_end >= x_start);
	assert( y_end < pd->height);
	assert( x_end < pd->width);

	entropy[0] = entropy[1] = entropy[2] = 0.;

	size = (y_end - y_start + 1) * (x_end - x_start + 1);

	/* array for colours, maximal number is equal to image size */
	ALLOC( pat_ptr, size, unsigned long);
	/* array for frequency counts	*/
	CALLOC_( count_ptr, size, unsigned long);

	for (comp = 0; comp < pd->channel && comp < 3; comp++)
	{
		pc = 0;
		for (y = y_start+1, py = (y_start+1) * pd->width; y <= y_end; 
			y++, py += pd->width)
		{
			for (x = x_start+1, pos = py + x_start+1; x <= x_end; x++, pos++)
			{
				/* combine the three components in a single value	*/
				pat_ptr[pc++] = ((unsigned long)(pd->data[comp][pos])<<16)+
												((unsigned long)(pd->data[comp][pos-1])<<8) + 
												pd->data[comp][pos - pd->width];
			}
		}


		/* sort colours */
		heap_sort( size, pat_ptr);

		/* count patterns */
		patt = pat_ptr[0];
		numPatterns = 0;
		start_count = 0;
		//*maxCount = 0;
		for (i = 1u; i < size; i++)
		{
			if (patt != pat_ptr[i])
			{
				patt = pat_ptr[i];
				/* determine highest frequency of a colour	*/
				patCount = i - start_count;
				start_count = i;
				count_ptr[numPatterns] = patCount; /* numColours =2 for first colour*/
				numPatterns++;
			}		
		}
		/* determine highest frequency of a colour	*/
		patCount = size - start_count; /* last section	*/
		count_ptr[numPatterns] = patCount; /* */
		numPatterns++;

		/* compute entropy  of colour distribution */
		entropy[comp] = 0.;
		for (i = 0u; i < numPatterns; i++)
		{
			double p;
			p = (double)count_ptr[i] / size;
			entropy[comp] -= p * log(p) * invLOG2;
		}
	}


	FREE( pat_ptr);
	FREE( count_ptr);
}

/*---------------------------------------------------------
 * PCA_3D()
 *---------------------------------------------------------*/
int PCA_3D( PicData *pd)
{
	unsigned long i;
	double m, p, q,  phi;
	double eig1, eig2, eig3;
	double sum;
	double d[3], A[3][3], V[3][3];	/* eigen values and vectore*/
	double CT[3][3]; /* matrix for optimal colour transform	*/
	double eigval[3];
	double diffR, diffG, diffB;
	double meanR, meanG, meanB;
	double var_RR, var_RG, var_RB;
	double var_GR, var_GG, var_GB;
	double var_BR, var_BG, var_BB;
	double var_RR_reduced, var_GG_reduced, var_BB_reduced;
	unsigned long	sum_R, sum_G, sum_B;

	/* compute mean values	*/
	sum_R = sum_G = sum_B = 0;
	for ( i = 0; i < pd->size; i++) /* all pixels	*/
	{
		sum_R += pd->data[0][i]; 
		sum_G += pd->data[1][i]; 
		sum_B += pd->data[2][i]; 
	}
	meanR = (double)sum_R / pd->size;
	meanG = (double)sum_G / pd->size;
	meanB = (double)sum_B / pd->size;

	/* compute variance	*/
	var_RR = var_RG = var_RB = 0;
	var_GR = var_GG = var_GB = 0;
	var_BR = var_BG = var_BB = 0;
	for ( i = 0; i < pd->size; i++) /* all pixels	*/
	{
		diffR = pd->data[0][i] - meanR; 
		diffG = pd->data[1][i] - meanG; 
		diffB = pd->data[2][i] - meanB; 
		var_RR += diffR * diffR; 
		var_RG += diffR * diffG; 
		var_RB += diffR * diffB; 
		var_GG += diffG * diffG; 
		var_GB += diffB * diffG; 
		var_BB += diffB * diffB; 
	}
	/* co-variances
   *  RR  RG  RB
	 *  RG  GG  GB
	 *  RB  GB  BB
	 */
	var_RR /= pd->size;
	var_RG /= pd->size;
	var_RB /= pd->size;
	var_GG /= pd->size;
	var_GB /= pd->size;
	var_BB /= pd->size;

	if (0)
	{
	/* eigenvalues: 2, 1, -1
	 * vectors: (1 1 -1); 
	 */
	//var_RR = 0;
	//var_RG = 1;
	//var_RB = -1;
	//var_GG = 1;
	//var_GB = 0;
	//var_BB = 1;

	// Given symmetric 3x3 matrix M, compute the eigenvalues
	m = (var_RR + var_GG + var_BB)/3;
	// K = M-m*eye(3);
	var_RR_reduced = var_RR - m;
	var_GG_reduced = var_GG - m;
	var_BB_reduced = var_BB - m;
	// q = det(K)/2;
	q = (var_RR_reduced * var_GG_reduced * var_BB_reduced +
	    var_RG * var_GB * var_RB +
			var_RB * var_RG * var_GB -
			var_RR_reduced * var_GB * var_GB -
			var_RG * var_RG * var_BB_reduced -
			var_RB * var_GG_reduced * var_RB
			) / 2;
	p = M_PI;
	//for i=1:3
	//		for j=1:3
	//				p = p + K(i,j)^2;
	p = (
			var_RR_reduced * var_RR_reduced + var_RG*var_RG + var_RB*var_RB +
			var_GG_reduced * var_GG_reduced + var_RG*var_RG + var_GB*var_GB + 
			var_BB_reduced * var_BB_reduced + var_RB*var_RB + var_GB*var_GB
			) / 6;
	 
	phi = 1. / 3. * acos( q / pow( p,(3./2.)));
 
	/* NOTE: the follow formula assume accurate computation and 
	 * therefor q/p^(3/2) should be in range of [1,-1], 
	 * but in real code, because of numerical errors, it must be checked. 
	 * Thus, in case abs(q) >= abs(p^(3/2)), set phi = 0;
	 */
	if (fabs( q) >= fabs( pow(p, (3./2.)))) 		phi = 0;
	if ( phi < 0 ) phi = phi + M_PI / 3;
 
	eig1 = m + 2 * sqrt( p) * cos( phi);
	eig2 = m - sqrt( p) * (cos( phi) + sqrt(3.) * sin( phi));
	eig3 = m - sqrt( p) * (cos( phi) - sqrt(3.) * sin( phi));
	}


	/* 
	 * determine the eigenvectors
	 *
	 * (A - lambda_i*I)*a_i = 0
	 * in LS: J*a = y
	 * eigenvectors = columns of V
	 */

	A[0][0] = var_RR;
	A[0][1] = A[1][0] = var_RG;
	A[0][2] = A[2][0] = var_RB;
	A[1][1] = var_GG;
	A[1][2] = A[2][1] = var_GB;
	A[2][2] = var_BB;
	eigen_decomposition( A, V, d);

	/* normalisation of eigenvectors	*/
  /* assume d[2] as being the largest eigenvalue */

	sum = fabs( V[0][2]) + fabs( V[1][2]) + fabs( V[2][2]);
	V[0][2] /= sum;
	V[1][2] /= sum;
	V[2][2] /= sum;

	sum = fabs( V[0][1]) + fabs( V[1][1]) + fabs( V[2][1]);
	V[0][1] /= sum;
	V[1][1] /= sum;
	V[2][1] /= sum;

	sum = fabs( V[0][0]) + fabs( V[1][0]) + fabs( V[2][0]);
	V[0][0] /= sum;
	V[1][0] /= sum;
	V[2][0] /= sum;

	/* identify Y component */
	sum = V[0][2] + V[1][2] + V[2][2];
	if (sum > 0.9)
	{
		CT[0][0] = V[0][2];
		CT[0][1] = V[1][2];
		CT[0][2] = V[2][2];
		eigval[0] = d[2];

		CT[1][0] = V[0][1] * 2;
		CT[1][1] = V[1][1] * 2;
		CT[1][2] = V[2][1] * 2;
		eigval[1] = d[1];

		CT[2][0] = V[0][0] * 2;
		CT[2][1] = V[1][0] * 2;
		CT[2][2] = V[2][0] * 2;
		eigval[2] = d[0];
	}
	else 
	{
		sum = V[0][1] + V[1][1] + V[2][1];
		if (sum > 0.9)
		{
			CT[0][0] = V[0][1];
			CT[0][1] = V[1][1];
			CT[0][2] = V[2][1];
			eigval[0] = d[1];

			CT[1][0] = V[0][2] * 2;
			CT[1][1] = V[1][2] * 2;
			CT[1][2] = V[2][2] * 2;
			eigval[1] = d[2];

			CT[2][0] = V[0][0] * 2;
			CT[2][1] = V[1][0] * 2;
			CT[2][2] = V[2][0] * 2;
			eigval[2] = d[0];
		}
		else
		{
			CT[0][0] = V[0][0];
			CT[0][1] = V[1][0];
			CT[0][2] = V[2][0];
			eigval[0] = d[0];

			CT[1][0] = V[0][2] * 2;
			CT[1][1] = V[1][2] * 2;
			CT[1][2] = V[2][2] * 2;
			eigval[1] = d[2];

			CT[2][0] = V[0][1] * 2;
			CT[2][1] = V[1][1] * 2;
			CT[2][2] = V[2][1] * 2;
			eigval[2] = d[1];
		}
	}
	/* exchange U and V, if neccessary */
	if ( fabs( CT[1][0]) < fabs( CT[2][0]) )
	{
		double tmp[3];

		tmp[0] = CT[1][0];
		tmp[1] = CT[1][1];
		tmp[2] = CT[1][2];
		CT[1][0] = CT[2][0];
		CT[1][1] = CT[2][1];
		CT[1][2] = CT[2][2];
		CT[2][0] = tmp[0];
		CT[2][1] = tmp[1];
		CT[2][2] = tmp[2];
	}
	/* invert direction of vector, if necessary	*/
	if ( CT[1][0] < 0.)
	{
		CT[1][0] = -CT[1][0];
		CT[1][1] = -CT[1][1];
		CT[1][2] = -CT[1][2];
	}
	return RGBc;
}


/*---------------------------------------------------------
 * Apply paeth to image and
 * count gray pixels afterwards (R=G=B) 
 *---------------------------------------------------------*/
uint CountGray(PicData *pd)
{
	uint i, k, cnt;
	cbool hit;
	PaethFilter( pd);
	
	for (i = 0, cnt = 0; i < pd->size; i++)
	{
		for (hit = TRUE, k = 1u; k < pd->channel; k++)
		{
			if (pd->data[k][i] != pd->data[0][i]) hit = FALSE;
		}
		if (hit == TRUE) cnt++;
	}

	PaethInverse( pd);
	return cnt;
}



