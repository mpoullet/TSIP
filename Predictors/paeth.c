/*****************************************************
* Datei: paeth.c
* Autor: Thomas Schmiedel
* Datum: 02.11.2010
* Desc.: Implementiert den Paeth-Vorfilter
* Vers.: 1.0
* 08.07.2011 Strutz offset = (maxval+1)>>1
* 12.03.2014: offset computed based on bit depth
******************************************************/


#include "paeth.h"
#include "ginterface.h"
#include <stdlib.h>
#include <string.h>


/* Errechnet den Schätzwert für die Prädiktion */
MYINLINE int GetPredVal(PicData *pd, uint chan, uint x, uint y)
{
	static int ca, cb, cc,
			   da, db, dc, p;
	
	/* Umgebende Pixel holen,
	ca liegt links vom aktuellen Bildwert,
	cb liegt über dem aktuellen Bildwert,
	cc liegt links neben cb und über ca,
	also diagonal links oben,
	Werte außerhalb des Bildes sind 0 */
	
	if (x == 0) ca = 0; 
	else ca = (int)*GetXYValue(pd, chan, x - 1u, y);
	if (y == 0) cb = 0;
	else cb = (int)*GetXYValue(pd, chan, x, y - 1u);
	if (x == 0 || y == 0) cc = 0;
	else cc = (int)*GetXYValue(pd, chan, x - 1u, y - 1u);
	
	/* Erste Schätzung */	
	p = ca + cb - cc; 	
	/* Abstände der umgebenden Farbwerte zur Schätzung */
	da = abs(p - ca);
	db = abs(p - cb);
	dc = abs(p - cc);

	/* Farbwert mit geringster Abweichung zum Schätzwert
	zurückgeben */
	if (da <= db && da <= dc) return ca;
	else if (db <= dc) return cb;
	else return cc;	
}

/*---------------------------------------------
 *Paeth-Prädiktor auf PicData anwenden
 *--------------------------------------------*/
void PaethFilter( PicData *pd)
{
	uint x, y, chan;
	int err, range, offset;
	PicData *result = NULL;	
	
	/*  leere Zeichenfläche anlegen */
	result = CreatePicData( pd->width, pd->height, pd->channel, 
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
			for (x = 0; x < pd->width; x++)
			{				
		    	/* Schätzfehler bestimmen */
		    	err = (int)*GetXYValue( pd, chan, x, y) - GetPredVal( pd, chan, x, y);	
		    	
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
	DeletePicData(result);	
}


/*------------------------------------------------
 * Rückgewinnung des Originalbildes
 *------------------------------------------------*/
void PaethInverse(PicData *pd)
{
	uint x, y, chan;
	int val, err, range, offset;
	PicData *result = NULL;	
	
	/* Größten Farbwert bestimmen und leere Zeichenfläche anlegen */
	result = CreatePicData(pd->width, pd->height, pd->channel, 
		pd->bitperchannel, pd->bitperchan);
	
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
			for (x = 0; x < pd->width; x++)
			{				
		    	/* Schätzfehler holen und in seinen
		    	Originalwertebereich verschieben */		    	
		    	err  = GetPredVal(result, chan, x, y);
		    	// orig: 
					err -= offset;
		    	// Strutz: de-interleaving  geht nicht
	    		//if (err & 0x01) err = -(err + 1)/2;
					//else err = err >> 1;

		    	/* Farbwert rückgewinnen */ 
		    	val  = (int)*GetXYValue( pd, chan, x, y) + err;		    	
		    	if (val > range-1) val -= range;
		    	else if (val < 0) val += range;	    		    	
		    			    	
		    	/* ...und ihn speichern */
		    	*GetXYValue( result, chan, x, y) = (uint)val;		    			    	
			}	
		}	
		/* Kanal der Zeichenfläche in Originalbild kopieren */
		memcpy(pd->data[chan], result->data[chan], pd->size * sizeof(uint));
	}
	/* Zeichenfläche löschen */		
	DeletePicData( result);	
}

