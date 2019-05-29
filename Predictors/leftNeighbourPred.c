/*****************************************************
* File..: leftNeighbourPred.c
* Desc..: simple prediction from left neighbour
* Author: Tilo Strutz 
* Date..: 23.01.2012
* 12.03.2014: offset computed based on bit depth
******************************************************/
#include "leftneighbour.h"
#include "ginterface.h"
#include <stdlib.h>
#include <string.h>




/*----------------------------------------------------
 * LEFTNEIGHBOURprediction()
 *-----------------------------------------------------*/
void LEFTNEIGHBOURprediction( PicData *pd)
{
	uint x, y, chan;
	int err, range, offset;
	PicData *result = NULL;	
	
	/*  leere Zeichenfläche anlegen */
	result = CreatePicData(pd->width, pd->height, pd->channel, 
		pd->bitperchannel, pd->bitperchan);
	
	/* Alle Kanäle zeilenweise links oben beginnend durchgehen */
	for (chan = 0; chan < pd->channel; chan++)
	{
		range = (1 << pd->bitperchan[chan]);
		//maxval = pd->maxVal[chan];	Strutz this value will be different after prediction
/*
		better if YuvYuv is used, because offset will be the same 
		benefitting the run-length coding:
		maxval = (1 << pd->bitperchannel) - 1; 
*/
		offset = range >> 1;
		pd->offset[chan] = offset; /* overwrite old offset as it becomes obsolete here	*/
		for (y = 0; y < pd->height; y++)
		{
			if (y == 0)
			{
	    	err = (int)*GetXYValue( pd, chan, 0, y);	/* w/o prediction	*/
	    	if (err > ((range-1) >> 1)) err -= range;
	    	else if (err < -offset) err += range;			
				err += offset;	    	
	    	*GetXYValue( result, chan, 0, y) = (uint)err;		    			    	
			}
			else
			{
				/* take upper value	*/
	    	err = (int)*GetXYValue( pd, chan, 0, y) - (int)*GetXYValue( pd, chan, 0, y-1);	
	    	if (err > ((range-1) >> 1)) err -= range;
	    	else if (err < -offset) err += range;			
				err += offset;	    	
	    	*GetXYValue( result, chan, 0, y) = (uint)err;		    			    	
			}
			/* start with second column	*/
			for (x = 1; x < pd->width; x++)
			{				
		    	/* Schätzfehler bestimmen */
		    	err = (int)*GetXYValue( pd, chan, x, y) - (int)*GetXYValue( pd, chan, x-1, y);	
		    	
		    	/* Wertebereich des Schätzfehlers halbieren */ 
		    	if (err > ((range-1) >> 1)) err -= range;
		    	else if (err < -offset) err += range;			
		    	/* ...und in den positiven Bereich verschieben */    	
		    	// orig: 
					 err += offset;	    	
		    	// Strutz: interleaving  ! geht nicht !!
					//if (err < 0) err = -2*err -1;
					//else err = 2* err;

					/* Schätzfehler einsetzen */		    	
		    	*GetXYValue( result, chan, x, y) = (uint)err;		    			    	
			}	
		}		
		/* Kanal der Zeichenfläche in Originalbild kopieren */
		memcpy( pd->data[chan], result->data[chan], pd->size * sizeof(uint));	
	}		
	/* Zeichenfläche löschen */
	DeletePicData( result);	
}


/*----------------------------------------------------
 * LEFTNEIGHBOURprediction()
 *	inverse prediction 
 *-----------------------------------------------------*/
void iLEFTNEIGHBOURprediction( PicData *pd)
{
	uint x, y, chan;
	int val, err, range, offset;
	
	/* Alle Kanäle zeilenweise links oben beginnend durchgehen */
	for (chan = 0; chan < pd->channel; chan++)
	{
		range = (1 << pd->bitperchan[chan]);
		//maxval = pd->maxVal[chan]; Strutz: maxVal is changed by forward prediction
/*
		better if YuvYuv is used, because offset will be the same 
		benefitting the run-length coding:
		maxval = (1 << pd->bitperchannel) - 1; 
*/
		offset = range >> 1;
		for (y = 0; y < pd->height; y++)
		{
			if (y == 0)
			{
		    err  = (int)*GetXYValue( pd, chan, 0, y);
				err -= offset;
	    	val  = 0 + err;		    	
	    	if (val > range-1) val -= range;
	    	else if (val < 0) val += range;	    		    	
//	    	*GetXYValue( result, chan, 0, y) = (uint)val;		    			    	
	    	*GetXYValue( pd, chan, 0, y) = (uint)val;		    			    	
			}
			else
			{
		    err  = (int)*GetXYValue( pd, chan, 0, y);
				err -= offset;
	    	val  = (int)*GetXYValue( pd, chan, 0, y-1) + err;		    	
	    	if (val > range-1) val -= range;
	    	else if (val < 0) val += range;	    		    	
//	    	*GetXYValue( result, chan, 0, y) = (uint)val;		    			    	
	    	*GetXYValue( pd, chan, 0, y) = (uint)val;		    			    	
			}
			/* start with second column	*/
			for (x = 1; x < pd->width; x++)
			{				
		    	/* Schätzfehler holen und in seinen
		    	Originalwertebereich verschieben */		    	
		    	err  = (int)*GetXYValue( pd, chan, x, y);
		    	// orig: 
					err -= offset;
		    	// Strutz: de-interleaving  geht nicht
	    		//if (err & 0x01) err = -(err + 1)/2;
					//else err = err >> 1;

		    	/* Farbwert rückgewinnen */ 
		    	val  = (int)*GetXYValue( pd, chan, x-1, y) + err;		    	
		    	if (val > range-1) val -= range;
		    	else if (val < 0) val += range;	    		    	
		    			    	
		    	/* ...und ihn speichern */
//		    	*GetXYValue(result, chan, x, y) = (uint)val;		    			    	
		    	*GetXYValue( pd, chan, x, y) = (uint)val;		    			    	
			}	
		}	
		/* Kanal der Zeichenfläche in Originalbild kopieren */
//		memcpy(pd->data[chan], result->data[chan], pd->size * sizeof(uint));
	}
	/* Zeichenfläche löschen */		
//	DeletePicData( result);	
}