/*******************************************************************
 *
 * File....: arithmetic.c
 * Function: Arithmetic coding - Coding functions
 * Author..: Dávid Bankó
 * Date....: 25.07.2012.
 *
 * 24.07.2012 Titus: Function colour_quantise is added, also modifications
 * to ADC_adj
 * 30.07.2012 Strutz: commenting, formatting and changing some bits
 * 31.07.2012 Banko: Few corrections based on commenting, AC_Mixed LUT
 * 14.09.2012 Banko: Modifications regarding the error reported in AC_Mixed()
 * 28.10.2015 Strutz: bugfix memory leak for *symbols, M1, M2, results.histogramm
 ********************************************************************/
#include "arithmeticB.h"
// for debuggung #include "extract.h"

static unsigned long range, low, n_bits;

/*------------------------------------------------------------------
 * uint colour_quantise(uint col, uint numColours)
 *
 * Returns a positive integer representing the domain to which the
 * colour belongs.
 *-----------------------------------------------------------------*/
uint colour_quantise( uint col, uint numColours)
{
	if (col <= numColours/8)
	{
		return 0;
	}
	else if (col <= numColours/3)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

/*------------------------------------------------------------------
 * Shift()
 *
 * Marker symbol will be 0, all others are shifted by 1;
 *-----------------------------------------------------------------*/
void
Shift( unsigned int *arr, unsigned int len)
{
	unsigned int i, max = 0;

	for ( i = 0; i < len; i++)
	{
		if (arr[i] > max)
			max = arr[i];
	}
	for ( i = 0; i < len; i++)
	{
		if (arr[i] == max)
			arr[i] = 0;
		else
			arr[i]++;
	}
	return;
}

/*------------------------------------------------------------------
 * UndoShift()
 *
 * Reverse the shifting process. (For run series, the maximal value is
 * recieved as a parameters)
 *-----------------------------------------------------------------*/
void
UndoShift( unsigned int *arr, unsigned int len, unsigned int max)
{
	unsigned int i;

	for ( i = 0; i < len; i++)
	{
		if (arr[i] > max)
			max = arr[i];
	}
	for ( i = 0; i < len; i++)
	{
		if (arr[i] == 0)
			arr[i] = max;
		else
			arr[i]--;
	}
	return;
}

/*------------------------------------------------------------------
 * Shift_Histogram()
 *
 * Adjust the histogram after the shifting process. Basically shift
 * the frequencies accordingly.
 *-----------------------------------------------------------------*/
void
Shift_Histogram( sStats *stats)
{
	unsigned int tmp, i;

	tmp = stats->histogram[stats->max_symbol];
	for ( i = stats->max_symbol; i > 0; i--)
		stats->histogram[i] = stats->histogram[i-1];
	stats->histogram[0] = tmp;
}

/*------------------------------------------------------------------
 * Analyze()
 *
 * Analyzing the symbol series, determining the biggest symbol value,
 * creating the histogram.
 *-----------------------------------------------------------------*/
void
Analysis(  sStats *stats, unsigned int *arr, unsigned int len)
{
	unsigned int i, k;

	stats->hist_start = 0;
	stats->max_symbol = 0;	/* reset maximum symbol	*/
	stats->second = 0;			/* reset second maximum symbol	*/
	stats->used_symbols = 0;
	stats->runs = 0;
	stats->zrun = 0;
	stats->follow = 0;
	stats->avgF = 0;
	stats->histogram = NULL; // Strutz, required for freeing the memory space
	stats->symbols = NULL; // Strutz
	
	/* Biggest symbol */
	for ( i = 0; i < len; i++)
	{
		if (arr[i] > stats->max_symbol)
		{			
			/* copy old mx value to second max value	*/
			stats->second = stats->max_symbol;
			/* set new max value	*/
			stats->max_symbol = arr[i];
		}
		else
		{
			/* set second max value if current lower than max but higher than second	*/
			if ((arr[i] > stats->second) && (arr[i] != stats->max_symbol))
					stats->second = arr[i];
		}
	}
	/* maximum symbol is marker	symbol (after RLC2 and only in symbol stream)*/

	/* Setting the array's values to zero */
	ALLOC( stats->histogram, stats->max_symbol + 1, unsigned int);

	for ( i = 0; i < stats->max_symbol + 1; i++)
	{
		stats->histogram[i] = 0;
	}

	/* Histogram and number of run-markers */
	for ( i = 0; i < len; i++)
	{
		/* if first occurence of a symbol, then increment number  */
		if (stats->histogram[arr[i]] == 0)	stats->used_symbols++;

		/* increment bin of current symbol	*/
		stats->histogram[arr[i]]++;

		if (arr[i] == stats->max_symbol)
		{
			/* current symbol is equal to maximum symbol	*/
			stats->runs++;	/* assume Symbolstream including runs	*/

			if (i < len - 1)
			{
				/* if not at end of sequence then sum following symbol	*/
				stats->avgF += (float)arr[i+1];
				/* remember maximal following symbol	*/
				if (arr[i+1] > stats->follow)	stats->follow = arr[i+1];
			}
		}
	}
	/* compute average of symbols after markers, 
	 * Nonsense if sequence is not result of RLC	
	 */
	stats->avgF /= (float)stats->runs;

	/* Finding the first zero bin, saving the previous symbol */
	// Strutz for ( i = 0; i < stats->max_symbol + 1; i++)
	for ( i = 1; i < stats->max_symbol + 1; i++) 
	/* assumes that zero appears at least one time	*/
	{
		if (stats->histogram[i] == 0)
		{
			stats->hist_start = i-1;
			break;
		}
	}

	/* Zeros run percent */
	k = 0; /* Temp counter */

	for ( i = stats->hist_start+1; i < stats->max_symbol + 1; i++)
	{
		if (stats->histogram[i] == 0)
		{
			k++;	/* count empty bins	*/
		}
		else
		{
			if (k > stats->zrun)
			{
				/* remember longest gap in histogram	*/
				stats->zrun = k;
			}
			k = 0;
		}
	}

	return;
}


/*------------------------------------------------------------------
 * SelectMethod()
 *
 * Selecting the best method, performing an auto-evaluation.
 *-----------------------------------------------------------------*/
int
SelectMethod( sStats *stats, char s)
{
	float Zeros = 0., Zrun = 0.;

	Zeros = (float)(stats->max_symbol + 1 - stats->used_symbols) / (float)(stats->max_symbol + 1);

	if ((stats->max_symbol + 1 - stats->used_symbols) != 0)
	{
		Zrun =  (float)stats->zrun / (float)(stats->max_symbol + 1 - stats->used_symbols);
	}

	/* Automated method selection */
	if (( stats->max_symbol < 10) || ( stats->used_symbols < 2))
		return 3; // ADVANCED

	if (s == 'R')
	{
		/* Runs */
		if ((Zeros < ZEROS_UPPER) && ((Zrun < ZRUN_RUN) || (Zeros < ZEROS_RUN)))
		{
			return 3; // ADVANCED
		}
		else
		{
			return 2; // MIXED
		}
	}
	else
	{
		/* Symbols */
		if ((Zrun > ZRUN_SYM) && (Zeros > ZEROS_SYM) && (stats->avgF > AVGF_LOW))
		{
			return 2; // MIXED  
		}
		else if (stats->avgF < AVGF_HIGH)
		{
			return 1; // MARKER
		}
		else
		{
			// return 3; // ADVANCED  buggy???
			return 1; // MARKER
		}
	}
}

/*------------------------------------------------------------------
 * start_model()
 *
 * Initializing the model, and the variables used for coding.
 *-----------------------------------------------------------------*/
void
start_modelB( acModellB *M, unsigned int K)
{
	unsigned int i;
	low  = 0;
	range  = Q2;
  n_bits = 0;
	M->K = K;

	ALLOC( M->H, K+1, unsigned long);
  
  for ( i = 0; i <= K; i++)
	{
    	M->H[i] = i;
  }
}

/*------------------------------------------------------------------
 * update_model()
 *
 * Update the probability model
 *-----------------------------------------------------------------*/
void
update_modelB( unsigned long *H, unsigned int K, unsigned int symbol)
{
	unsigned int i;
    
  if (H[K] == Q1)
	{
		unsigned int diff, next_diff;
      
		next_diff = H[1] - H[0];
    for ( i = 1; i<K; i++)
		{
			diff = next_diff;
			next_diff = H[i+1] - H[i];
			H[i] =  H[i-1] + ((diff + 1) >> 1);
    }
		H[K] =  H[K-1] + ((next_diff + 1) >> 1);
  }
    
  for ( i = symbol+1; i <= K ; i++)
	{
		H[i]++;
  }
}

/*------------------------------------------------------------------
 * Encode_Core()
 *
 * The core of the coding algorithm. Receives the model, the symbol
 * and the bitbufput file to operate with.
 *-----------------------------------------------------------------*/
void
Encode_coreB( acModellB *M, unsigned int index, BitBuffer *bitbuf)
{
	unsigned long H_range;

	H_range = range / M->H[M->K];
	low += H_range * M->H[index];
	range = H_range * (M->H[index+1] - M->H[index]);

	while( range <= Q1)
	{
		if (low >= Q2)
		{
			BufferPutBit( bitbuf, 1); // bitbuf 1
			while( n_bits)
			{
				BufferPutBit( bitbuf, 0); // bitbuf 0
				n_bits--;
			}
			low -= Q2;
		}
		else if ( (low + range) <= Q2)
		{
			BufferPutBit( bitbuf, 0); // bitbuf 0
			while( n_bits)
			{
				BufferPutBit( bitbuf, 1); // bitbuf 1
				n_bits--;
			}
		}
		else
		{
			n_bits++;
			low -= Q1;
		}
		low   <<= 1;
		range <<= 1;
	}
	return;
}

/*------------------------------------------------------------------
 * Finish_EncodeB()
 *
 * Write out whats left in the buffer.
 *-----------------------------------------------------------------*/
void
Finish_EncodeB(BitBuffer *bitbuf)
{
	int nb, i, bit;
	unsigned long bits;
	
	nb = ARITH_B;
  bits  = low;

  for (i = 1; i <= nb; i++)
	{
    bit = (bits >> (nb - i) ) & 1;
		BufferPutBit( bitbuf, bit & 1);
		while( n_bits)
		{
			BufferPutBit( bitbuf, !bit); // bitbuf !bit
			n_bits--;
   	}
  }
}

/*------------------------------------------------------------------
 * AC_Adv()
 *
 * Advanced method - description is given in documentation
 *-----------------------------------------------------------------*/
void
AC_Adv( unsigned int *arr, unsigned int len, BitBuffer *bitbuf, sStats *stats)
{
	
	unsigned long n, symbol_index, delimiter = 0, i, j;
	unsigned long delimiterp1;
	acModellB *M=NULL, M1, M2;

	if (stats->hist_start == 0)
	{
		/* Search for the delimiter */
		j = stats->histogram[0];
		for ( i = 1;
			i < stats->max_symbol &&			// Added for avoiding errors
			stats->histogram[i]*100 >= j;
			i++);
		delimiter = i -1; // 1% condition

		if (delimiter == stats->max_symbol)
		{
			unsigned long th;
			th = (3 * len) >> 2;
			j = stats->histogram[0];
			for ( i = 1;
				j < th;	// Strutz
				i++)
			{
				/* compare to cumulative histogram	*/
				j += stats->histogram[i];
			}
			delimiter = i; // 75% condition - backup
			if (delimiter >= stats->max_symbol)
			{
				/* XXX this should never happen XXX*/
				delimiter = stats->max_symbol/2; //backup of backup
			}
		}
	}
	else
	{
		delimiter = stats->hist_start;
	}

	delimiterp1 = delimiter + 1; /* assisting variable	*/
	WriteFiboToStream( bitbuf, stats->max_symbol);
	WriteFiboToStream( bitbuf, delimiter);

	/* Initialization */
	//M = &M1; // Strutz
	start_modelB( &M1, delimiter + 2); /* plus zero plus ESC symbol	*/
	//M = &M2;
	start_modelB( &M2, stats->max_symbol + 1 - delimiterp1 ); /* remaining symbols	*/
 
	/* Coding the smybols */
  for ( n = 0; n < len; n++)
	{
		M = &M1;
		/* Check if symbol is from first group */
		if (arr[n] <= delimiter)
		{
			symbol_index = arr[n];
		}
		else
		{ 
			Encode_coreB( M, M->K-1, bitbuf); // Encode an ESC symbol
			/* Update Model */
			update_modelB( M->H, M->K, M->K-1);
			/* Switch to 2nd model */
			M = &M2;
			symbol_index = arr[n] - delimiterp1;
		}
		/* Code the actual symbol */
		Encode_coreB( M, symbol_index, bitbuf);
		/* Update Model */
		update_modelB( M->H, M->K, symbol_index);
	}
    
  /* Finish up the coding */
  Finish_EncodeB(bitbuf);

	/* free memory, was allocated in start_model() */
	FREE( M1.H); // Strutz
	FREE( M2.H); // Strutz

	return;
}

/*------------------------------------------------------------------
 * AC_Marker()
 *
 * Marker method - description is given in documentation
 *-----------------------------------------------------------------*/
void
AC_Marker( unsigned int *arr, unsigned int len, BitBuffer *bitbuf, sStats *stats)
{	
	unsigned int n, prev;
	acModellB *M=NULL, M1, M2;

	/* write maximum symbol	*/
	WriteFiboToStream( bitbuf, stats->max_symbol);
	/* write maximum symbol, which follows a run marker	*/
	WriteFiboToStream( bitbuf, stats->follow);

	/* Initialization */
	// Strutz M = &M1;
	start_modelB( &M1, stats->max_symbol + 1);
	// M = &M2;
	start_modelB( &M2, stats->follow + 1);
	prev = stats->max_symbol + 1;
 
	M = &M1;
	/* Coding the smybols */
  for ( n = 0; n < len; n++)
	{
		/* Check if previoues symbol was a marker */
		if (prev == stats->max_symbol)
		{
			M = &M2;
		}
		else
		{
			M = &M1;
		}
		/* Code the actual symbol */
		Encode_coreB( M, arr[n], bitbuf);
		/* Update Model */
		update_modelB( M->H, M->K, arr[n]);
		
		prev = arr[n];
	}
    
  /* Finish up the coding */
  Finish_EncodeB( bitbuf);
	//M = &M1;
	//FREE( M->H);
	//M = &M2;
	//FREE( M->H);
	FREE( M1.H);
	FREE( M2.H); // Strutz

	return;
}


/*------------------------------------------------------------------
 * AC_Mixed()
 *
 * Mixed method - description is given in documentation
 *-----------------------------------------------------------------*/
void
AC_Mixed( unsigned int *arr, unsigned int len, BitBuffer *bitbuf, sStats *stats)
{
	unsigned int n, j, symbol_index, *symbols=NULL, i, delimiter, k;
	acModellB *M=NULL, M1, M2;

	/* Check Hist_start */
	if (stats->hist_start == 0)
	{
		unsigned long th;
		th = (2 * len) >> 2;
		j = stats->histogram[0];
		for ( i = 1;
			j < th;	// Strutz
			i++)
		{
			/* compare to cumulative histogram	*/
			j += stats->histogram[i];
		}
		stats->hist_start = i; // 50% condition - backup
	}

	/* Building the compressed histogram */

	/* Emitting the zeros. Hist-Short */
	ALLOC( stats->symbols, stats->used_symbols, unsigned int);
	j = 0; k = 1;
	for ( i = stats->hist_start; i < stats->max_symbol + 1; i++)
	{
		if (stats->histogram[i] > 0)
		{
			stats->symbols[k++] = i-j; /* save gap size		*/
			j = i;
		}
	}
	stats->symbols[0] = k;	// Number of differences transmitted
	
	/* Transmitting parameters */
	delimiter = stats->symbols[1] - 1;
	for ( n = 0; n < stats->symbols[0]; n++)
	{
		WriteFiboToStream( bitbuf, stats->symbols[n]);
	}
	ALLOC( symbols, stats->max_symbol + 1, unsigned int);
	/* Creating lookup table for symbols */
	i = 0;	// new index
	j = 0;	// summing the gaps
	for ( n = 1; n < stats->symbols[0]; n++)
	{
		j += stats->symbols[n];	// jump to next symbol
		symbols[j] = i++;	// save new index
	}

	/* Initialization */
	//M = &M1;
	start_modelB( &M1, delimiter + 2);
	//M = &M2;
	start_modelB( &M2, stats->symbols[0]);
 
	/* Coding the smybols */
    for ( n = 0; n < len; n++)
	{
		M = &M1;
		/* Check if symbol is from first group */
		if (arr[n] <= delimiter)
		{
			symbol_index = arr[n];
		}
		else
		{ 
			Encode_coreB( M, M->K-1, bitbuf); // Encode an ESC symbol
			/* Update Model */
			update_modelB( M->H, M->K, M->K-1);
			M = &M2;
			/* Get the symbols smaller index */
			symbol_index = symbols[arr[n]];
		}
		Encode_coreB( M, symbol_index, bitbuf);
		/* Update Model */
		update_modelB( M->H, M->K, symbol_index);
	}
    
  /* Finish up the coding */
  Finish_EncodeB( bitbuf);
	//M = &M1;
	//FREE( M->H);
	//M = &M2;
	//FREE( M->H);
	FREE( M1.H); // Strutz
	FREE( M2.H); // Strutz
  FREE( symbols); /* Strutz */
	return;
}

/*------------------------------------------------------------------
 * AC_Encode()
 *
 * Main encoding function, handles shifting, method selection
 *-----------------------------------------------------------------*/
void
AC_Encode( unsigned int *arr, unsigned int len, BitBuffer *bitbuf, char s)
{
	sStats results;
	int method;

	/* Analyzis */
	Analysis( &results, arr, len);
	/* Method selection */
	method = SelectMethod( &results, s);

	/* Method transmitted to decoder */
	if (method == 1) /* MARKER*/
	{
		BufferPutBit( bitbuf, 1);
	}
	else
	{
		BufferPutBit( bitbuf, 0);
		if (method == 2)
			BufferPutBit( bitbuf, 1);
		else	/* Advanced	*/
			BufferPutBit( bitbuf, 0);
	}
	/* Encode */
	switch( method)
	{
	case 1:
		AC_Marker( arr, len, bitbuf, &results);
		break;
	case 2:
		/* Shift only symbol series */
		if (s == 'S')
		{
			Shift( arr, len);
			BufferPutBit( bitbuf, 1);
			/* Shift histogram and adjust parameters*/
			Shift_Histogram( &results);
			results.max_symbol = results.second + 1;
			if (results.hist_start != 0)	// Exception handling - 14.09.2012
				results.hist_start += 1;
		}
		else
		{
			BufferPutBit( bitbuf, 0);
		}
		// for debugging: ExtractSeries( arr, len, 'S');
		AC_Mixed( arr, len, bitbuf, &results);
		break;
	case 3:
		/* Shift */
		if (s == 'S')
		{
			Shift( arr, len);
			BufferPutBit( bitbuf, 1);
		}
		else
		{
			/* For run series, max_symbol is sent for revesibility */
			BufferPutBit( bitbuf, 0);
			/* Biggest symbol */
			WriteFiboToStream( bitbuf, results.max_symbol);
			Shift( arr, len);
		}
		/* Adjust parameters after shifting */
		results.max_symbol = results.second;
		if ((results.second != 0) || (results.histogram[results.second] != 0))	// Exception handling - 14.09.2012
			results.max_symbol++;
		if (results.hist_start != 0)	// Exception handling - 14.09.2012
			results.hist_start += 1;
		AC_Adv( arr, len, bitbuf, &results);
		break;
	}

	// Strutz
	if (results.histogram != NULL) FREE( results.histogram);
	if (results.symbols != NULL) FREE( results.symbols);
	return;
}


/*------------------------------------------------------------------
 * Get_Est_Symbol()
 *
 * Estimes the next coded symbol to be decoded and returns it back.
 *-----------------------------------------------------------------*/
unsigned int
Get_Est_Symbol( acModellB *M, unsigned int diff)
{
	unsigned int H_range, s, kum;

	H_range = range / M->H[M->K];
	kum = diff / H_range;
	s = M->K-1;
	while ( kum < M->H[s])
		s--;

	return s;
}

/*------------------------------------------------------------------
 * Decode_core()
 *
 * Decodes the given symbol, reads the associated bits.
 * Also receives the "diff" variable as a parameter and then returns it.
 *-----------------------------------------------------------------*/
unsigned int
Decode_coreB( acModellB *M, unsigned int index, BitBuffer *in, unsigned int diff)
{
	unsigned int H_range;

	H_range = range / M->H[M->K];
	diff -= H_range * M->H[index];
	range = H_range * (M->H[index+1] - M->H[index]);
	while ( range <= Q1)
	{					
		range <<= 1;			
		diff = (diff << 1) + BufferGetBit( in);
	}

	return diff;
}


/*------------------------------------------------------------------
 * ADC_Adv()
 *
 * Advanced method decoding - description is given in documentation
 *-----------------------------------------------------------------*/
void 
ADC_Adv( unsigned int *arr, unsigned int len, BitBuffer *in)
{
	/* Variables for decoding */
	acModellB *M=NULL, M1, M2;
	unsigned long diff;
	unsigned int s, n;
	unsigned int max_symbol, delimiter;

	/* Reading the max_symbol */
	max_symbol = GetFiboFromStream( in);
	delimiter = GetFiboFromStream( in);
     
	/* Initialization */
	M = &M1;
	start_modelB( M, delimiter+2);
	M = &M2;
	start_modelB( M, max_symbol + 1 - (delimiter + 1));
	
  diff = 0;
  for ( s = ARITH_B; s > 0 ; s--)
	{
		diff = (diff << 1) + BufferGetBit( in);
	}

	/* Decoding the symbols */
  for ( n = 0; n < len; n++)
	{
		M = &M1;
		/* Cumulative frequency prediction */
		s = Get_Est_Symbol(M,diff);
		/* Handling the ESC character */
		if (s == M->K - 1)
		{
			diff = Decode_coreB( M, s, in, diff);
			update_modelB( M->H, M->K, s);
			M = &M2;
			s = Get_Est_Symbol( M, diff);
		}
		/* Read the actual symbol's bits */
		diff = Decode_coreB( M, s, in, diff);
		update_modelB( M->H, M->K, s);
		/* Store the decoded symbol */
		arr[n] = ( M==&M1 ? s : s+delimiter+1 );
	}
	
	//M = &M1; FREE( M->H);
	//M = &M2; FREE( M->H);
	FREE( M1.H); // Strutz
	FREE( M2.H); // Strutz

	return;
}


/*------------------------------------------------------------------
 * ADC_Marker()
 *
 * Marker method decoding - description is given in documentation
 *-----------------------------------------------------------------*/
void 
ADC_Marker( unsigned int *arr, unsigned int len, BitBuffer *in)
{
	/* Variables for decoding */
	acModellB *M=NULL, M1, M2;
	unsigned long diff;
	unsigned int s, n, prev ;
	unsigned int max_symbol, follow, marker;

	max_symbol = GetFiboFromStream( in);
	follow = GetFiboFromStream( in);
     
	/* Initialization */
	M = &M1;
	start_modelB( M, max_symbol + 1);
	M = &M2;
	start_modelB( M, follow + 1);
	marker = max_symbol;
	prev = marker + 1;
	
  diff = 0;
  for ( s = ARITH_B; s > 0 ; s--)
	{
		diff = (diff << 1) + BufferGetBit( in);
	}

	M = &M1;
	/* Decoding the symbols */
  for ( n = 0; n < len; n++)
	{
		/* Check is previous symbol was a marker */
		if (prev == marker)
		{
			M = &M2;
		}
		else
		{
			M = &M1;
		}
		/* Cumulative frequency prediction */
		s = Get_Est_Symbol(M,diff);
		/* Read the actual symbol's bits */
		diff = Decode_coreB( M, s, in, diff);
		update_modelB( M->H, M->K, s);
		/* Store the decoded symbol */
		arr[n] = s;

		prev = arr[n];
	}
	
	//M = &M1; FREE( M->H);
	//M = &M2; FREE( M->H);
	FREE( M1.H); // Strutz
	FREE( M2.H); // Strutz

	return;
}


/*------------------------------------------------------------------
 * ADC_Mixed()
 *
 * Mixed method decoding - description is given in documentation
 *-----------------------------------------------------------------*/
void 
ADC_Mixed( unsigned int *arr, unsigned int len, BitBuffer *in)
{
	/* Variables for decoding */
	acModellB *M=NULL, M1, M2;
	unsigned long diff;
	unsigned int s, n, number, *symbols, delimiter, start, i;

	/* Read in parameters */
	number = GetFiboFromStream( in);
	start = GetFiboFromStream( in);
	delimiter = start - 1;
	ALLOC( symbols, number + 1, unsigned int);
	/* Creating array for used symbols */
	symbols[0] = start;
	for ( i = 1; i < number - 1; i++)
	{
		symbols[i] = symbols[i-1] + GetFiboFromStream( in);
	}
     
	/* Initialization */
	M = &M1;
	start_modelB( M, delimiter + 2);
	M = &M2;
	start_modelB( M, number );
	
  diff = 0;
  for ( s = ARITH_B; s > 0 ; s--)
	{
		diff = (diff << 1) + BufferGetBit( in);
	}

	/* Decoding the symbols */	
  for ( n = 0; n < len; n++)
	{
		M = &M1;
		/* Cumulative frequency prediction */
		s = Get_Est_Symbol(M,diff);
		/* Handling the ESC character */
		if (s == M->K - 1)
		{
			diff = Decode_coreB( M, s, in, diff);
			update_modelB( M->H, M->K, s);
			M = &M2;
			s = Get_Est_Symbol( M, diff);
		}
		/* Read the actual symbol's bits */
		diff = Decode_coreB( M, s, in, diff);
		update_modelB( M->H, M->K, s);
		/* Store the decoded symbol */
		arr[n] = ( M==&M1 ? s : symbols[s] );
	}
	
	//M = &M1; FREE( M->H);
	FREE( M1.H); // Strutz
	FREE( M2.H); // Strutz
  FREE( symbols);
	return;
}

/*------------------------------------------------------------------
 * AC_Decode()
 *
 * Main decoding function, handles shifting, method selection
 *-----------------------------------------------------------------*/
void
AC_Decode( unsigned int *arr, unsigned int len, BitBuffer *bitbuf)
{
	unsigned int method = 0, shift = 0, max = 0;

	/* Method selection */
	if (BufferGetBit( bitbuf) == 1u)
	{
		method = 1;
	}
	else
	{
		if (BufferGetBit( bitbuf) == 1u)
			method = 2;
		else
			method = 3;
	}
	/* Decode */
	switch( method)
	{
	case 1:
		ADC_Marker( arr, len, bitbuf);
		break;
	case 2:
		/* Read if shifted */
		shift = BufferGetBit( bitbuf);
		ADC_Mixed( arr, len, bitbuf);
		break;
	case 3:
		/* Read if shifted */
		shift = BufferGetBit( bitbuf);
		if (!shift)
		{
			/* For run series the max_symbol is transmitted */
			max = GetFiboFromStream( bitbuf);
			shift = 1;
		}
		ADC_Adv( arr, len, bitbuf);
		break;
	}
	/* Undo shift */
	if (shift)
		UndoShift( arr, len, max);

	return;
}

/*------------------------------------------------------------------
 * AC_adj()
 *
 * To be used with the adjacency mapping algorithm.
 *-----------------------------------------------------------------*/
void
AC_adj( PicData* pd, unsigned int numColours, unsigned int *arr, BitBuffer *out)
{
	unsigned int len = pd->size;
	unsigned int* cxarr;
	unsigned int n;
	acModellB *M=NULL, *Models;
	uint i, j, ibuf, jbuf, size = 0;
	uint nb_left = 0, nb_topl = 0, nb_top = 0, nb_topr = 0;

	cxarr = (uint*) malloc(sizeof(uint) * len);
	for(i = 0, ibuf = 0; i < pd->height; i++, ibuf+= pd->width)
	{
		for(j = 0, jbuf = ibuf; j < pd->width; j++, jbuf++)
		{
			if(i != 0 && j != 0 && j != pd->width - 1)
			{
				nb_left = arr[jbuf - 1];
				nb_topl = arr[jbuf - pd->width - 1];
				nb_top = arr[jbuf - pd->width];
				nb_topr = arr[jbuf - pd->width + 1];
			}
			else if(j == 0 && i != 0)
			{
				nb_left = nb_topl = nb_top = arr[jbuf - pd->width];
				nb_topr = arr[jbuf - pd->width + 1];
			}
			else if(i == 0 && j != 0)
			{
				nb_left = nb_topl = nb_top = nb_topr = arr[jbuf - 1];
			}
			else if(i != 0 && j == pd->width - 1)
			{
				nb_left = arr[jbuf - 1];
				nb_topl = arr[jbuf - pd->width - 1];
				nb_top = nb_topr = arr[jbuf - pd->width];
			}
			else if(i == 0 && j == 0)
			{
				nb_left = nb_topl = nb_top = nb_topr = arr[jbuf];
			}
			else
			{
				//Shouldn't be here. This case is impossible.
			}
			nb_left = colour_quantise(nb_left, numColours);
			nb_topl = colour_quantise(nb_topl, numColours);
			nb_top = colour_quantise(nb_top, numColours);
			nb_topr = colour_quantise(nb_topr, numColours);

			//Using 4 neighbours
			cxarr[jbuf] = ((nb_left + 3 * nb_top) * 3 + nb_topl) * 3 + nb_topr;
			//Using 2 neighbours
			//cxarr[jbuf] = nb_left + 3 * nb_top;
		}
	}

	for ( i = 0; i < len; i++)
	{
		if (arr[i] > size)
			size = arr[i];
	}

	WriteFiboToStream( out, size);
	WriteFiboToStream( out, len);

	/* Initialization */
	ALLOC( Models, MODEL_NUMBER, acModellB);
	for ( i = 0; i < MODEL_NUMBER; i++)
	{
		start_modelB( &Models[i], size + 1);
	}
 
	/* Coding the smybols */
	cxarr[0] = 0; //Set first model
  for ( n = 0; n < len; n++)
	{
		M = &Models[cxarr[n]]; // n between 0 and 624
		Encode_coreB( M, arr[n], out);
		/* Update Model */
		update_modelB( M->H, M->K, arr[n]);
	}
    
    /* Finish up the coding */
    Finish_EncodeB(out);
	for ( i = 0; i < MODEL_NUMBER; i++)
	{
		FREE( Models[i].H);
	}
	FREE( Models);

	return;
}

/*------------------------------------------------------------------
 * ADC_adj()
 *
 * To be used with adjacency mapping.
 *-----------------------------------------------------------------*/
void 
ADC_adj( PicData* pd, unsigned int numColours, unsigned int *arr, BitBuffer *in)
{
	/* Variables for decoding */
	acModellB *M=NULL, *Models;
	unsigned long diff;
	unsigned int* cxarr;
	unsigned int s, len;
	unsigned int i, j, ibuf, jbuf, size = 0;
	uint nb_left, nb_topl, nb_top, nb_topr;

	size = GetFiboFromStream( in);
	len = GetFiboFromStream( in);

	/* Initialization */
	ALLOC( Models, MODEL_NUMBER, acModellB);
	for ( i = 0; i < MODEL_NUMBER; i++)
	{
		start_modelB( &Models[i], size + 1);
	}
	
    diff = 0;
    for ( s = ARITH_B; s > 0 ; s--)
		diff = (diff << 1) + BufferGetBit( in);

	//Build cxarray
	cxarr = (uint*) malloc(sizeof(uint) * len);
	for(i = 0, ibuf = 0; i < pd->height; i++, ibuf+= pd->width)
	{
		for(j = 0, jbuf = ibuf; j < pd->width; j++, jbuf++)
		{
			if(i != 0 && j != 0 && j != pd->width - 1)
			{
				nb_left = arr[jbuf - 1];
				nb_topl = arr[jbuf - pd->width - 1];
				nb_top = arr[jbuf - pd->width];
				nb_topr = arr[jbuf - pd->width + 1];
			}
			else if(j == 0 && i != 0)
			{
				nb_left = nb_topl = nb_top = arr[jbuf - pd->width];
				nb_topr = arr[jbuf - pd->width + 1];
			}
			else if(i == 0 && j != 0)
			{
				nb_left = nb_topl = nb_top = nb_topr = arr[jbuf - 1];
			}
			else if(i != 0 && j == pd->width - 1)
			{
				nb_left = arr[jbuf - 1];
				nb_topl = arr[jbuf - pd->width - 1];
				nb_top = nb_topr = arr[jbuf - pd->width];
			}
			else /* Strutz, in all other cases. if(i == 0 && j == 0) */
			{
				nb_left = nb_topl = nb_top = nb_topr = 0;
			}
			nb_left = colour_quantise(nb_left, numColours);
			nb_topl = colour_quantise(nb_topl, numColours);
			nb_top = colour_quantise(nb_top, numColours);
			nb_topr = colour_quantise(nb_topr, numColours);
			cxarr[jbuf] = ((nb_left + 3 * nb_top) * 3 + nb_topl) * 3 + nb_topr;

			/* Decoding the symbols */
			M = &Models[cxarr[jbuf]];
			/* Build cx array */

			/* Cumulative frequency prediction */
			s = Get_Est_Symbol( M, diff);
			/* Read the actual symbol's bits */
			diff = Decode_coreB( M, s, in, diff);
			update_modelB( M->H, M->K, s);
			/* Store the decoded symbol */
			arr[jbuf] = s;
		}
	}
	
	for ( i = 0; i < MODEL_NUMBER; i++)
	{
		FREE( Models[i].H);
	}
	FREE( Models);
	free(cxarr);
	return;
}