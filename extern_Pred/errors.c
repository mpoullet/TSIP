/*******************************************************************
 *
 * File....: errors.c
 * Function: Error messages.
 * Author..: Thomas Schatton
 * Date....: 11/05/2010
 *
 ********************************************************************/

#include <string.h>

static char *getErrorBild( int code)
{
  switch ( code)
  {
    case 0: return "Could not malloc BILD struct.";
    case 1: return "Could not malloc BILD row array.";
    case 2: return "Could not malloc BILD column array.";
    case 3: return "Could not malloc histogram array.";
    default:return "Unknown error.";
  }
}

static char *getErrorParameter( int code)
{
  switch ( code)
  {
    case 0: return "Could not malloc PARAMETER struct.";
    case 1: return "Could not malloc PARAMETER predictors array.";
    case 2: return "Could not malloc PARAMETER bias array.";
    case 3: return "Could not malloc string array.";
    case 4: return "Could not malloc stream array.";
    default:return "Unknown error.";
  }
}

static char *getErrorSeiteninfo( int code)
{
  switch ( code)
  {
    case 0: return "Could not malloc SEITENINFO struct.";
    case 1: return "Could not malloc SEITENINFO predictors array.";
    case 2: return "Could not malloc stream array.";
    case 3: return "Error while reading file.";
    case 4: return "Could not malloc SEITENINFO array.";
    case 5: return "Error allocating array fo SEITENINFO entropy calculation.";
    default:return "Unknown error.";
  }
}

static char *getErrorData( int code)
{
  switch ( code)
  {
    case 0: return "Could not malloc DATA struct.";
    case 1: return "Could not malloc BILD array.";
    case 2: return "Could not malloc SEITENINFO array.";
    case 3: return "Could not malloc string.";
    case 4: return "Could not malloc PicData struct.";
    case 5: return "DATA struct pointer is NULL.";
    default:return "Unknown error.";
  }
}

static char *getErrorPredictor( int code)
{
  switch ( code)
  {
    case 0: return "Could not malloc PREDICTOR_S struct.";
    case 1: return "Could not malloc PREDICTOR_S associations array.";
    case 2: return "Could not malloc PREDICTOR_S toggle bias array.";
    case 3: return "Could not malloc PREDICTOR_S bias array.";
    case 4: return "Could not malloc PREDICTOR_S toggle predictors array.";
    default:return "Unknown error.";
  }
}

static char *getErrorContext( int code)
{
  switch ( code)
  {
    case 0: return "Could not malloc KONTEXT struct.";
    case 1: return "Context space too large. Use smaller context parameters.";
    default:return "Unknown error.";
  }
}

static char *getErrorBlock( int code)
{
  switch ( code)
  {
    case 0: return "Could not malloc BLOCK struct.";
    case 1: return "Could not malloc histogram array.";
    default:return "Unknown error.";
  }
}

static char *getErrorBias( int code)
{
  switch ( code)
  {
    case 0: return "Could not malloc BIAS struct.";
    case 1: return "Could not malloc a long array.";
    default:return "Unknown error.";
  }
}

static char *getErrorAuswahl( int code)
{
  switch ( code)
  {
    case 0: return "Could not malloc AUSWAHL struct.";
    default:return "Unknown error.";
  }
}

static char *getErrorMatrix( int code)
{
  switch ( code)
  {
    case 0: return "Could not malloc MATRIX struct.";
    case 1: return "Could not malloc value matrix.";
    case 2: return "MATRIX struct pointer is NULL.";
    case 3: return "Matrix Inversion requires a quadratical matrix.";
    case 4: return "Unable to calculate the inverse matrix.";
    case 5: return "Wrong dimensions.";
    default:return "Unknown error.";
  }
}

static char *getErrorCobalp( int code)
{
  switch ( code)
  {
    case 0: return "Malloc failed.";
    default:return "Unknown error.";
  }
}

static char *getErrorTMatching( int code)
{
  switch ( code)
  {
    case 0: return "Malloc failed.";
    default:return "Unknown error.";
  }
}

static char *getErrorPrediction( int code)
{
  switch ( code)
  {
    case 0: return "BILD struct pointer is NULL.";
    case 1: return "Malloc failed.";
    case 2: return "No active predictors found.";
    default:return "Unknown error.";
  }
}

static char *getErrorGlobal( int code)
{
  switch ( code)
  {
    case 0: return "Successful exit.";
    case 2: return "Malloc failed.";
    case 3: return "Error while reading file.";
    default:return "Unknown error.";
  }
}

static char *getErrorName( int code)
{
  switch ( code / 10)
  {
    case 0: return getErrorGlobal( code % 10);
    case 1: return getErrorBild( code % 10);
    case 2: return getErrorParameter( code % 10);
    case 3: return getErrorSeiteninfo( code % 10);
    case 4: return getErrorData( code % 10);
    case 5: return getErrorPredictor( code % 10);
    case 6: return getErrorContext( code % 10);
    case 7: return getErrorBlock( code % 10);
    case 8: return getErrorBias( code % 10);
    case 9: return getErrorAuswahl( code % 10);
    case 10:return getErrorMatrix( code % 10);
    case 11:return getErrorCobalp( code % 10);
    case 12:return getErrorTMatching( code % 10);
    case 13:return getErrorPrediction( code % 10);
    default:return "Unknown error.";
  }
}
