/*******************************************************************
 *
 * File....: prediction.c
 * Function: Predictor selection module. Contains functions for image
 *           prediction and reconstruction.
 * Author..: Thomas Schatton
 * Date....: 01/14/2011, 14.01.2011 (Strutz)
 * 18.07.2011 Strutz: BILD ->range instead of -> maxValue
 ********************************************************************/

#include "prediction.h"
#include "predictor.h"
//#define BLOCK_BY_BLOCK
#define ENTROPIE_CRITERION

/*------------------------------------------------------------------
 * predict( ...)
 *
 * Executes a prediction of all pixel values in the specified image
 * structure and writes the resulting prediction error image to
 * *ptrOut. The parameter structure defines the parameters used for
 * prediction. If block based mode is used, side information will be
 * created and written to *sInfo.
 *-----------------------------------------------------------------*/
int predict( BILD **ptrOut, /*pinter to image to be created */
						 BILD *img,			/* the input image	*/
						 PARAMETER *param, /* parameter structure	*/
            SEITENINFO **sInfo, /* structure for side information, block mode	*/
						ERRORDB **db,		/* context data from file	*/
						BILD *psImg			/* NULL	when called from ExternalPred() */
						)
{
  if (img != NULL)
	{
		PREDICTOR_S *pred = NULL; /* prediction module */
		AUSWAHL     *a = NULL;    /* selection module */
	  
    /* initialize structures */
		/* new structure for resulting image */
		TRY( newBild( ptrOut, img->width, img->height, img->bitDepth /*,
			img->maxValue*/));
		TRY( newAuswahl( &a, img, *ptrOut, param, sInfo));
		TRY( newPredictor( &pred, a, param, FALSE));
		
    /* choose prediction mode */
		if (a->blockMode == FALSE)
    {    
      /* context data is available */
      if (*db != NULL)
      {
        TRY( errorDbFitToContext( db, a->cx));
        a->db = *db;
      }

      /* context based predictor switching */
      if (param->wMeanMode == FALSE && a->db != NULL)
      {
        TRY( errorDbMode(a, pred, psImg));
      }
      /* context based prediction using weighted means */
      else
      {
        TRY( contextMode(a, pred));
      }
    }
    else
    {
		  TRY( blockMode( a, pred, FALSE, psImg)); /* block-based processing */
    }
    
    freePredictor( pred);
    freeAuswahl( a);
    
		return 0;
	}
  return 130;
}

/*------------------------------------------------------------------
 * reconstruct( ...)
 *
 * Reconstructs the original image from a prediction error image.
 * The parameter set has to be equal with the parameter set used for
 * image prediction. The specification of a side information file is
 * only necessary if prediction used block-based processing.
 *-----------------------------------------------------------------*/
int reconstruct( BILD **ptrOut, BILD *eImg, PARAMETER *param, 
                SEITENINFO **sInfo, ERRORDB **db)
{
  if (eImg != NULL)
	{
		PREDICTOR_S *pred = NULL; /* prediction module */
		AUSWAHL     *a = NULL;    /* selection module */
	
    /* initialize structures 
     * the reconstructed pixel values (output) will be written to the
     * input image structure as the prediction module requires them
     * for correct calculation of estimation values */
		TRY( newAuswahl( &a, eImg, eImg, param, sInfo));
    TRY( newPredictor( &pred, a, param, TRUE));

		if (a->blockMode == FALSE)
    {    
      if (*db != NULL)
      {
        TRY( errorDbFitToContext( db, a->cx));
        a->db = *db;
      }

      if (param->wMeanMode == FALSE && a->db != NULL)
      {
        TRY( errorDbMode(a, pred, NULL));
      }
      else
      {
        TRY( contextMode(a, pred));
      }
    }
    else							
    {
		  TRY( blockMode( a, pred, TRUE, NULL)); /* block-based processing */
    }

    freePredictor( pred);
    freeAuswahl( a);

    *ptrOut = eImg;
		return 0;
	}
  return 130;
}

/*------------------------------------------------------------------
 * selectionTraining( ...)
 *
 * Performs a context training on the image data in *img. The
 * gathered data will be added to the specified context data
 * structure.
 *-----------------------------------------------------------------*/
int selectionTraining( BILD *img, ERRORDB **ptrDb)
{
  if (img != NULL)
  {
    PREDICTOR_S *p = NULL;
    AUSWAHL     *a = NULL;
    PARAMETER   *para = NULL;
    BILD        *eImg = NULL;
    UINT        *pErrors = NULL,
                pNr, cNr, i;
    ERRORDB     *db;
    int         pIndex;

    TRY( ECALLOC( pErrors, PREDICTORCOUNT, UINT, 2));
    TRY( newParameter( &para, PREDICTORCOUNT));
    para->cxGrad = DB_CXGRAD;
    para->cxPx = DB_CXPX;

    TRY( newBild( &eImg, img->width, img->height, img->bitDepth /*,
			img->maxValue*/));
    TRY( newAuswahl( &a, img, eImg, para, NULL));
    TRY( newPredictor( &p, a, para, FALSE));

    freeParameter( para); 

    if (*ptrDb == NULL)
      TRY( newErrorDB( ptrDb, PREDICTORCOUNT, a->cx));

    db = *ptrDb;

    /* loop through all pixels */
    for ( a->r = 0; a->r < a->src->height; a->r++)
    {
      for ( a->c = 0; a->c < a->src->width; a->c++)
      {
        cNr = getKontext( a);

        /* try all predictors */
        for ( pNr = 0; pNr < PREDICTORCOUNT; pNr++)
        {
          /*
					pIndex = predictorIsActive( p, pNr);
          if (pIndex >= 0)
          {
            writePredictionError( p, pIndex, FALSE);
            db->sse[pNr][cNr] += powf( (float)((int)a->tar->wert[a->r][a->c] - p->halfRange), 2);
          }
					*/
					/* Strutz: check whether predictor can be used	*/
				  for ( i = 0; i < p->activePredictors && p->assoc[i] != pNr; i++);
					pIndex = i >= p->activePredictors ? -1 : (int)i;
          if (pIndex >= 0)
          {
            writePredictionError( p, pIndex, FALSE);
            db->sse[pNr][cNr] += powf( (float)((int)a->tar->wert[a->r][a->c] - p->halfRange), 2);
          }
        }

        db->values[cNr]++;
        
        /* periodically halve the values to prevent overflows */
        if (db->values[cNr] >= db->reset)
        {
          for ( i = 0; i < db->cx->anzKontexte;
                db->values[i++] >>= 1);
          
          for ( pNr = 0; pNr < PREDICTORCOUNT; pNr++)
            db->sse[pNr][cNr] /= 2;
        }
      }
    }

    return 0;
  }
  return 130;
}

/*------------------------------------------------------------------
 * errorDbMode( ...)
 *
 * Prediction mode which uses context data to switch between optimal
 * predictors.
 *-----------------------------------------------------------------*/
int errorDbMode( AUSWAHL *a, PREDICTOR_S *pred, BILD *psImg)
{
  UINT    cxNr,            /* current context */
          i,
          *ranking;
  int     pNr = 0;        /* predictor index */

  /* alloc */
  TRY( ECALLOC( ranking, PREDICTORCOUNT, UINT, 131));
  
	for ( a->r = 0; a->r < a->src->height; a->r++)  /* all rows */
  {
    for ( a->c = 0; a->c < a->src->width; a->c++) /* all columns */
    {
      cxNr = getKontext( a); /* context modelling */

      /* determine best predictor */
      TRY( getPredRanking( &ranking, a->db, cxNr));

      for ( i = 0; i < PREDICTORCOUNT; i++)
      {
        /*
				pNr = predictorIsActive( pred, ranking[i]);
        if (pNr >= 0) break;
				*/
				/* Strutz: inline calculation	*/
				unsigned int ip;
			  for ( ip = 0; ip < pred->activePredictors && pred->assoc[ip] != ranking[i]; ip++);
				pNr = (ip >= pred->activePredictors) ? -1 : (int)ip;
        if (pNr >= 0) break;
      }

      if (pNr < 0) return 132; /* all predictors disabled */

      /* log selection */
      if (psImg != NULL)
      {
        psImg->wert[a->r][a->c] = 
          (psImg->range / PREDICTORCOUNT) * pNr; 
      }

      writePredictionError( pred, (UINT)pNr, TRUE);
    }
  }

  /* free */
  FREEPTR( ranking);

  return 0;
}

/*------------------------------------------------------------------
 * contextMode( AUSWAHL *a, PREDICTOR_S *pred)
 *
 * Prediction using a weighted mean of the prediction values of all
 * enabled predictors. The weights depend on the current context.
 *-----------------------------------------------------------------*/
int contextMode( AUSWAHL *a, PREDICTOR_S *pred)
{
  KONTEXT *cx = a->cx;     /* pointer to context structure */
  UINT    cxNr,            /* current context */
          pNr;             /* predictor index */
  /* mean value calculation */
  float   *w;              /* predictor weights */
  UINT    *x;              /* estimation values (per predictor) */
  int     e;               /* prediction error */
  /* weight adaption */
  float   **sse;           /* SSE per predictor per context */
  ULONG   *N;              /* activity per context */
  UINT    reset = 64;           
  
  TRY( ECALLOC( N, cx->anzKontexte, ULONG, 131));
  
  /* allocate and initialize SSE-array */
  TRY( ECALLOC( sse, pred->activePredictors, float*, 131));
  for ( pNr = 0; pNr < pred->activePredictors; pNr++)
  {
    TRY( ECALLOC( sse[pNr], cx->anzKontexte, float, 131));
  }
/* original   
  for ( pNr = 0; pNr < pred->activePredictors; pNr++)
    for ( cxNr = 0; cxNr < cx->anzKontexte; cxNr++)
      sse[pNr][cxNr] = 1;
*/
  for ( pNr = 0; pNr < pred->activePredictors; pNr++)
	{
    for ( cxNr = 0; cxNr < cx->anzKontexte; cxNr++)
		{
      sse[pNr][cxNr] = 1;
//Strutz
//			if (pred->assoc[pNr] == 0) sse[pNr][cxNr] = 1;  /* MED */
//			else sse[pNr][cxNr] = 100;  /* suppress others in the beginning */
		}
	}
  
  /* allocate w, x */
  TRY( ECALLOC( w, pred->activePredictors, float, 131));
  TRY( ECALLOC( x, pred->activePredictors, UINT, 131));
  
  /* process context data if provided */
  if (a->db != NULL)
  {
    for ( cxNr = 0; cxNr < cx->anzKontexte; cxNr++)
    {
      N[cxNr] = a->db->values[cxNr];
      
      for ( pNr = 0; pNr < pred->activePredictors; pNr++)
      {
        sse[pNr][cxNr] = a->db->sse[pred->assoc[pNr]][cxNr];
        if (sse[pNr][cxNr] == 0)
          sse[pNr][cxNr] = 1;
      }
    }
  }

	for ( a->r = 0; a->r < a->src->height; a->r++)  /* all rows */
  {
		for ( a->c = 0; a->c < a->src->width; a->c++) /* all columns */
    {
      cxNr = getKontext( a); /* context modelling for predictor combination*/

      /* compute predictor weights */
      for ( pNr = 0; pNr < pred->activePredictors; pNr++)
        w[pNr] = 1.0f / sse[pNr][cxNr];

			writePredictionErrorW( pred, w, &x, TRUE);

      /* update SAD and increase N */
      for ( pNr = 0; pNr < pred->activePredictors; pNr++)
      {
        e = a->src->wert[a->r][a->c] - x[pNr];  /* prediction error */
          
        sse[pNr][cxNr] += (ULONG)(e * e); 
        /* domain adjustment */
	      if      (e >= pred->halfRange) e -= pred->range;
        else if (e < -pred->halfRange) e += pred->range;

// Strutz: summation of SSE should be before clipping, shouldn't it?
//        sse[pNr][cxNr] += (ULONG)(e * e);
      }
      
      N[cxNr]++;

      /* prevent overflows */
      if (N[cxNr] == reset)
      {
        N[cxNr] = 0;
        for ( pNr = 0; pNr < pred->activePredictors; pNr++)
          sse[pNr][cxNr] /= 2;
      }
    }
  }

  /* free */
  FREEPTR( N);
  for ( pNr = 0; pNr < pred->activePredictors; pNr++)
    FREEPTR( sse[pNr]);
  FREEPTR( sse);
  FREEPTR( x);
  FREEPTR( w);
 
  return 0;
}

#ifdef BLOCK_BY_BLOCK
/*------------------------------------------------------------------
 * blockMode( AUSWAHL *a, PREDICTOR_S *pred, BOOL readSInfo)
 *
 * Performs a prediction using a block-based predictor selection.
 * This mode produces side information that is required for a proper
 * image reconstruction.
 *-----------------------------------------------------------------*/
int blockMode( AUSWAHL *a, PREDICTOR_S *pred, BOOL readSInfo,
               BILD *psImg)
{
 	UINT  bNr,               /* block index */
        rOffset, cOffset,  /* position of first pixel in a block */
			  pNr;               /* predictor index */
	float H;                 /* entropy */
	BLOCK *bk = a->bk;       /* pointer to block structure */
  BILD  **blockImg = NULL; /* array of image structures */
        
  /* encoding */
  if (readSInfo == FALSE)
  {
    /* initialize side information */
    freeSeiteninfo( *(a->sInfo));
		TRY( newSeiteninfo( (a->sInfo), a->bk->count));

    /* allocate image structures used to store prediction values
     * 0: original block, 1..N: block for predictor #(i-1) 
     */
    TRY( ECALLOC( blockImg, pred->activePredictors, BILD*, 131));
    for ( pNr = 0; pNr < pred->activePredictors; pNr++)
    {
      TRY( newBild( &(blockImg[pNr]), bk->width, bk->height, a->src->bitDepth /*,
				a->src->maxValue*/));	/*Strutz: */
    }

    for ( bNr = 0; bNr < bk->count; bNr++) /* all blocks */
	  { 
      /* get position of first pixel */
      rOffset = startRow( bk, bNr);
		  cOffset = startCol( bk, bNr);
          
      bk->minEntropie = 1e10; /* reset minimal entropy */
      bk->bestPredictor = 0;
      
      /* try all enabled predictors */
      for ( pNr = 0; pNr < pred->activePredictors; pNr++) 
      { 
        /* all pixels in the block */
        for ( a->r = rOffset; a->r < rOffset + bk->height 
                                    && a->r < a->src->height; a->r++)
        {
  	      for ( a->c = cOffset; a->c < cOffset + bk->width 
                                    && a->c < a->src->width; a->c++)
          {
            /* calculate and write prediction error */
				    writePredictionError( pred, pNr, TRUE);

            /* store prediction errors */
            (blockImg[pNr])->wert[a->r - rOffset][a->c - cOffset] = 
                                  a->tar->wert[a->r][a->c];
          }
        }
       
        /* evaluate the result (compare entropies) */
	      calcHistogramm( bk, bNr);
        H = blockEntropie( bk);
        
        /* save best result */
	      if (H < bk->minEntropie)
	      {
		      bk->minEntropie = H;
		      bk->bestPredictor = pNr;
	      }
      }
      (*a->sInfo)->predictor[bNr] = (BYTE)bk->bestPredictor;
    
      /* apply best predictor */
	    for ( a->r = rOffset; a->r < rOffset + bk->height 
                                && a->r < a->src->height; a->r++)
      {
		    for ( a->c = cOffset; a->c < cOffset + bk->width 
                                  && a->c < a->src->width; a->c++)
        {
          /* log predictor selection */
          if (psImg != NULL)
          {
            psImg->wert[a->r][a->c] = 
              (psImg->range / PREDICTORCOUNT) * (*a->sInfo)->predictor[bNr];
          }

          /* copy prediction values to the output image */
          a->tar->wert[a->r][a->c] = 
            blockImg[bk->bestPredictor]->wert[a->r - rOffset][a->c - cOffset];
        }
      }

      /* update histogram */
	    calcHistogramm( bk, bNr);
      mergeHistograms( bk);
    }

    for ( pNr = 0; pNr < pred->activePredictors; freeBild( blockImg[pNr++]));
    FREEPTR( blockImg);
  }
  /* decoding */
  else
  {
    for ( bNr = 0; bNr < bk->count; bNr++) /* all blocks */
	  { 
      /* get position of first pixel */
      rOffset = startRow( bk, bNr);
		  cOffset = startCol( bk, bNr);
          
      /* apply optimal predictor first to restore original neighborhood */
      for ( a->r = rOffset; a->r < rOffset + bk->height 
                                && a->r < a->src->height; a->r++)
      {
		    for ( a->c = cOffset; a->c < cOffset + bk->width 
                                  && a->c < a->src->width; a->c++)
        {
          /* log predictor selection */
          if (psImg != NULL)
          {
 /* 
						psImg->wert[a->r][a->c] = 
              (psImg->range / PREDICTORCOUNT) * (*a->sInfo)->predictor[bNr];
	*/
            psImg->wert[a->r][a->c] = (*a->sInfo)->predictor[bNr];
         }

          /* calculate and write prediction error */
				  writePredictionError( pred, (*a->sInfo)->predictor[bNr], TRUE);
        }
      }

      /* apply other predictors for adaption */
      for ( pNr = 0; pNr < pred->activePredictors; pNr++) 
      { 
        if (pNr != (*a->sInfo)->predictor[bNr])
        {
          /* all pixels in the block */
          for ( a->r = rOffset; a->r < rOffset + bk->height 
                                      && a->r < a->src->height; a->r++)
            for ( a->c = cOffset; a->c < cOffset + bk->width 
                                      && a->c < a->src->width; a->c++)
              /* predictor and bias adaption */
              restorePredictionErrorB( pred, pNr);
        } 
      } 
    } 
	}

  return 0;
}

#else

/*------------------------------------------------------------------
 * blockMode( AUSWAHL *a, PREDICTOR_S *pred, BOOL readSInfo)
 *
 * alternative processing (raster scan -> no missing pixels at block
 * borders)
 *-----------------------------------------------------------------*/
int blockMode( AUSWAHL *a, PREDICTOR_S *pred, BOOL readSInfo,
               BILD *psImg)
{
 	UINT  bNr,							/* block index */
        rOffset, cOffset, /* position of first pixel in a block */
			  pNr,              /* predictor index */
        pCur;             /* predictor for current position (for decoding) */
	float H;                /* entropy */
	unsigned long sse;			/* sum of squared errors	*/
	BLOCK *bk = a->bk;      /* pointer to block structure */
  BILD  **eImg = NULL;    /* array of prediction error images */
  BLOCK **eBk = NULL;     /* array of block modules (for each *eImg) */
  PARAMETER *p = NULL;

  TRY( newParameter(&p, PREDICTORCOUNT));
  p->blockHeight = bk->height;
  p->blockWidth = bk->width;
  p->selCriterion = bk->selCriterion;
  
  /* allocate image structures used to store prediction errors */
  TRY( ECALLOC( eImg, pred->activePredictors, BILD*, 131));
  TRY( ECALLOC( eBk, pred->activePredictors, BLOCK*, 131));
  for ( pNr = 0; pNr < pred->activePredictors; pNr++)
  {
    TRY( newBild( &(eImg[pNr]), a->src->width, a->src->height, a->src->bitDepth));
    TRY( newBlock( &(eBk[pNr]), eImg[pNr], p));
    (eBk[pNr])->hBlock[0] = bk->hBlock[0];
  }
  freeParameter(p);

  /* encoding */
  if (readSInfo == FALSE)
  {
    /* step 1: apply all enabled predictors, 
		 * create X images containing prediction errors
		 */
    for ( pNr = 0; pNr < pred->activePredictors; pNr++) 
    { 
			fprintf( stderr, " %d", pNr);
      /* write prediction error images for each predictor */
      for ( a->r = 0; a->r < a->src->height; a->r++)
      {
        for ( a->c = 0; a->c < a->src->width; a->c++)
        {
          writePredictionError( pred, pNr, TRUE);
          /* store prediction errors */
          (eImg[pNr])->wert[a->r][a->c] = a->tar->wert[a->r][a->c];
        }
      }
    }

    /* initialize side information */
    freeSeiteninfo( *(a->sInfo));
		TRY( newSeiteninfo( (a->sInfo), a->bk->count));

    /* determine best predictor for each block	*/
#ifdef ENTROPIE_CRITERION
		for ( bNr = 0; bNr < bk->count; bNr++) /* all blocks */
	  { 
      /* get position of first pixel */
      rOffset = startRow( bk, bNr);
		  cOffset = startCol( bk, bNr);
        
      bk->minEntropie = 1e10; /* reset minimal entropy */
      bk->bestPredictor = 0;
 			bk->minSSE = bk->height * bk->width * 1024 * 1024; /* reset minimal SSE */
       
      for ( pNr = 0; pNr < pred->activePredictors; pNr++)
      {
				/* evaluate the result (compare entropies) */
        calcHistogramm( eBk[pNr], bNr);
        H = blockEntropie( eBk[pNr]);
        if (bNr == 0)
				{
					/* first block: avoid skewed distribution	*/
					sse = calcBlockSSE( eBk[pNr], bNr, a->src->range);
					if (sse < bk->minSSE)
					{
						bk->minSSE = sse;
						bk->bestPredictor = pNr;
					}
				}
				else
				{
					/* save best result */
					if (H < bk->minEntropie)
					{
						bk->minEntropie = H;
						bk->bestPredictor = pNr;
					}
				}
      }
      (*a->sInfo)->predictor[bNr] = (BYTE)bk->bestPredictor;
    
      /* apply best predictor */
	    for ( a->r = rOffset; a->r < rOffset + bk->height 
                                && a->r < a->src->height; a->r++)
      {
		    for ( a->c = cOffset; a->c < cOffset + bk->width 
                                  && a->c < a->src->width; a->c++)
        {
          /* log predictor selection */
          if (psImg != NULL)
          {
/* 
						psImg->wert[a->r][a->c] = 
              (psImg->range / PREDICTORCOUNT) * (*a->sInfo)->predictor[bNr];
	*/
            psImg->wert[a->r][a->c] = (*a->sInfo)->predictor[bNr];
          }

          /* copy prediction values to the output image */
          a->tar->wert[a->r][a->c] = eImg[bk->bestPredictor]->wert[a->r][a->c];
        }
      }

      /* update histogram */
	    calcHistogramm( bk, bNr);
      mergeHistograms( bk);
    }
#else

		for ( bNr = 0; bNr < bk->count; bNr++) /* all blocks */
	  { 
      /* get position of first pixel */
      rOffset = startRow( bk, bNr);
		  cOffset = startCol( bk, bNr);
        
			bk->minSSE = bk->height * bk->width * 1024 * 1024; /* reset minimal SSE */
      bk->bestPredictor = 0;
        
      for ( pNr = 0; pNr < pred->activePredictors; pNr++)
      {
        /* evaluate the result (compare entropies) */
				sse = calcBlockSSE( eBk[pNr], bNr, a->src->range);
#ifdef PENALTY        
				/* penalty if block number is different than previous */
				if (bNr > 0 && pNr != (*a->sInfo)->predictor[bNr-1])
				{
					/* save best result */
					if (sse < bk->minSSE * 0.95)
					{
						bk->minSSE = sse;
						bk->bestPredictor = pNr;
					}
				}
				else
#endif
				{
					/* save best result */
					if (sse < bk->minSSE)
					{
						bk->minSSE = sse;
						bk->bestPredictor = pNr;
					}
				}
      }
      (*a->sInfo)->predictor[bNr] = (BYTE)bk->bestPredictor;
    
      /* apply best predictor */
	    for ( a->r = rOffset; a->r < rOffset + bk->height 
                                && a->r < a->src->height; a->r++)
      {
		    for ( a->c = cOffset; a->c < cOffset + bk->width 
                                  && a->c < a->src->width; a->c++)
        {
          /* log predictor selection */
          if (psImg != NULL)
          {
            psImg->wert[a->r][a->c] = (*a->sInfo)->predictor[bNr];
/* scaled to the range
						psImg->wert[a->r][a->c] = 
              (psImg->range / PREDICTORCOUNT) * (*a->sInfo)->predictor[bNr];
*/
          }

          /* copy prediction values to the output image */
          a->tar->wert[a->r][a->c] = eImg[bk->bestPredictor]->wert[a->r][a->c];
        }
      }
    }
#endif
  }
  /* decoding */
  else
  {
    for ( a->r = 0; a->r < a->src->height; a->r++)
    {
      for ( a->c = 0; a->c < a->src->width; a->c++)
      {
        pCur = (*a->sInfo)->predictor[(a->r / bk->height) * bk->countX + (a->c / bk->width)];
        
        /* calculate and write prediction error */
			  writePredictionError( pred, pCur, TRUE);

        /* apply other predictors for adaption */
        for ( pNr = 0; pNr < pred->activePredictors; pNr++) 
        { 
          /* predictor and bias adaption */
          if (pNr != pCur)
            restorePredictionErrorB( pred, pNr);
        }
      }
    }
	}

  for ( pNr = 0; pNr < pred->activePredictors; pNr++)
  {
    freeBild( eImg[pNr]);
    /* each block uses a shared array hBlock[0] from *(a->bk). freeBlock() will lead to
     * a heap error when trying to free an unassigned pointer. 
     */
    // freeBlock( eBk[pNr]);
		FREEPTR( eBk[pNr]);  // Strutz
  }
  FREEPTR( eImg);
  FREEPTR( eBk);

  return 0;
}
#endif //BLOCK_BY_BLOCK