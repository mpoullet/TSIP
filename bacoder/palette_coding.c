/*****************************************************
 * File...: palette_coding.c
 * Desc...: red/write Palette PVL KTKI10 Team Wulffpack
 * Authors: Tilo Strutz
 * Date...: 11.07.2012
 * changed
 * 25.11.2014 bugfix start at 0 "for (pos = 0;"
 * 06.05.2015 Indexed2RGBColours() and GetIndexedColours()
 *            moved from stats.c
 ******************************************************/

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <math.h>

#include "tstypes.h"
#include "ginterface.h"
#include "stats.h"
#include "bitbuf.h"
#define LUMINANCE_BASED

/*---------------------------------------------------------
 * GetIndexedColours()
 * counting of unique colours in the image
 * an creation of colour palette
 *---------------------------------------------------------*/
unsigned long GetIndexedColours( PicData *pd, 
						 unsigned char *palette)
{
	/* pallete array requires memory of 3x numColours bytes	*/
	unsigned long numColours, colour, pidx, pos;
	unsigned long *col_ptr = NULL; /* array for all colours */
	unsigned long *idx_sort = NULL; /* keeps track of sorting of pixels	*/
		
	/* array for colours */
	ALLOC( col_ptr, pd->size, unsigned long);

	/* array for sorted indices */
	ALLOC( idx_sort, pd->size, unsigned long);

	if (pd->channel < 3 || palette == NULL)
	{
		return 0;
	}

	for (pos = 0; pos < pd->size; pos++) /* for all pixels	*/
	{
		/* combine three values to a single one.
		 * values with brightest green (component 1) get highest numbers.
		 * followed by red
		 */
		col_ptr[pos] = ((unsigned long)(pd->data[1][pos])<<16)+
									 ((unsigned long)(pd->data[0][pos])<<8) + pd->data[2][pos];
		assert( col_ptr[pos] <= 255*256*256 + 255*256 +255);
	}

	/* sort colours in adjacent order */
	heap_sort_ul( pd->size, col_ptr, idx_sort);

	/* count colours, create palette, and put index into first component */
	colour = col_ptr[0]; /* remember first colour	*/

	pidx = 0;	/* first palette index	*/

	palette[pidx++] = col_ptr[0] & 0xff; /* extract blue component */
	col_ptr[0] >>= 8;	
	palette[pidx++] = col_ptr[0] & 0xff; /* extract red component */
	col_ptr[0] >>= 8;
	palette[pidx++] = col_ptr[0] & 0xff; /* extract green component */

	/* write index of current colour into pixel array	*/
	pd->data[0][ idx_sort[0] ] = 0; /* index of first colour (darkest)	*/ 
	numColours = 1;
	for (pos = 1u; pos < pd->size; pos++)
	{
		if (colour != col_ptr[pos])
		{
			/* there is a new colour ! */
			numColours++;
			colour = col_ptr[pos];	/* remember the colour */
			palette[pidx++] = col_ptr[pos] & 0xff; /* extract blue component */
			col_ptr[pos] >>= 8;
			palette[pidx++] = col_ptr[pos] & 0xff; /* extract red component */
			col_ptr[pos] >>= 8;
			palette[pidx++] = col_ptr[pos] & 0xff; /* extract green component */
			/* write index of current colour into pixel array	*/
		}		
		/* write index of current colour into pixel array	*/
		pd->data[0][ idx_sort[pos] ] = numColours-1; /* index of colour */ 
	}

	if (pd->transparency == 1)
	{ /* set pixels with full transparency to a new colour index	*/
		for (pos = 0u; pos < pd->size; pos++)
		{
			if ( pd->data[3][pos] == 0)
			{
				pd->data[0][pos] = numColours; /* index, which is not in palette	*/
			}
		}
	}
#ifdef Q_DEBUG
	for (i = 0u; i < pd->size; i++)
	{
		pidx = pd->data[0][i];
		if ( palette[pidx*3] != pd->data[2][i]  ||
				 palette[pidx*3+2] != pd->data[1][i] )
		{
			pidx = pidx;
		}
	}
#endif
	FREE( col_ptr);
	FREE( idx_sort);
	return numColours;
}

/*---------------------------------------------------------
 * Indexed2RGBColours()
 *---------------------------------------------------------*/
void Indexed2RGBColours( PicData *pd, unsigned char *palette,
												unsigned int numOfColours)
{
	/* pallete array requires memory of 3x numColours bytes	*/
	unsigned int i;
	unsigned long pidx;

	assert( pd->channel >= 3);
	for (i = 0u; i < pd->size; i++)
	{
		pidx = pd->data[0][i];	/* get used index	*/
		if (pidx == numOfColours)
		{	/* this is index for binary transparancy, use arbitrary colour	*/
			pd->data[0][i] = 200;	/* red 	*/
			pd->data[1][i] = 10;	/* green	*/
			pd->data[2][i] = 200;		/* blue	*/
			if (pd->transparency == 1) pd->data[3][i] = 0;		/* alpha	*/
		}
		else
		{
			/* restore corresponding colours	*/
			pd->data[2][i] = palette[pidx*3];		/* blue	*/
			pd->data[0][i] = palette[pidx*3+1];	/* red 	*/
			pd->data[1][i] = palette[pidx*3+2];	/* green	*/
			if (pd->transparency == 1) pd->data[3][i] = 255;		/* alpha	*/
		}
	}
}

/*---------------------------------------------------------
 * GetIndexedColours_2()
 * counting of unique colours in the image
 *---------------------------------------------------------*/
unsigned long GetIndexedColours_2(PicData * pd, unsigned char *palette)
{
  unsigned long j3;
	unsigned long pos, j=0;               /* j is the last used colour */
	unsigned long long *key_ptr = NULL;	/* array for the key to sort by */
	unsigned long *idx_sort = NULL;	/* keeps track of sorting of pixels     */
  bool *processed = NULL; /* marks if this entry was already processed */
	bool done;

	/* array for colours */
	ALLOC(key_ptr, pd->size, unsigned long long);
	if (ERRCODE != 0)
		return 0;
	/* array for sorted indices */
	ALLOC(idx_sort, pd->size, unsigned long);
	if (ERRCODE != 0)
		return 0;

	/* array that keeps track what was already processed */
	ALLOC(processed, pd->size, bool);
	if (ERRCODE != 0)
		return 0;

	if (pd->channel < 3 || pd->channel > 4 || palette == NULL)
	{
		fprintf(stderr,
			"Indiced palettes with more than 3 colour components are not supported!\n");
		return 0;
	}

	for (pos = 0; pos < pd->size; pos++) /* for all pixels */
	{
    processed[pos] = false;	/* reset flags	*/
    #ifdef LUMINANCE_BASED
      // luminance: Y = 0.299R + 0.587G + 0.114B
      key_ptr[pos] =  299lu * (pd->data[0][pos]) +   /* R */
                      587lu * (pd->data[1][pos]) +   /* G */
                      114lu * (pd->data[2][pos]);    /* B */
    #else
      /* combine three values to a the intensity.
       * whereas I = (R+G+B)/3 - but the division does not change the ordering
       */
      key_ptr[i] =pd->data[1][i] + pd->data[0][i] + pd->data[2][i];
    #endif

      // optional - seems to be only good for bigger palettes
      //  key_ptr[i] = (key_ptr[i] << 32) + (pd->data[1][i] << 16) + (pd->data[0][i] << 8) + (pd->data[2][i] << 0);
	}

	/* sort colours ascending by key_ptr */
	heap_sort_ull( pd->size, key_ptr, idx_sort);

  /* store colour (B,R,G) of darkest pixel in palette	*/
	j = 0;
	j3 = 0; /* j * 3	*/
	palette[0] = (unsigned char)pd->data[2][ idx_sort[j] ];
  palette[1] = (unsigned char)pd->data[0][ idx_sort[j] ];
  palette[2] = (unsigned char)pd->data[1][ idx_sort[j] ];
  pd->data[0][ idx_sort[j] ] = j; // put in new index
  processed[0] = true;

	/* write index of current colour into pixel array       */
	do 
	{
		done = true;
		for ( pos = 1u; pos < pd->size; pos++) 
		{
      unsigned int *r, *b, *g;

			if (processed[pos])   continue;	// already processed

			r = &(pd->data[0][idx_sort[pos]]);
			g = &(pd->data[1][idx_sort[pos]]);
			b = &(pd->data[2][idx_sort[pos]]);

			//if  (pd->data[2][idx_sort[i]] == palette[j * 3 + 0] && 
			//					pd->data[0][idx_sort[i]] == palette[j * 3 + 1] &&
			//					pd->data[1][idx_sort[i]] == palette[j * 3 + 2])
			if  (*b == palette[j3 + 0] && 
					 *r == palette[j3 + 1] &&
					 *g == palette[j3 + 2])
			{
				/* colour in sorted array is the same as the current palette entry	*/
				//pd->data[0][idx_sort[i]] = j; /* write colour index to pixel	*/
				r[0] = j; /* write colour index to pixel	*/
        processed[pos] = true;	/* mark pixel as processed	*/
        done = false;	/* need more loops	*/
			} 
			else if (done ||  /* is this color a new one? , start of a new block of colours */
									/* check monotony of RGB values	*/
                 (*b >= palette[j3 + 0] &&	
						      *r >= palette[j3 + 1] && 
									*g >= palette[j3 + 2])) 
         //        (pd->data[2][idx_sort[i]] >= palette[j * 3 + 0] &&	
						   //   pd->data[0][idx_sort[i]] >= palette[j * 3 + 1] && 
									//pd->data[1][idx_sort[i]] >= palette[j * 3 + 2])) 
			{
        /* yes is a new colour with non-motonely increasing RGB value	*/
        j++;	/* next palette index	*/
				j3 += 3;
				/* write colour into palette 	*/
				palette[j3] = (unsigned char)*b;
				palette[j3 + 1] = (unsigned char)*r;
				palette[j3 + 2] = (unsigned char)*g;
				//pd->data[0][idx_sort[i]] = j; /* write colour index to pixel	*/
				r[0] = j; /* write colour index to pixel	*/
        processed[pos] = true;	/* mark pixel as processed	*/
				done = false;	/* something was changed, need more loops	*/
      }
		}
	} while (!done);

	if (pd->transparency == 1)
	{ /* set pixels with full transparency to new maximum colour index	*/
		for (pos = 0; pos < pd->size; pos++)
		{
			if ( pd->data[3][pos] == 0)
			{
				pd->data[0][pos] = j+1;	/* index, which is not in palette	*/
			}
		}
	}

	FREE( processed);
	FREE( key_ptr);
	FREE( idx_sort);
	return j+1;	/* number of different colours, size of palette	*/
}

/*--------------------------------------------------------
 * write_palette_2()
 * palette:  BRG BRG BRG ...
 *--------------------------------------------------------*/
void write_palette_2( BitBuffer * bitbuf, unsigned int numColours, 
										 unsigned char *palette)
{
	const int RESET = 10;
	unsigned int k;		/* coding parameter     */
	int N[5];		/* number of occurences per context     */
	int A[5];		/* sum of differences per context */
	int val;	  /* value to be transmitted       */
	unsigned long i, j;	/* i=current color, j=last index of block+1*/
	unsigned char col;	  /* colour (mostly used as context) */

	/* basic check  */
	assert(palette != NULL);
	if (palette == NULL)
		return;
	assert(bitbuf != NULL);
	if (bitbuf == NULL)
		return;

	for (col = 0; col < 3; col++) 
		N[col] = A[col] = 1;      // Context 0-2: Colours
	N[3] = 1; A[3] = 20;		// Context 3: Block-runlength
	N[4] = 1; A[4] = 1;		  // Context 4: differences to prev. block start

  j = 0;
	for (i = 0; i < numColours; i++) 
	{
		if (i == 0 || palette[(i - 1) * 3 + 0] > palette[i * 3 + 0]
		    || palette[(i - 1) * 3 + 1] > palette[i * 3 + 1]
		    || palette[(i - 1) * 3 + 2] > palette[i * 3 + 2])
		{
			// if new block begins here -> transmit length
			
			// count block length
			for (j = i + 1;
			     j < numColours && (palette[(j - 1) * 3 + 0] <= palette[j * 3 + 0]
						&& palette[(j - 1) * 3 + 1] <= palette[j * 3 + 1]
						&& palette[(j - 1) * 3 + 2] <= palette[j * 3 + 2]);
			     j++) ;

      {
        register int nst = N[3], At = A[3];
        for ( k = 0; nst < At; nst <<= 1, k++);
      }

      val = j - i;
			rice_encoding( bitbuf, val, k);
			
      if ((unsigned long)A[3] + val > UINT_MAX || N[3] > RESET) 
			{
				A[3] >>= 1; N[3] >>= 1;
			}
			A[3] += val; N[3] ++;


      // transmit "start block colors"
      // either as difference(1) to prev. block start
      // or as absolute(0) value
      {
			  static unsigned long prev_block_start=0; 
        bool diff = i > prev_block_start;

        for (col = 0; col < 3; col++) /* for  B R G values	*/
				{
          {
            register int nst = N[4], At = A[4];
            for ( k = 0; nst < At; nst <<= 1, k++);
          }

          if (diff)
            val = palette[i*3+col] - palette[prev_block_start * 3 + col];
          else
            val = palette[i*3+col];
          
          if (val > 0) /* sign is only required in case of diff==true	*/
            BufferPutBit( bitbuf, 0); // sign: 0 positive
          else 
            BufferPutBit( bitbuf, 1); // sign: 1 negative
          rice_encoding( bitbuf,  abs(val), k);

          if ((unsigned long)A[4] + abs(val) > UINT_MAX || N[4] > RESET) 
					{
            A[4] >>= 1; N[4] >>= 1;
          }
          A[4] += abs(val); N[4] ++;
        }
        prev_block_start = i;      
      }
		}  
		else 
		{
      // transmit color ("normal mode")
      for (col = 0; col < 3; col++) 
			{
        {
          register int nst = N[col], At = A[col];
          for ( k = 0; nst < At; nst <<= 1, k++);
        }

        val = palette[i * 3 + col] - palette[(i - 1) * 3 + col];
        rice_encoding( bitbuf, val, k);

        if ((unsigned long)A[col] + abs(val) > UINT_MAX || N[col] > RESET) 
				{
          A[col] >>= 1; N[col] >>= 1;
        }
        A[col] += abs(val); N[col]++;
      }
    }
	}

}

/*--------------------------------------------------------
 * read_palette_2()
 * palette:  BRG BRG BRG ...
 *--------------------------------------------------------*/
void read_palette_2( BitBuffer * bitbuf, unsigned int numColours, 
										 unsigned char *palette)
{
	const int RESET = 10;
	unsigned int k;		/* coding parameter     */
	int N[5];		/* number of occurences per context     */
	int A[5];		/* sum of differences per context */
	int val;	/* colour       */
  char sign;
	unsigned long i, j;	/* context      */
	unsigned char cx;	/* colour (mostly used as context) */


  //printf("read_palette");

	/* basic check  */
	assert(palette != NULL);
	if (palette == NULL)
		return;
	assert(bitbuf != NULL);
	if (bitbuf == NULL)
		return;

	for (cx = 0; cx < 3; cx++) 
	{
		N[cx] = A[cx] = 1;      // Context 0-2: Colours
	}
	N[3] = 1; A[3] = 20;		// Context 3: Block-runlength
	N[4] = 1; A[4] = 1;		  // Context 4: differences to prev. block start

  j = 0;
  for ( i = 0; i < numColours; i++) 
	{
      if(j==i) 
			{ 
        // new block begins here
        {
          register int nst = N[3], At = A[3];
          for ( k = 0; nst < At; nst <<= 1, k++);
        }

        val = rice_decoding(bitbuf,k);
        j = i+val;

        if ((unsigned long)A[3] + val > UINT_MAX || N[3] > RESET) 
				{
          A[3] >>= 1; N[3] >>= 1;
        }
        A[3] += val; N[3] ++;


        // receive "start block colors"
        // either as difference(1) to prev. block start
        // or as absolute(0) value
        {
          static unsigned long prev_block_start=0; 
          bool diff = i > prev_block_start;
    
          for (cx = 0; cx < 3; cx++) 
          {
            {
              register int nst = N[4], At = A[4];
              for ( k = 0; nst < At; nst <<= 1, k++);
            }

            sign = BufferGetBit(bitbuf);
            val = rice_decoding(bitbuf, k);
            if (sign)
              val = -val;

            if(diff)
              palette[i*3+cx] = (unsigned char)(palette[prev_block_start*3+cx] + val);
            else
              palette[i*3+cx] = (unsigned char)val;

            if ((unsigned long)A[4] + abs(val) > UINT_MAX || N[4] > RESET) 
						{
              A[4] >>= 1; N[4] >>= 1;
            }
            A[4] += abs(val); N[4] ++;
          }
          prev_block_start = i;      
        }
      } 
			else 
			{
        // transmit color ("normal mode")
        for (cx = 0; cx < 3; cx++) 
				{
          {
            register int nst = N[cx], At = A[cx];
            for ( k = 0; nst < At; nst <<= 1, k++);
          }

          val = rice_decoding(bitbuf, k);
          palette[i*3+cx] = (unsigned char)(palette[(i-1)*3+cx] + val);

          if ((unsigned long)A[cx] + abs(val) > UINT_MAX || N[cx] > RESET) 
					{
            A[cx] >>= 1; N[cx] >>= 1;
          }
          A[cx] += abs(val); N[cx]++;
        }
      }
    } // for i
    //printf( "end read palette");
}


/*--------------------------------------------------------
* write_palette()
* write GRB triplets in compressed form to stream
* palette:  GRB 
* Example:
*  G  R  B
*  1  2  8
*  1  4  3
*  2  2  2
*  3  3  1
*  3  3  3
* encoding of G:
*     1 0 1 1 0
* encoding of R:
*     2 2 2 3 0
* encoding of B:
*     8 3 2 1 2
*--------------------------------------------------------*/
void write_palette( BitBuffer *bitbuf, 
									 unsigned int numColours, 
									 unsigned char *palette)
{
	const int RESET = 10;
	int k;	/* coding parameter	*/
	int N[2];	/* number of occurences per context	*/
	int A[2];	/* number of occurences per context	*/
	unsigned int	val;		/* colour	*/
	unsigned int	i, cx;		/* context	*/

	/* basic check	*/
	assert( palette != NULL);
	if (palette == NULL) return;
	assert( bitbuf != NULL);
	if (bitbuf == NULL) return;

	/* transmit green, assuming that all green 
	 * values are in ascending order
	 * this was already realised in GetIndexedColours()
	 */
	cx = 0;
	N[cx] = 1; /* use a single context for green	*/
	A[cx] = 1; /* use a single context for green	*/
	for ( i = 0; i < numColours; i++)
	{
		/* determine coding parameter k for Rice coding	*/
		if (numColours > 30)
		{
			register int nst = N[cx], At = A[cx];
			for ( k = 0; nst < At; nst <<= 1, k++);
		}
		else
		{
			k = 6; /* almost flat distribution	*/
		}
		if ( i == 0)
		{
			/* the first value is transmitted directly	*/
			val = (unsigned int)palette[i*3+2]; /* extract green from triple	*/
		}
		else	/* use difference between green values	*/
		{
			assert( palette[i*3+2] >= palette[(i-1)*3+2]);
			val = (unsigned int)(palette[i*3+2] - palette[(i-1)*3+2]);
		}
		rice_encoding( bitbuf, val, k); /* encoding of the value/difference	*/

		/* accumulate values for determination of coding parameter	*/
		A[cx] += val; 
		/* Increment occurence count */
		N[cx]++; 

		/* Rescale N, A and B if the counts get too large */
		if (N[cx] == RESET) 
		{ 
			A[cx] >>= 1; 
			N[cx] >>= 1; 
		}
	}


	/* transmit red, assuming that all red 
	 * values are in ascending order when green
	 * does not change
	 */
	N[0] = N[1] = 1; 
	A[0] = 32;
	A[1] = 64; 
	/* for all red values		*/
	for ( i = 0; i < numColours; i++)
	{
		if ( i == 0)
		{
			cx = 0;
			k = 1;
			val = (unsigned int)palette[i*3+1]; /* extract red	*/
		}
		else
		{
			/* if green has not changed take cx = 0
			* high probability that also red does not change very much
			*/
			if (palette[i*3+2] == palette[(i-1)*3+2])
			{
				/* determine coding parameter k for Rice coding	*/
				cx = 0;
				if (numColours > 30)
				{
					register int nst = N[cx], At = A[cx];
					for ( k = 0; nst < At; nst <<= 1, k++);
				}
				else k = 6;
				/* use differential coding	*/
				assert( palette[i*3+1] >= palette[(i-1)*3+1]);
				val = (unsigned int)(palette[i*3+1] - palette[(i-1)*3+1]); /* extract red differentially	*/
			}
			else
			{
				cx = 1;

				/* determine coding parameter k for Rice coding	*/
				if (numColours > 30)
				{
					register int nst = N[cx], At = A[cx];
					for ( k = 0; nst < At; nst <<= 1, k++);
				}
				else k = 6;
				val = (unsigned int)palette[i*3+1]; /* extract red	*/
			}
		}
		rice_encoding( bitbuf, val, k); /* encoding of the value/difference	*/

		/* accumulate values for determination of coding parameter	*/
		A[cx] += val;
		/* Increment occurence count */
		N[cx]++; 

		/* Rescale N, A and B if the counts get too large */
		if (N[cx] == RESET) 
		{ 
			A[cx] >>= 1; 
			N[cx] >>= 1; 
		}
	}


	/* transmit blue, assuming that all blue 
	* values are in ascending order when red
	* does not change
	*/
	N[0] = N[1] = 1; 
	A[0] = 32;
	A[1] = 64; 
	/* for all blue values		*/
	for ( i = 0; i < numColours; i++)
	{
		if ( i == 0)
		{
			cx = 0;
			k = 1;
			val = (unsigned int)palette[i*3]; /* extract blue	*/
		}
		else
		{
			/* if green and red have not changed take cx = 0
			* high probability that also red does not change very much
			*/
			if (palette[i*3+2] == palette[(i-1)*3+2] &&
				  palette[i*3+1] == palette[(i-1)*3+1])
			{
				cx = 0;
				if (numColours > 30)
				{
					register int nst = N[cx], At = A[cx];
					for ( k = 0; nst < At; nst <<= 1, k++);
				}
				else k = 6;
				/* use differential coding	*/
				assert( palette[i*3] >= palette[(i-1)*3]);
				val = (unsigned int)(palette[i*3] - palette[(i-1)*3]); /* extract blue diff	*/
			}
			else
			{
				cx = 1;

				if (numColours > 30)
				{
					register int nst = N[cx], At = A[cx];
					for ( k = 0; nst < At; nst <<= 1, k++);
				}
				else k = 6;
				val = (unsigned int)palette[i*3]; /* extract blue	*/
			}
		}
		rice_encoding( bitbuf, val, k);

		A[cx] += val;
		/* Increment occurence count */
		N[cx]++; 

		/* Rescale N, A and B if the counts get too large */
		if (N[cx] == RESET) 
		{ 
			A[cx] >>= 1; 
			N[cx] >>= 1; 
		}
	}
}

/*--------------------------------------------------------
* read_palette()
* palette:  BRG BRG BRG ...
*--------------------------------------------------------*/
void read_palette( BitBuffer *bitbuf, 
									unsigned int numColours, 
									unsigned char *palette)
{
	const int RESET = 10;
	int k;	/* coding parameter	*/
	int N[2];	/* number of occurences per context	*/
	int A[2];	/* number of occurences per context	*/
	unsigned int	val;		/* colour	*/
	unsigned int	i, cx;		/* context	*/

	/* basic check	*/
	assert( palette != NULL);
	if (palette == NULL) return;
	assert( bitbuf != NULL);
	if (bitbuf == NULL) return;

	/* receive green, assuming that all green 
	* values are in ascending order
	*/
	cx = 0;
	N[cx] = 1; /* use a single context for green	*/
	A[cx] = 1; /* use a single context for green	*/
	for ( i = 0; i < numColours; i++)
	{
		if (numColours > 30)
		{
			register int nst = N[cx], At = A[cx];
			for ( k = 0; nst < At; nst <<= 1, k++);
		}
		else k = 6;

		val = rice_decoding( bitbuf, k);
		if ( i == 0) palette[i*3+2] = (unsigned char)val; /* put green	*/
		else	/* use difference	*/
		{
			/* new is old plus val( =difference)	*/
			palette[i*3+2] = (unsigned char)val + palette[(i-1)*3+2];
		}

		A[cx] += val;
		/* Increment occurence count */
		N[cx]++; 

		/* Rescale N, A and B if the counts get too large */
		if (N[cx] == RESET) 
		{ 
			A[cx] >>= 1; 
			N[cx] >>= 1; 
		}
	}


	/* receive red, assuming that all red 
	* values are in ascending order when green
	* does not change
	*/
	N[0] = N[1] = 1; 
	A[0] = 32;
	A[1] = 64; 
	for ( i = 0; i < numColours; i++)
	{
		if ( i == 0)
		{
			cx = 0;
			k = 1;
			val = rice_decoding( bitbuf, k);
			palette[i*3+1] = (unsigned char)val; /* put red	*/
		}
		else
		{
			/* if green has not changed take cx = 0
			* high probability that also red does not change very much
			*/
			if (palette[i*3+2] == palette[(i-1)*3+2])
			{
				cx = 0;
				if (numColours > 30)
				{
					register int nst = N[cx], At = A[cx];
					for ( k = 0; nst < At; nst <<= 1, k++);
				}
				else k = 6;

				val = rice_decoding( bitbuf, k);
				palette[i*3+1] = (unsigned char)val + palette[(i-1)*3+1]; /* put red	*/
			}
			else
			{
				cx = 1;
				if (numColours > 30)
				{
					register int nst = N[cx], At = A[cx];
					for ( k = 0; nst < At; nst <<= 1, k++);
				}
				else k = 6;
				val = rice_decoding( bitbuf, k);
				palette[i*3+1] = (unsigned char)val; /* put red	*/
			}
		}

		A[cx] += val;
		/* Increment occurence count */
		N[cx]++; 

		/* Rescale N, A and B if the counts get too large */
		if (N[cx] == RESET) 
		{ 
			A[cx] >>= 1; 
			N[cx] >>= 1; 
		}
	}


	/* receive blue, assuming that all blue 
	* values are in ascending order when red
	* does not change
	*/
	N[0] = N[1] = 1; 
	A[0] = 32;
	A[1] = 64; 
	for ( i = 0; i < numColours; i++)
	{
		if ( i == 0)
		{
			cx = 0;
			k = 1;
			val = rice_decoding( bitbuf, k);
			palette[i*3] = (unsigned char)val; /* put blue	*/
		}
		else
		{
			/* if red has not changed take cx = 0
			* high probability that also red does not change very much
			*/
			if (palette[i*3+2] == palette[(i-1)*3+2] &&
				palette[i*3+1] == palette[(i-1)*3+1])
			{
				cx = 0;
				if (numColours > 30)
				{
					register int nst = N[cx], At = A[cx];
					for ( k = 0; nst < At; nst <<= 1, k++);
				}
				else k = 6;
				/* use differential coding	*/
				val = rice_decoding( bitbuf, k);
				palette[i*3] = (unsigned char)val + palette[(i-1)*3]; /* put blue	*/
			}
			else
			{
				cx = 1;
				if (numColours > 30)
				{
					register int nst = N[cx], At = A[cx];
					for ( k = 0; nst < At; nst <<= 1, k++);
				}
				else k = 6;
				val = rice_decoding( bitbuf, k);
				palette[i*3] = (unsigned char)val; /* put blue	*/
			}
		}

		A[cx] += val;
		/* Increment occurence count */
		N[cx]++; 

		/* Rescale N, A and B if the counts get too large */
		if (N[cx] == RESET) 
		{ 
			A[cx] >>= 1; 
			N[cx] >>= 1; 
		}
	}
}