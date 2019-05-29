/*****************************************************
* Datei: externpred.c
* Autor: Thomas Schmiedel
* Datum: 02.11.2010
* Desc.: Durchführung der externen Prädiktion
* Vers.: 1.0
* 08.07.2011 Strutz copy of true max. values
 * 18.07.2011 Strutz: BILD ->range instead of -> maxValue
 * 05.10.2011 Strutz: blockInfo output in Q_DEBUG mode only
******************************************************/
//#define Q_DEBUG

/* Externe Prädiktion einbinden */
#pragma comment(lib, "Prediction.lib")

#include "externpred.h"
#include "bitbuf.h"
#include "ginterface.h"
#include "fibocode.h"
#include "glue.h"
// Strutz #include "externpred\errors.c"


/* Globale Parameterzwischenspeicher */
CustomParam *cparam = NULL;
SEITENINFO  **sinfo = NULL; 


/* Frühere externe Prädiktion durch */
void ExternalPred( PicData *pd, void *parameter)
{
	uint i, err = 0;
	ERRORDB *db = NULL;
	BILD *bild_blockInfo  = NULL;
	BILD *bild_in  = NULL,
		 *bild_out = NULL;
	ERRCODE = PRED_CRASHED;	

	/* Parameter importieren */
	cparam = (CustomParam *)parameter;		

	/* Wenn nötig, Contextdaten laden */
	if (cparam->usextfile) loadErrorDBFromFile(&db, cparam->ctxfile);
	
	/* Blockmodus initialisieren */
	if (cparam->predparam->blockMode == TRUE)
	{
		ALLOC( sinfo, pd->channel, SEITENINFO *);
		for (i = 0; i < pd->channel; i++)
		{
			newSeiteninfo( &sinfo[i], 1u);
		}
	}
	else
	{
		sinfo = NULL;
	}

	/* Strutz overwrite initial RESET value */
	cparam->predparam->biasReset = (pd->width + pd->height + 16) / 32 + 32;


	/* Prädiktion starten */
	for (i = 0; i < pd->channel; i++)
	{
		bild_in = ConvertPD2BILD( pd, i);		
		bild_blockInfo = ConvertPD2BILD( pd, i);	/* container for block selection info	*/	
		err = predict( &bild_out, bild_in, cparam->predparam, 
			cparam->predparam->blockMode == TRUE ? &sinfo[i] : NULL, &db, bild_blockInfo);
		if (err)
		{
			err = err;
		}
		/* range is set  in ConvertPD2BILD() and added as offset in predictionError()
		 * p->halfRange = p->range >> 1 is set in predictor.c, newPredictor()
		 */
		// Strutz for some reason this does not work together with BPC
		// pd->offset[i] = (pd->maxVal[i] + 1) >> 1;
		// reset offset
		pd->offset[i] = (pd->maxVal_rct[i] + 1) >> 1; 
		IncludeBILD( pd, i, bild_out);
		freeBild( bild_in);
		freeBild( bild_out);

#ifdef Q_DEBUG
		{
			uint x, y;
			long py, pos;
			FILE *out;
			char fname[512];

			sprintf( fname, "blockInfo_%d.pgm", i);
			out = fopen( fname, "wt");
			fprintf( out, "P2\n%d %d\n %d\n", pd->width, pd->height, cparam->predparam->anzPredictors);
			for (y = 0; y < pd->height; y++)
			{
				for ( x = 0; x <pd->width; x++)
				{
					fprintf( out, "%2d ", bild_blockInfo->wert[y][x]);
				}
				fprintf( out, "\n");
			}
			fclose( out);

			sprintf( fname, "blockInfoC_%d.txt", i);
			out = fopen( fname, "wt");
			for (y = 0; y < pd->height; y+=cparam->predparam->blockHeight)
			{
				for ( x = 0; x <pd->width; x+=cparam->predparam->blockWidth)
				{
					switch (bild_blockInfo->wert[y][x])
					{
						/* output is only correct, if all six predictors are enabled,
						 * otherwise the numbering is diferent
						 */
					case 0: fprintf( out, "MED "); break;	/* MED*/
					case 1: fprintf( out, "Pae "); break;	/* Paeth	*/
					case 2: fprintf( out, "Lin "); break;	/* linear	*/
					case 3: fprintf( out, "GAP "); break;	/* GAP	*/
					case 4: fprintf( out, "CoB "); break;	/*CoBalp	*/
					case 5: fprintf( out, "TM  "); break;	/* TM		*/
					}
				}
				fprintf( out, "\n");
			}
			fclose( out);
		}
#endif
		freeBild( bild_blockInfo);
	}		
	
	if (db != NULL) freeErrorDB(db);
	ERRCODE = 0;
}


/* Header der Prädiktion schreiben */
void WriteExtHeader( BitBuffer *bitbuf, uint channel)
{
	uint i, c;
	PARAMETER *param = cparam->predparam;	
	WriteFiboToStream( bitbuf, (uint)param->tmSigma);
	WriteFiboToStream( bitbuf, param->anzPredictors);
	WriteFiboToStream( bitbuf, param->biasReset);	
	WriteFiboToStream( bitbuf, param->wMeanMode);
	WriteFiboToStream( bitbuf, param->blockMode);
	WriteFiboToStream( bitbuf, cparam->usextfile ? 1u : 0);
	if (param->blockMode == TRUE)
	{
		WriteFiboToStream( bitbuf, param->blockWidth);
		WriteFiboToStream( bitbuf, param->blockHeight);
	}
	else
	{
		WriteFiboToStream(bitbuf, param->cxGrad);
		WriteFiboToStream(bitbuf, param->cxPx);
	}

	for (i = 0; i < param->anzPredictors; i++)
	{
		WriteFiboToStream( bitbuf, param->bias[i]);
		WriteFiboToStream( bitbuf, param->predictors[i]);
	}	

	/* Seiteninfo schreiben */
	if (param->blockMode == TRUE)
	{
		WriteFiboToStream( bitbuf, sinfo[0]->length);
		for (c = 0; c < channel; c++)
		{
			for (i = 0; i < sinfo[c]->length; i++)
			{				
				WriteFiboToStream( bitbuf, sinfo[c]->predictor[i]);
			}	
			freeSeiteninfo(sinfo[c]);
		}
		FREE(sinfo);
	}
}


/* Header lesen */
void ReadExtHeader(BitBuffer *bitbuf, uint channel, CustomParam *cparam)
{
	uint i, c, len;
	PARAMETER *param = NULL;
	// Strutz outside cparam = new CustomParam;

	newParameter( &param, 6u);
	cparam->predparam = param;
	cparam->ctxfile = NULL;

	param->tmSigma       = (float)GetFiboFromStream(bitbuf);
	param->anzPredictors = GetFiboFromStream(bitbuf);
	param->biasReset     = GetFiboFromStream(bitbuf);	
	param->wMeanMode	 = (BYTE)GetFiboFromStream(bitbuf);
	param->blockMode	 = (BYTE)GetFiboFromStream(bitbuf);
	cparam->usextfile    = GetFiboFromStream(bitbuf) ? true : false;
	if (param->blockMode == TRUE)
	{
		param->blockWidth  = GetFiboFromStream(bitbuf);
		param->blockHeight = GetFiboFromStream(bitbuf);
	}
	else
	{
		param->cxGrad = GetFiboFromStream(bitbuf);
		param->cxPx	  = GetFiboFromStream(bitbuf);
	}

	for (i = 0; i < param->anzPredictors; i++)
	{
		param->bias[i] = (BYTE)GetFiboFromStream(bitbuf);
		param->predictors[i] = (BYTE)GetFiboFromStream(bitbuf);
	}	

	/* Seiteninfo laden */
	if (param->blockMode == TRUE)
	{
		len = GetFiboFromStream( bitbuf);
		
		ALLOC(sinfo, channel, SEITENINFO *);
		for (c = 0; c < channel; c++)
		{
			newSeiteninfo(&sinfo[c], len);
			for (i = 0; i < sinfo[c]->length; i++)
			{
				sinfo[c]->predictor[i] = (BYTE)GetFiboFromStream(bitbuf);			
			}
		}
	}
	else
	{
		sinfo = NULL;
	}
}


/* Externe Prädiktion rückgängig machen */
void ExternalPredInverse(PicData *pd, void *parameter)
{
	uint i;
	ERRORDB *db = NULL;
	BILD *bild_in  = NULL,
		 *bild_out = NULL;
	
	//char *filename = (char *)parameter;		
	/* Parameter importieren */
	cparam = (CustomParam *)parameter;		

	if (cparam->usextfile && cparam->ctxfile == NULL)
	{
		ERRCODE = CTX_NOT_SET;
		throw 0;
	}
	else
	{		
		ERRCODE = PRED_CRASHED;	
	}

	if (cparam->usextfile) loadErrorDBFromFile( &db, cparam->ctxfile);

	for (i = 0; i < pd->channel; i++)
	{
		bild_in = ConvertPD2BILD( pd, i);		
		reconstruct( &bild_out, bild_in, cparam->predparam, cparam->predparam->blockMode == TRUE ? &sinfo[i] : NULL, &db);
		IncludeBILD( pd, i, bild_out);			
		freeBild( bild_in);
	}	
	
	/* Seiteninfo löschen */
	if (cparam->predparam->blockMode == TRUE)
	{		
		for (i = 0; i < pd->channel; i++)
		{		
			freeSeiteninfo( sinfo[i]);
		}
		FREE(sinfo);
	}

	if (db != NULL) freeErrorDB(db);		
	// extern freeParameter( cparam->predparam);
	// delete cparam;	
	ERRCODE = 0;
}


/* BILD in PicData importieren */
void IncludeBILD(PicData *pd, uint chan, BILD *bild)
{
	uint i, x, y;
	
	for (i = 0, y = 0; y < pd->height; y++)
	{
		for (x = 0; x < pd->width; x++)
		{
			pd->data[chan][i++] = bild->wert[y][x];
		}
	}
}

/*----------------------------------------------------------
 * *ConvertPD2BILD()
 * copy one component (channel) from a one-dimensional array
 * to a two-dimensional for external prediction
 *----------------------------------------------------------*/
BILD *ConvertPD2BILD( PicData *pd, uint chan)
{
	uint i, x, y;	
	BILD *bild = NULL;
	ALLOC( bild, 1u, BILD);

	bild->width    = pd->width;
	bild->height   = pd->height;
	bild->length   = pd->size;
	bild->bitDepth = (BYTE)pd->bitperchan[chan];
	bild->maxVal   = pd->maxVal_rct[chan];
	bild->range   = bild->maxVal + 1;
	/* Strutz: What happens, if we use the true maxVal+1? */
	// bild->range = (1u << pd->bitperchan[co]); /* - 1u; */
	
	ALLOC( bild->wert, pd->height, uint *);
	for (i = 0; i < pd->height; i++)
	{
		ALLOC( bild->wert[i], pd->width, uint);
	}

	for (i = 0, y = 0; y < pd->height; y++)
	{
		for (x = 0; x < pd->width; x++)
		{
			
			bild->wert[y][x] = pd->data[chan][i++];
		}
	}

	return bild;
}
