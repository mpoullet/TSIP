/*****************************************************
* File..: get_syntheticness.c
* Desc..: determines the level of synthetic content
* Author: Tilo Strutz
* Date..: 02.02.2015 (Strutz)
* 09.04.2015 exlude counts before and including mode value
******************************************************/
#include <math.h>
#include <string.h>
#include <assert.h>

#include "codec.h" /* for max()	*/
#include "ginterface.h"

/*---------------------------------------------------------
 * get_syntheticness(),
 * result: 0-200,
 * the higher, the more synthetic
 *---------------------------------------------------------*/
void get_syntheticness( PicData *pd, ImageFeatures * iF)
{
	int	diff;
	unsigned int x, y;
	unsigned int i, numChan, comp, hist_size;
	unsigned int modal1_i, modal2_i;
	unsigned long modal1_cnt, modal2_cnt;
	unsigned long pixel_count, count1, old_cnt1, old_cnt2;
	unsigned long  *histo1 = NULL, *histo2 = NULL;
	unsigned long py, pos;

	/* number of different values in component */
	hist_size = (1u << pd->bitperchannel);
			
	if (pd->channel >= 3) numChan = 3;	/* ignore alpha channel	*/
	else numChan = 1;

	/* create histogram  */
	ALLOC( histo1, hist_size, unsigned long); /* histogram of horizontal gradients	*/
	ALLOC( histo2, hist_size, unsigned long); /* histogram of vertical gradients	*/

	for (comp = 0; comp < numChan; comp++) /* for all components in the image	*/
	{
		/* times 2 because two histograms are evaluated	*/
		pixel_count = 2 * (pd->height - 2) * (pd->width - 2); /* total number of accessed pixels	*/
		memset( histo1, 0, hist_size * sizeof(unsigned long)); /* reset histograms	*/
		memset( histo2, 0, hist_size * sizeof(unsigned long));

		/* könnte doch auch in der zweiten Zeile/Spalte beginnen, oder? */
		for (y = 2, py = 2*pd->width; y < pd->height; y++, py += pd->width)
		{
			for (x = 2, pos = py+2; x < pd->width; x++, pos++)
			{
				/* evaluate horizontal difference	*/
				diff = pd->data[comp][pos] - pd->data[comp][pos-1];
				histo1[ abs( diff) ]++;
				/* evaluate vertical difference	*/
				diff = pd->data[comp][pos] - pd->data[comp][pos-pd->width];
				histo2[ abs( diff) ]++;
			}
		}
#ifdef _DEBUGx
			{
				FILE *out=NULL;
				char filename[512];
				sprintf( filename, "hist_grad_%d.txt", comp);
				out = fopen( filename, "wt");
				fprintf( out,"# created by TSIP_v1.30d\n");
				fprintf( out,"# size: %d\n", hist_size);
				fprintf( out,"# idx  horizontal  vertikal diag1 diag2  2-dist 2-dist\n");
				fprintf( out,"# -------------------------- \n");

				for ( i = 0; i < hist_size; i++) 
				{
					fprintf( out,"%3d %6d %6d", i, 
						histo1[i], histo2[i]);
					//fprintf( out," %6d %6d %6d %6d\n", i, 
					//	histo3[i], histo4[i], histo5[i], histo6[i]);
					fprintf( out,"\n");
				}
				fclose( out);
			}
#endif
		/* - it is expected for natural images that small differences are more frequent 
		 *   than large diferences
		 * - differences == 0 correspond to constant signal values (background)
		 * - if larger differences appear with higher count than smaller differences, 
		 *   this is an indication that the image contains synthetic elements
		 */

		/* dertimination of modal values in both histograms	*/
		modal1_i = 0;
		modal1_cnt = histo1[0];
		modal2_i = 0;
		modal2_cnt = histo2[0];
		for (i = 1u; i < hist_size; i++)
		{
			if (modal1_cnt < histo1[i])
			{
				modal1_cnt = histo1[i];
				modal1_i = i;
			}
			if (modal2_cnt < histo2[i])
			{
				modal2_cnt = histo2[i];
				modal2_i = i;
			}
		}
		old_cnt1 = modal1_cnt;
		old_cnt2 = modal2_cnt;

		count1 = 0;	
		for (i = 0u; i < hist_size; i++)
		{
			if (i > modal1_i) /* exclude counts before mode value	*/
			{
				if (histo1[i])/* exclude empty bins (can be removed via compaction)	*/
				{
					/* accumulate increasing bin values only	*/
					count1 += max( 0, (long)histo1[i] - (long)old_cnt1);	
					old_cnt1 = histo1[i];
				}
			}
			else pixel_count -= histo1[i]; /* remove contribution of this count	*/
			if (i > modal2_i)
			{
				/* exclude counts before mode value	*/
				if (histo2[i] && i > modal2_i)
				{
					/* exclude empty bins (can be removed via compaction)	*/
					count1 += max( 0, (long)histo2[i] - (long)old_cnt2);	
					old_cnt2 = histo2[i];
				}
			}
			else pixel_count -= histo2[i];
		}
		if (count1 == 0)  /* constant image	*/
			iF->syn_grad[comp] = 200;
		else if (pixel_count) /* avoid division by zero	*/
			iF->syn_grad[comp] = (unsigned int)(200.0 * (double)count1 / (double)pixel_count);
		else
			iF->syn_grad[comp] = 0;
	} /* comp */
	
	/* Aufräumen und return */
	FREE( histo1);
	FREE( histo2);
}

/*---------------------------------------------------------
 * get_syntheticnessXXX(),
 * result: 0-200,
 * the higher, the more synthetic
 *---------------------------------------------------------*/
unsigned int get_syntheticnessXXX( PicData *pd)
{
	int	diff;
	unsigned int x, y;
	unsigned int i, numChan, comp, hist_size;
	unsigned long old_cnt1, old_cnt2;
	unsigned long  count, *histo1 = NULL, *histo2 = NULL;
	unsigned long py, pos, pixel_count;

	/* number of different values in component */
	hist_size = (1u << pd->bitperchannel);
			
	if (pd->channel >= 3) numChan = 3;	/* ignore alpha channel	*/
	else numChan = 1;

	/* create histogram  */
	ALLOC( histo1, hist_size, unsigned long);
	ALLOC( histo2, hist_size, unsigned long);
	pixel_count = (pd->height - 1) * (pd->width - 1) * numChan;
	count = 0;
	for (comp = 0; comp < numChan; comp++)
	{
		memset( histo1, 0, hist_size * sizeof(unsigned long));
		memset( histo2, 0, hist_size * sizeof(unsigned long));
		for (y = 1, py = pd->width; y < pd->height; y++, py += pd->width)
		{
			for (x = 1, pos = py+1; x < pd->width; x++, pos++)
			{
				diff = pd->data[comp][pos] - pd->data[comp][pos-1];
				histo1[ abs( diff) ]++;
				diff = pd->data[comp][pos] - pd->data[comp][pos-pd->width];
				histo2[ abs( diff) ]++;
			}
		}
#ifdef _DEBUGx
			{
				FILE *out=NULL;
				char filename[512];


				sprintf( filename, "hist_grad_%d.txt", comp);
				out = fopen( filename, "wt");
				fprintf( out,"# created by TSIP_v1.30d\n");
				fprintf( out,"# size: %d\n", hist_size);
				fprintf( out,"# idx  horizontal  vertikal \n");
				fprintf( out,"# -------------------------- \n");

				for ( i = 0; i < hist_size; i++) 
				{
					fprintf( out,"%3d %6d %6d\n", i, histo1[i], histo2[i]);
				}
				fprintf( out,"\n");
				fclose( out);


			}

#endif

		/* it is expected that small differences are mor frequent than 
		 * large diferences
		 * differences == 0 correspond to constant signal values (background)
		 */
		/* Differenzen benachbarter Häufigkeiten bilden und aufsummieren */
		/* exclude constant background */
		pixel_count = pixel_count - (long)((histo1[ 0] + histo2[ 0]) >> 1);
		old_cnt1 = histo1[ 1];
		old_cnt2 = histo2[ 1];
		for (i = 2u; i < hist_size; i++)
		{
			if (histo1[i])/* exclude empty bins (can be removed via compaction)	*/
			{			
				count += abs( (long)old_cnt1 - (long)histo1[i]);	
				old_cnt1 = histo1[i];
			}
			if (histo2[i])/* exclude empty bins (can be removed via compaction)	*/
			{
				count += abs( (long)old_cnt2 - (long)histo2[i]);	
				old_cnt2 = histo2[i];
			}
		}
		count += old_cnt1;
		count += old_cnt2;
	} /* comp */
	
	/* Aufräumen und return */
	FREE( histo1);
	FREE( histo2);

	if (pixel_count) /* avoid division by zero	*/
		return (unsigned int)(100.0 * (double)count / (double)pixel_count);
	else
		return 0;
}

