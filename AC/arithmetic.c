/*******************************************************************
 *
 * File....: arithmetic.c
 * Function: Arithmetic coding - Coding functions
 * Author..: Strutz
 * Date....: 04.03.2013
 *
 ********************************************************************/
#include <assert.h>
#include "arithmetic.h"
#include "codec.h"

/*------------------------------------------------------------------
 * start_model()
 *
 * Initializing the model, and the variables used for coding.
 *-----------------------------------------------------------------*/
void
start_model( acModell *M, unsigned long K)
{
	unsigned long i;

	assert(K > 0);
	M->K = K;

	/* allocate cumulative histogram	*/
	ALLOC(M->H, K+1, unsigned long);
	assert(M->H != NULL);
	/* assume that each index/symbol appears at least one time	*/
  for ( i = 0; i <= K; i++)
	{
    	M->H[i] = i;
  }
}

/*------------------------------------------------------------------
 * free_model()
 *-----------------------------------------------------------------*/
void free_model( acModell *M)
{
	/* free cumulative histogram	*/
	FREE( M->H);
}

/*------------------------------------------------------------------
 * update_model()
 *
 * Update the probability model
 *-----------------------------------------------------------------*/
void
update_model( unsigned long *H, unsigned long K, unsigned long symbol)
{
	unsigned long i;
    
  if (H[K] == Q1)
	{
		unsigned long diff, next_diff;
      
		next_diff = H[1] - H[0];
    for ( i = 1; i < K; i++)
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
 * start_encoding()
 * *-----------------------------------------------------------------*/
void start_encoding( acState *acSt)
{
	acSt->low  = 0;
	acSt->range  = Q2;
  acSt->n_bits = 0;

}
/*------------------------------------------------------------------
 * encode_AC()
 *
 * The core of the coding algorithm. Receives the model, the symbol
 * and the bitbufput file to operate with.
 *-----------------------------------------------------------------*/
void
encode_AC( acModell *M, acState *acSt, unsigned long sym, BitBuffer *bitbuf)
{
	acSt->H_range = acSt->range / M->H[M->K];
	acSt->low += acSt->H_range * M->H[sym];
	assert( M->H[sym+1] > M->H[sym]);
	acSt->range = acSt->H_range * (M->H[sym+1] - M->H[sym]);
	assert( acSt->range );

	while( acSt->range <= Q1)
	{
		if (acSt->low >= Q2)
		{
			BufferPutBit( bitbuf, 1); // bitbuf 1
			while( acSt->n_bits)
			{
				BufferPutBit( bitbuf, 0); // bitbuf 0
				acSt->n_bits--;
			}
			acSt->low -= Q2;
		}
		else if ( (acSt->low + acSt->range) <= Q2)
		{
			BufferPutBit( bitbuf, 0); // bitbuf 0
			while( acSt->n_bits)
			{
				BufferPutBit( bitbuf, 1); // bitbuf 1
				acSt->n_bits--;
			}
		}
		else
		{
			acSt->n_bits++;
			acSt->low -= Q1;
		}
		acSt->low   <<= 1;
		acSt->range <<= 1;
	}
	return;
}

/*------------------------------------------------------------------
 * finish_encode()
 *
 * Write out whats left in the buffer.
 *-----------------------------------------------------------------*/
void finish_encode( acState *acSt, BitBuffer *bitbuf)
{
	int nb, i, bit;
	unsigned long bits = 0;

#ifdef BACK_BITS
  unsigned long roundup, value;

	for (nb = 1; nb <= B; nb++)
	{
		roundup = (1 << (B - nb)) - 1;
		bits = (acSt->low + roundup) >> (B - nb);
		value = bits << (B - nb);
		if ( (acSt->low <= value) && 
			(value + roundup) <= (acSt->low + (acSt->range - 1)) )
			break;
	}
#else
	nb = ARITH_B;
  bits  = acSt->low;
#endif
  for (i = 1; i <= nb; i++)
	{
    bit = (bits >> (nb - i) ) & 1;
		BufferPutBit( bitbuf, bit & 1);
		while( acSt->n_bits)
		{
			BufferPutBit( bitbuf, !bit); // bitbuf !bit
			acSt->n_bits--;
   	}
  }
}

/*------------------------------------------------------------------
 * start_decoding()
 *-----------------------------------------------------------------*/
void start_decoding( acState *acSt, BitBuffer *bitbuf)
{
	unsigned int b;

		acSt->low  = 0;
		acSt->range  = Q2;
		acSt->n_bits = 0;
		/* read initial bits for arithmetic decoding	*/
		acSt->ac_diff = 0;
		for ( b = ARITH_B; b > 0 ; b--)
		{
			acSt->ac_diff = (acSt->ac_diff << 1) + BufferGetBit( bitbuf);
		}

#ifdef BACK_BITS
    acSt->pos = acSt->ac_diff;
#endif

}

/*------------------------------------------------------------------
 * decode_AC()
 *
 * Decodes the given symbol, reads the associated bits.
 * Also receives the "diff" variable as a parameter and then returns it.
 *-----------------------------------------------------------------*/
unsigned long decode_AC( acModell *M, acState *acSt, BitBuffer *in)
{
	unsigned long kum, sym;

	acSt->H_range = acSt->range / M->H[M->K];
	kum = acSt->ac_diff / acSt->H_range;
	sym = M->K-1;
	while ( kum < M->H[sym])
		sym--;

	acSt->ac_diff -= acSt->H_range * M->H[sym];
	acSt->range = acSt->H_range * (M->H[sym+1] - M->H[sym]);
	while ( acSt->range <= Q1)
	{					
		acSt->range <<= 1;			
		acSt->ac_diff = (acSt->ac_diff << 1) + BufferGetBit( in);
#ifdef BACK_BITS
    acSt->pos <<= 1;			
		if (acSt->ac_diff & 1)	acSt->pos++;
#endif
	}

	return sym;
}
