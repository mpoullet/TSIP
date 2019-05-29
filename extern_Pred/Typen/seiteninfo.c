/*******************************************************************
 *
 * File....: seiteninfo.c
 * Function: Function for manipulating the SEITENINFO (side 
 *           information) structure.
 * Author..: Thomas Schatton
 * Date....: 11/02/2010
 *
 ********************************************************************/

#include "seiteninfo.h"

/*------------------------------------------------------------------
 * sInfoEntropie(SEITENINFO *s)
 *
 * Calculates the entropy of the side information data.
 *-----------------------------------------------------------------*/
float sInfoEntropie(SEITENINFO *s)
{
	const float ln2 = logf( 2);           /* ln(2) */
  float       lnN = logf( (float)s->length);   /* ln(N) */
	ULONG       *h = NULL;                /* histogram */
	UINT        i;     
  BYTE        pMax = 0;                 /* maximum pred. index */
	float       H = 0;                    /* entropy */

  /* find maximum predictor index */
  for ( i = 0; i < s->length; i++)
    if (s->predictor[i] > pMax) pMax = s->predictor[i];
  pMax++;
  
  /* return -1 if malloc fails */
  if (CALLOC( h, pMax, ULONG) > 0)
    return -1.0;

  /* count values */
  for ( i = 0; i < s->length; i++)
    h[s->predictor[i]]++;

  for ( i = 0; i < pMax; i++) /* all possible values */
  {
	  if ( h[i] > 0) /* ln(x) only defined for x > 0 */
      H += h[i] * (logf( (float)h[i]) - lnN);
  }
  H /= -(s->length * ln2);

  FREEPTR( h);

  return H;
}

/*------------------------------------------------------------------
 * serializeSeiteninfo(UINT **ptrStream, SEITENINFO *s)
 *
 * Converts a side information structure to an array of UINT. This 
 * array should be transmitted with the image data for the purpose
 * of reconstructing the image.
 *-----------------------------------------------------------------*/
int serializeSeiteninfo( UINT **ptrStream, SEITENINFO *s)
{
	UINT i;										
	UINT *stream;

  TRY( ECALLOC(*ptrStream, 1 +(UINT)ceil((float)s->length / 4), UINT, 32));
  stream = *ptrStream;

  /* first integer: length of side information */
	stream[0] = s->length;
  /* side information */
	for ( i = 0; i < s->length; i++)
		stream[1 + i / 4] |= ((UINT)(s->predictor[i]) << (8 * (i % 4)));

	return 0;
}

/*------------------------------------------------------------------
 * printSeiteninfo( SEITENINFO *s)
 *
 * Prints the current side information on stdout.
 *-----------------------------------------------------------------*/
void printSeiteninfo( SEITENINFO *s)
{
	UINT i;

	printf( "\nSide Information structure:\n");
	printf( "Length: %d\n", s->length);
	for ( i = 0; i < s->length; i++)
		printf( "Block #%d -> Predictor #%d\n", i, s->predictor[i]);
}

/*------------------------------------------------------------------
 * newSeiteninfo( SEITENINFO **ptrSInfo, UINT length)
 *
 * Constructor: Allocates and initializes a new side information
 * structure.
 *-----------------------------------------------------------------*/
int newSeiteninfo( SEITENINFO **ptrSInfo, UINT length)
{
  SEITENINFO *sInfo;
	TRY( ECALLOC( *ptrSInfo, 1, SEITENINFO, 30));

  sInfo = *ptrSInfo;

  /* initialization */
  sInfo->length = length;
  TRY( ECALLOC( sInfo->predictor, length, BYTE, 31));

	return 0;
}

/*------------------------------------------------------------------
 * newSeiteninfoFromStream( SEITENINFO *s, UINT *in)
 *
 * Converts a serialized side information stream to a SEITENINFO
 * structure.
 *-----------------------------------------------------------------*/
int newSeiteninfoFromStream( SEITENINFO **ptrS, UINT *in)
{
	UINT i;
	SEITENINFO *s;
  
  s = *ptrS;

  /* allocation and initialization */
	TRY( newSeiteninfo( &s, in[0]));
  
  /* write selected predictors */
  for (i = 0; i < s->length; i++)
	  s->predictor[i] = (in[1 + i / 4] >> (8 * (i % 4))) & 0xFF;

	return 0;
}

/*------------------------------------------------------------------
 * loadSeiteninfosFromFile( SEITENINFO ***ptrSInfo, char *file)
 *
 * Loads one or more side information structures from a file into
 * an array of SEITENINFO.
 *-----------------------------------------------------------------*/
int loadSeiteninfosFromFile( SEITENINFO ***ptrSInfo, char *file)
{
	FILE *in = NULL;  /* input file */
	char buf[BUFFER]; /* buffer */
	UINT i, c,          
       channelCnt,  /* channel count */
       sinfoLen;    /* length of side information per channel */
	SEITENINFO **sInfo;

  /* open file */
	in = fopen( file, "rb");
	if ( in == NULL) 
    return 33; /* could not open the file */
	
	/* read first row: sinfoLen, channelCnt */
	do 
	{
		fgets(buf, BUFFER, in);
	}
	while ( buf[0] == '#' || buf[0] == '\n');
	sscanf( buf, "%u %u", &channelCnt, &sinfoLen);

	/* Allocate and initialize SEITENINFO structure(s) */
  TRY( ECALLOC( *ptrSInfo, channelCnt, SEITENINFO*, 34));
  sInfo = *ptrSInfo;

  for ( i = 0; i < channelCnt; i++)
    TRY( newSeiteninfo( &(sInfo[i]), sinfoLen));
  
  /* write selected predictors */
  for ( c = 0; c < channelCnt; c++)  /* all channels */
  {
	  for ( i = 0; i < sinfoLen; i++)  /* all fields */
	  {
		  sInfo[c]->predictor[i] = (BYTE)abs( fgetc(in));

		  if ( feof( in)) return 33; /* error while reading */
	  }
  }
  /* close file */
  fclose( in);

	return 0;
}

/*------------------------------------------------------------------
 * writeSeiteninfosToFile( SEITENINFO **sInfo, char *file, 
 *                         UINT channelCnt)
 *
 * Writes one or more side information structures to a file.
 *-----------------------------------------------------------------*/
int writeSeiteninfosToFile( SEITENINFO **sInfo, char *file, 
                             UINT channelCnt)
{
	FILE *out = NULL;                 /* output file */
	UINT i, c,                        
       sinfoLen = sInfo[0]->length; /* length of side information */

  /* create/open file */
	out = fopen( file, "wb");
	if ( out == NULL) return 33; /* error while reading */

	/* write channelCnt and sinfoLen */	
	fprintf( out, "%u %u\n", channelCnt, sinfoLen);
	
  /* write selected predictors */
	for ( c = 0; c < channelCnt; c++)  /* all channels */
		for ( i = 0; i < sinfoLen; i++)  /* all fields */
			fputc( sInfo[c]->predictor[i], out);
  
  /* close file */
	fclose(out);

  return 0;
}

/*------------------------------------------------------------------
 * freeSeiteninfo( SEITENINFO *sInfo)
 *
 * Destructor: Frees a side information structure.
 *-----------------------------------------------------------------*/
void freeSeiteninfo( SEITENINFO *sInfo)
{
  /* free predictor array */
  FREEPTR( sInfo->predictor);
  /* free structure */
  FREEPTR( sInfo);
}