/*******************************************************************
 *
 * File....: main.c
 * Function: Command line interface.
 * Author..: Thomas Schatton
 * Date....: 11/05/2010
 *
 ********************************************************************/

#include <string.h>
#include "prediction.h"
#include "Typen\data.h"
#include "Module\predictors.h" /* for getPredictorName(...) */
#include "Module\errordb.h"    /* for getErrorName(...) */
#include "errors.c"

static char *progName;      /* Program name */

/*------------------------------------------------------------------
 * startTraining( ...)
 *
 * Starts an offline context training. The specified image will be
 * processed using all available predictors. The calculated
 * prediction errors will then be used to update the SAD array in
 * the context data. The context data will be written to the file
 * at dbFile.
 *-----------------------------------------------------------------*/
static int startTraining( char* inputFile, char* dbFile)
{
  DATA *in = NULL;
  ERRORDB *db = NULL;
  UINT i;

  /* load files */
  TRY( loadData( &in, inputFile, NULL));
  if ( loadErrorDBFromFile( &db, dbFile) == 3) /* new file */
  {  
    printf( "\nCreating new data file '%s'...\n", dbFile);
    db = NULL;
  }

  /* start training */
  /*
  for ( farbraum = RGB; farbraum <= YrUrVr; farbraum++)
  {
    TRY( dataTransform( in, farbraum));
  */  
    printf("Processing '%s' (%d channels)", 
           inputFile, in->channelCnt);
    for ( i = 0; i < in->channelCnt; i++) /* all channels */
	  {
      printf(" ..", i);
      TRY( selectionTraining( in->img[i], &db));
	  }
    printf("\n");
  /*}*/
  
  /* save context data */
  TRY( writeErrorDBToFile( db, dbFile));

  return 0;
}

/*------------------------------------------------------------------
 * start( ...)
 *
 * Reads a PPM or PGM file and a side information file if necessary.
 * The image data will be processed with the specified parameters
 * and the resulting prediction error image or reconstructed image
 * written to outputFile. A side information file will be created
 * if needed.
 * 'farbraum' specifies whether the color space of the input image 
 * should be transformed to RGB (0), YCgCo (1) or YrUrVr (2) prior
 * to prediction.
 *-----------------------------------------------------------------*/
static int start(char* inputFile, char* outputFile, char* sinfoFile,
                 char* dataFile, PARAMETER *p, BOOL reconstruction, 
                 int farbraum, BOOL tabular, char* psImageFile)
{
	DATA *in = NULL, *out = NULL; /* input, output */
  ERRORDB *db = NULL;
	UINT i;  

  /* read context data if specified */
  if (dataFile != NULL)
  {
    if (tabular == FALSE) 
      printf("\nLoading %s...", dataFile);
    TRY( loadErrorDBFromFile( &db, dataFile));
  }

	/* read image data */
	TRY( loadData( &in, inputFile, sinfoFile));

  if (reconstruction == FALSE)
  {
    TRY( dataTransform( in, farbraum)); /* color space */
  }
  else
    in->farbraum = farbraum;

  /* read side information */
	if (reconstruction == TRUE && p->blockMode == TRUE)
		TRY( loadSeiteninfosFromFile( &(in->sInfo), in->sInfoFile));
  
  /* create data structure */
	TRY( newData( &out, in->channelCnt, in->sInfoFile, farbraum, 
                psImageFile));
 
  /* print status */
  if (tabular == FALSE)
    printf("\nStarting %s...\n", 
      reconstruction == FALSE ? "prediction" : "reconstruction");

	/* process each channel */
	for ( i = 0; i < in->channelCnt; i++) /* all channels */
	{
    if ( reconstruction == FALSE)
    { /* prediction */
      if (out->psImgFile != NULL)
      {
        TRY( newBild( &(out->psImg[i]), (in->img[i])->width, 
                      (in->img[i])->height, (in->img[i])->bitDepth));
      }

		  TRY( predict( &(out->img[i]), in->img[i], p, &out->sInfo[i], 
                    &db, out->psImg[i]));
    }
    else
    { /* reconstruction */
		  TRY( reconstruct( &(out->img[i]), in->img[i], p, &in->sInfo[i], &db));
    }

    if (tabular == FALSE)
      printf("Channel #%d processed.\n", i + 1);
	}
  
	/* write output file */
  if (reconstruction == TRUE)
  {
    TRY( dataTransform( out, RGB));
  }

  TRY( writeData( out, outputFile));

  /* compute entropies */
  if (reconstruction == FALSE)
  {
    char  *strFarbraumOut;
   
    strFarbraumOut = 
      farbraum == YCgCo ? "YCgCo" : farbraum == YrUrVr ? "YrUrVr" : "RGB";

    if (tabular == FALSE)
    {
      printf( "\nEntropy   Source (%s) Output (%s)\n", 
              strFarbraumOut, strFarbraumOut);
      for ( i = 0; i < in->channelCnt; i++)
      {
        printf( "Channel #%d:  %3.3f   ->   %3.3f\n", i + 1,
                entropie( in->img[i]), entropie( out->img[i]));
      }
    }
    else
    {
      for ( i = 0; i < in->channelCnt; i++)
      {
        printf( "%s %d %s %f %f\n", 
                inputFile, i + 1, strFarbraumOut,
                entropie( in->img[i]), entropie( out->img[i]));
      }
    }

    /* print predictor selection saved in the side information */
    if (p->blockMode == TRUE && tabular == FALSE)
    {
      UINT *pCount, j;
      char **pName;

      TRY( ECALLOC( pCount, p->anzPredictors, UINT, 2));
      TRY( ECALLOC( pName, p->anzPredictors, char*, 2));
      
      /* count selected predictors */
      for ( i = 0; i < out->channelCnt; i++)
        for ( j = 0; j < out->sInfo[0]->length; j++)
          pCount[out->sInfo[i]->predictor[j]]++;

      /* get predictor names */
      j = 0;
      for (i = 0; i < PREDICTORCOUNT; i++)
        if (p->predictors[i] == TRUE)
          pName[j++] = getPredictorName( i);

      printf("\nSelected predictors (%s):\n", out->sInfoFile);
      for ( i = 0; i < j; i++)
        printf( "%-40s: %dx\n", pName[i], pCount[i]);

      FREEPTR( pCount);
      FREEPTR( pName);
    }
  }

  return 0;
}

/*------------------------------------------------------------------
 * usage( int page)
 *
 * Prints information about command line arguments on stderr.
 *-----------------------------------------------------------------*/
void usage( int page)
{
  int i;

  switch (page)
  {
    default: /* general info */
      fprintf( stderr, "\nUsage: %s [files] [options]\n",
           progName);
      fprintf( stderr, "\
Mandatory parameters [files]:                                  \n\
 -i<file>   input image [.ppm/.pgm]                               \n\
 -o<file>   output image [.ppm/.pgm]                              \n\
Optional parameters [options]:                                    \n\
 -s<file>   side information [.ppm/.pgm] (if -bm is set)          \n\
 -d<file>   context data (see -hd)                                \n\
 -u<file>   write predictor selection image (if -bm or -cs is set)\n\
 -r         image reconstruction [default: prediction]            \n\
 -bm        use block-based prediction [default: off]             \n\
 -bw<#>     block width [default: 16px]                           \n\
 -bh<#>     block height [default: 16px]                          \n\
 -cp<#>     pixels used for context modelling [default: 0]        \n\
 -cg<#>     gradients used for context modelling [default: 3]     \n\
 -cs        in context data mode, do not calculate mean values    \n\
 -pp<flags> enable/disable predictors [default: 1..1]             \n\
 -pb[flags] enable/disable bias compensation [default: 0..0]      \n\
 -pbr       adaption frequency of bias compensation [default: 64] \n\
 -yc        prediction in YCgCo color space [default: RGB]        \n\
 -yr        prediction in YrUrVr color space [default: RGB]       \n\
 -t         short, tabular output of resulting entropies          \n\
Further information:                                              \n\
 -hp        information about the syntax of '-pp'                 \n\
 -hb        information about the syntax of '-pb'                 \n\
 -hd        information about the argument '-d<file>'            \n");
      break;
    case 1: /* information about -pp */
      fprintf( stderr, "\n\
Information about the argument '-pp<flags>':                      \n\
This option specifies the activated predictors. It consists of a  \n\
%d characters long string of 1s and 0s:                           \n\
  - a '0' at position X in the string denotes that predictor #X   \n\
    of the predictor module will be disabled.                     \n\
  - a '1' at position X in the string denotes that predictor #X   \n\
    of the predictor module will be enabled.                      \n\
For example, the argument '-pp1100' would enable the predictors #0\n\
and #1, while predictors #2 and #3 would be disabled.            \n",
      PREDICTORCOUNT);
      usage(3);
      break;
    case 2: /* information about -pb */
      fprintf( stderr, "\n\
Information about the argument '-pb[flags]':                      \n\
This option specifies which predictors use a bias compensation.   \n\
If the argument '-pb' is specified without a following string, the\n\
bias compensation will be enabled for all available predictors.   \n\
Otherwise, the %d characters long string of 1s and 0s will be     \n\
processed as follows:                                             \n\
  - a '0' at position X in the string denotes that predictor #X   \n\
    of the prediction module will not be concatenated with a bias \n\
    compensation.                                                 \n\
  - a '1' at position X in the string denotes that predictor #X   \n\
    of the prediction module will be concatenated with a bias     \n\
    compensation.                                                 \n\
For example, the argument '-pb1100' would activate the bias       \n\
compensation, but only for the predictors #0 and #1.             \n",
      PREDICTORCOUNT);
      usage(3);
      break;
    case 3: /* print predictor list */
      fprintf( stderr, "\nPredictor list (index: predictor):\n");
      for ( i = 0; i < PREDICTORCOUNT; i++)
        fprintf( stderr, "%2d: %s\n", i, getPredictorName(i));
      break;
    case 4: /* information about -d */
      fprintf( stderr, "\n\
Information about the argument '-d<file>':                         \n\
The argument '-d' is optional and specifies the path to a context  \n\
data file. The context data has to be the same at prediction and   \n\
reconstruction.                                                    \n\
If the prediction is called with '-d' but without specifying an    \n\
output file with '-o', a context training will be performed. The   \n\
context data file in '-d' will be read and modified. If the file   \n\
at the path of '-d' does not exist, the program creates a new      \n\
context data file.                                                 \n\
By setting both '-d' and '-o', the context based prediction will   \n\
use the context data to improve the predictor selection.         \n");
      break;
  }
  exit( 0);
}

/*------------------------------------------------------------------
 * main( int argc, char *argv[])
 *
 * Main procedure. Processes the command line arguments and starts
 * the prediction, reconstruction or context data training.
 *-----------------------------------------------------------------*/
int main( int argc, char *argv[])
{
  char      *inputFile = NULL,    /* path to input file */
            *outputFile = NULL,   /* path to output file */
            *sInfoFile = NULL,    /* path to side information file */
            *dbFile = NULL,       /* path to context data file */
            *psImageFile = NULL;  /* predictor selection image */
  BOOL      rec;               /* reconstruction flag */
  BOOL      tabular;
  int       farbraum = 0;      /* color space (0: RGB) */
	UINT      i;
  PARAMETER *p = NULL;         /* parameter structure */
  int       exitCode = 0;

  TRY( newParameter( &p, PREDICTORCOUNT));

  /* get program name */
	progName = argv[0];

	/* set default values */
  rec = tabular = FALSE;
  
  /* show usage information if no arguments found */
	if (argc == 1) usage(0);
	
	/* loop through all arguments. stops at the last argument or at
   * the first invalid argument. */
	while ( (argc > 1) && (argv[1][0] == '-')) 
	{ 
    switch ( argv[1][1]) /* get option */
		{ 
      /* file paths */
			case 'i': /* input */
        inputFile = &argv[1][2];
        break;
			case 'o': /* output */
				outputFile = &argv[1][2];
        break;
			case 's': /* side information */
        sInfoFile = &argv[1][2];
        break;
      case 'd': /* context data */
        dbFile = &argv[1][2];
        break;
      case 'u': /* predictor selection image */
        psImageFile = &argv[1][2];
        break;
      /* prediction/reconstruction flag */
			case 'r': /* reconstruction */
				rec = TRUE;
				break;
			/* prediction parameters */
      case 'b': /* parameters related to the block module */
				switch ( argv[1][2]) /* read second character */
				{
					case 'm': /* enable block mode processing */
						p->blockMode = TRUE;
						break;
					case 'w': /* set block width */
            p->blockWidth = atoi( &argv[1][3]);
						break;
					case 'h': /* set block height */
            p->blockHeight = atoi( &argv[1][3]);
						break;
				}
				break;
      case 'c': /* parameters related to the context */
        switch ( argv[1][2]) /* read second character */
        {
          case 'p': /* #pixels for context modelling */
            p->cxPx = atoi( &argv[1][3]);
            break;
          case 'g': /* #gradients for context modelling */
            p->cxGrad = atoi( &argv[1][3]);
            break; 
          case 's': /* switch between predictors (no weighted means) */
            p->wMeanMode = FALSE;
            break;
        }
        break;
      case 'p': /* parameters related to predictors */
        switch ( argv[1][2]) /* read second character */
        {
          case 'p': /* enabled predictors */
            /* check for valid string length */
            if (strlen( &argv[1][3]) != p->anzPredictors)
            {
              fprintf( stderr, "Invalid length of the argument: %s\n",
                       argv[1]);
              usage(1);
            }
            /* enable/disable predictors */
            else
            {
              /* loop through string */
              for ( i = 0; i < strlen( &argv[1][3]); i++)
              {
                /* read character at position i */
                switch ( argv[1][3 + i])
                {
                  case '0':
                    p->predictors[i] = FALSE;
                    break;
                  case '1':
                    p->predictors[i] = TRUE;
                    break;
                  default:
                    fprintf( stderr, 
                     "Invalid character (%s) at: %s\n",
                     argv[1][3 + i], argv[1]);
                    usage(1);
                }
              }
            }
            break;
          case 'b': /* bias compensation */
            /* enable for all predictors */
            if (strlen( &argv[1][2]) == 1)
            {
              for ( i = 0; i < p->anzPredictors; i++)
                p->bias[i] = TRUE;
            }
            /* set adaption frequency */
            else if (argv[1][3] == 'r')
            {
              p->biasReset = atoi( &argv[1][4]);
            }
				    /* check for valid argument length */
            else if (strlen( &argv[1][3]) != p->anzPredictors)
            {
              fprintf( stderr, 
                        "Invalid length of the argument: %s\n",
                        argv[1]);
              usage(2);
            }
            /* enable/disable for specific predictors */
            else
            {
              /* loop through string */
              for ( i = 0; i < strlen( &argv[1][3]); i++)
              {
                /* read character at i */
                switch ( argv[1][3 + i])
                {
                  case '0':
                    p->bias[i] = FALSE;
                    break;
                  case '1':
                    p->bias[i] = TRUE;
                    break;
                  default:
                    fprintf( stderr, 
                             "Invalid character (%s) at: %s\n",
                             argv[1][3 + i], argv[1]);
                    usage(2);
                }
              }
            }
            break;  
          default:  /* show help */
            fprintf( stderr, "Invalid argument: %s\n", argv[1]);
            usage(0);
        }
        break;
      /* color space transformation */
      case 'y': 
      switch ( argv[1][2]) /* read second char */
      {
        case 'c': /* YCgCo */
          farbraum = 1;
          break;
        case 'r': /* YrUrVr */
          farbraum = 2;
          break;
      }
      break;
      /* short output */
      case 't':
        tabular = TRUE;
        break;
			/* help */
      case 'h': 
        switch ( argv[1][2]) /* read second character */
        {
          case 'p': /* information about -pp */
            usage(1);
            break;
          case 'b': /* information about -pb */
            usage(2);
            break;
          case 'd': /* information about -d */
            usage(4);
            break;
        }
        break;
      default:  /* print generic information */
        fprintf( stderr, "Invalid argument: %s\n", argv[1]);
        usage(0);
    }
    ++argv; // next argument in list
    --argc; // adjust argument list length
  }

  /* set default block dimensions */
  if (p->blockMode == TRUE)
  {
    if (p->blockWidth == 0)  p->blockWidth = 16;
    if (p->blockHeight == 0) p->blockHeight = 16;
  }
  
  /* start prediction/reconstruction */
  if (inputFile == NULL)
    fprintf( stderr, "\nInput file path is not set!\n");
  else if (outputFile == NULL)
  {
    /* context training */
    if (dbFile == NULL)
      fprintf( stderr, "\nOutput file path is not set!\n");
    else
      exitCode = startTraining( inputFile, dbFile);
  }
  else if (sInfoFile == NULL && p->blockMode == TRUE)
    fprintf( stderr, 
      "\nA side information file has to be specified for block-based prediction!\n");
  else
  {
    /* prediction/reconstruction */
    if (tabular == FALSE) printParameter( p);

    exitCode = start( inputFile, outputFile, sInfoFile, dbFile, p, 
                      rec, farbraum, tabular, psImageFile);
  }

  if (exitCode > 0)
    printf( "\nError occured:\n%s", 
            getErrorName( exitCode));

  return 0;
}