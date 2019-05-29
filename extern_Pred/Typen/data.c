/*******************************************************************
 *
 * File....: data.c
 * Function: Function for manipulating a DATA structure.
 * Author..: Thomas Schatton, T.Strutz
 * Date....: 11/02/2010, 8.3.2011 (use individual bitperchan[])
 * 18.07.2011 Strutz: BILD ->range instead of -> maxValue
 ********************************************************************/
#include "data.h"

#ifdef PREDICTION_ALONE
/*------------------------------------------------------------------
 * writeData( DATA *data, char *file)
 *
 * Writes the image data to a file at the specified path.
 * Additionally, a side information file will be written if the data
 * structure contains side information. The path to the side
 * information file will be read from data->sInfoFile.
 *-----------------------------------------------------------------*/
int writeData( DATA *data, char *file)
{
	PicData *pd = NULL,
          *pdPS = NULL; /* predictor selection image */
  UINT    k, r, c;

  TRY( dataToPicData( &pd, data));
    
  /* write image data */
	WritePPM( pd, file);
  
  /* write side information if existent */
  if (data->sInfo[0]->length > 0)
		TRY( writeSeiteninfosToFile( data->sInfo, 
            data->sInfoFile, data->channelCnt));

  if (data->psImgFile != NULL && data->psImg != NULL)
  {
		unsigned int tmp[3]={ 8u, 8u, 8u};
    /* create PicData structure */
    pdPS = CreatePicData( data->psImg[0]->width, 
                          data->psImg[0]->height,
                          data->channelCnt, 
                          data->psImg[0]->bitDepth,
													tmp);
   
    if (pdPS == NULL) return 44;

    /* copy pixel values from p. sel. image to PicData */
    for ( k = 0; k < data->channelCnt; k++)    /* all channels */
      for ( r = 0; r < pdPS->height; r++)     /* all rows */
        for ( c = 0; c < pdPS->width; c++)    /* all columns */
          pdPS->data[k][c + r * pdPS->width] = data->psImg[k]->wert[r][c];
    
    /* write to file */
    WritePPM( pdPS, data->psImgFile);

    /* free */
    DeletePicData( pdPS);
  }

  DeletePicData( pd);

  return 0;
}


/*------------------------------------------------------------------
 * loadData( ...)
 *
 * Loads image data from a PGM or PPM file and puts it into a new
 * data structure. This function does not read the side information
 * file but saves the path in data->sInfoFile.
 *-----------------------------------------------------------------*/
int loadData( DATA **ptrD, char *ppmFile, char *sInfoFile)
{
  PicData *pd;
  DATA    *d;

	pd = LoadPPM( ppmFile); /* read image data */
  TRY( picDataToData( &d, pd));
  
  if (sInfoFile != NULL)
  {
    d->sInfoFile = sInfoFile;
    //TRY( ECALLOC( d->sInfoFile, strlen( sInfoFile), char, 43));
	  //strcpy( d->sInfoFile, sInfoFile);
  }
  
  DeletePicData( pd);

  *ptrD = d;
	return 0;
}
#endif

/*------------------------------------------------------------------
 * dataTransform(DATA *d, int toFarbraum)
 *
 * Applies a color space transformation to a data structure. The
 * data structure has to contain three channels of image data.
 *-----------------------------------------------------------------*/

/*------------------------------------------------------------------
 * commented out since version 1.15 
 * Reason: CTs are no longer handled in the prediction project.
 * Also creates cross dependencies since both projects need each other
 *-----------------------------------------------------------------*/

//int dataTransform(DATA *d, int toFarbraum)
//{
//  if (d != NULL)
//  {
//    if (d->channelCnt != 3)
//      return 0;
//
//    if (d->farbraum != toFarbraum) /* transformation necessary */
//    {
//      UINT    k, r, c;
//      PicData *pd = NULL;
//      
//      TRY( dataToPicData( &pd, d));
//
//      if      (d->farbraum == RGB)     /* RGB --> YCgCo / YrUrVr */
//      {
//        if      (toFarbraum == YCgCo)  TransformRGB2YCgCo( pd);
//        else if (toFarbraum == YrUrVr) 
//        {
//          TransformRGB2YrUrVr( pd);
//          d->img[1]->bitDepth++;
//          d->img[2]->bitDepth++;
//          d->img[1]->range <<= 1;
//          d->img[2]->range <<= 1;
//        }
//      }
//      else if (d->farbraum == YCgCo)   /* YCgCo --> RGB / YrUrVr */
//      {
//        if      (toFarbraum == RGB)   
//        {
//          TransformYCgCo2RGB( pd);
//        }
//        else if (toFarbraum == YrUrVr) 
//        {
//          TransformYCgCo2RGB( pd);
//          TransformRGB2YrUrVr( pd);
//          d->img[1]->bitDepth++;
//          d->img[2]->bitDepth++;
//          d->img[1]->range <<= 1;
//          d->img[2]->range <<= 1;
//        }
//      }
//      else if (d->farbraum == YrUrVr)  /* YrUrVr --> RGB / YCgCo */
//      {
//        if      (toFarbraum == RGB)    
//        {
//          TransformYrUrVr2RGB( pd);  
//        }
//        else if (toFarbraum == YCgCo) 
//        {
//          TransformYrUrVr2RGB( pd);
//          TransformRGB2YCgCo( pd);
//        }
//
//        d->img[1]->bitDepth--;
//        d->img[2]->bitDepth--;
//        d->img[1]->range >>= 1;
//        d->img[2]->range >>= 1;
//      }
//      
//      d->farbraum = toFarbraum;
//
//      /* copy pixel values to image structure */  
//      for (k = 0; k < d->channelCnt; k++)   /* all channels */
//        for ( r = 0; r < pd->height; r++)   /* all rows */
//			    for ( c = 0; c < pd->width; c++)  /* all columns */
//				    d->img[k]->wert[r][c] = pd->data[k][c + r * pd->width];
//      
//      DeletePicData( pd);
//	  }
//    return 0;
//  }
//  return 45; /* d is NULL */
//}

/*------------------------------------------------------------------
 * newData( ...)
 *
 * Constructor: Allocates and initializes a new data structure.
 *-----------------------------------------------------------------*/
int newData( DATA **ptrD, UINT channelCnt, char *sInfoFile, int farbraum,
             char *psImageFile)
{
	UINT  i;
  DATA *d;

  TRY( ECALLOC( *ptrD, 1, DATA, 40));
  d = *ptrD;
  /* initialization */
  d->channelCnt = channelCnt;
  d->farbraum = farbraum;
  d->sInfoFile = NULL;
  d->psImgFile = NULL;
 
  TRY( ECALLOC( d->img,   channelCnt, BILD*, 41));
  TRY( ECALLOC( d->sInfo, channelCnt, SEITENINFO*, 42));
  TRY( ECALLOC( d->psImg, channelCnt, BILD*, 41));
     
  for (i = 0; i < channelCnt; i++)
  {
    TRY( newSeiteninfo( &(d->sInfo[i]), 0)); /* empty side information */
    d->img[i] = NULL;
    d->psImg[i] = NULL;
  }

  if (sInfoFile != NULL)
  {
    d->sInfoFile = sInfoFile;
  }

  if (psImageFile != NULL)
  {
    d->psImgFile = psImageFile;
  }

	return 0;
}

/*------------------------------------------------------------------
 * dataToPicData(PicData **ptrPd, DATA *d)
 *
 * Converts the image data from a data structure to a PicData
 * structure for further processing in ginterface.c.
 *-----------------------------------------------------------------*/
int dataToPicData(PicData **ptrPd, DATA *d)
{
  UINT    k, r, c; /* channel, row, column */
  PicData *pd;

  /* create PicData structure */
  if (d->channelCnt == 3)
  {
 		unsigned int tmp[4]={ 8u, 8u, 8u, 0u};
   (*ptrPd) = CreatePicData( d->img[0]->width, d->img[0]->height,
                      d->channelCnt, d->img[1]->bitDepth, tmp);
  }
  else
  { 
		unsigned int tmp[4]={ 8u, 0u, 0u, 0u};
    (*ptrPd) = CreatePicData( d->img[0]->width, d->img[0]->height,
                      d->channelCnt, d->img[0]->bitDepth, tmp);
  }
  
  pd = *ptrPd;

  if (pd == NULL) return 44;

  /* copy pixel values from data structure to PicData */
  for ( k = 0; k < d->channelCnt; k++)    /* all channels */
  {
    for ( r = 0; r < pd->height; r++)     /* all rows */
      for ( c = 0; c < pd->width; c++)    /* all columns */
        pd->data[k][c + r * pd->width] = d->img[k]->wert[r][c];
  }

  return 0;
}

/*------------------------------------------------------------------
 * picDataToData(DATA *d, PicData *pd)
 *
 * Converts a PicData structure to a data structure.
 *-----------------------------------------------------------------*/
int picDataToData(DATA **ptrD, PicData *pd)
{
  UINT k, r, c;     /* channel, row, column */
  DATA *d = NULL;


  /* create data structure */
  TRY( newData( &d, pd->channel, NULL, RGB, NULL));
  
  /* create an image structure for each channel */
  for (k = 0; k < d->channelCnt; k++)
  {
    //TRY( newBild( &(d->img[k]), pd->width, pd->height, pd->bitperchannel));
    TRY( newBild( &(d->img[k]), pd->width, pd->height, (BYTE)pd->bitperchan[k] /*,
					pd->maxVal[k]*/));
    
    /* copy pixel values */
    for ( r = 0; r < pd->height; r++)   /* all rows */
		  for ( c = 0; c < pd->width; c++)  /* all columns */
			  d->img[k]->wert[r][c] = pd->data[k][c + r * pd->width];
  }

  *ptrD = d;
  return 0;
}

/*------------------------------------------------------------------
 * freeData( DATA *data)
 *
 * Destructor: Frees a data structure.
 *-----------------------------------------------------------------*/
void freeData( DATA *data)
{
	UINT i;

  /* free image and side information structures */
	for ( i = 0; i < data->channelCnt; i++)
	{
    if (data->psImg != NULL)
      freeBild( data->psImg[i]);
    freeBild( data->img[i]);
		freeSeiteninfo( data->sInfo[i]);
	}
  /* free pointers */
  if (data->psImg != NULL) 
    FREEPTR( data->psImg);
  if (data->psImgFile != NULL) 
    FREEPTR( data->psImgFile);
	FREEPTR( data->img);
	FREEPTR( data->sInfo);
	FREEPTR( data->sInfoFile);
  /* free structure */
  FREEPTR( data);
}