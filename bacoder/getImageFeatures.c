/*****************************************************
* File..: getImageFeatures.c
* Desc..: determination of image features
* Author: T. Strutz
* Date..: 09.02.2012
* 17.02.2012 add estimateNoise()
* 18.07.2014 new getRepeatness()
* 21.07.2015 new getCoMoSyNoiCon() combined computation of features
* 10.09.2015 bugfix in getCoMoSyNoiCon() count1 == 0
* 28.09.2015 new: getPatternMeasure()
* 01.10.2015 depth as function of level
* 02.10.2015 new accumulation of pixel_count in getCoMoSyNoiCon()
* 02.10.2015 roughness now more orientation independent
* 02.10.2015 correlation: assume mean0 = mean1 saves some computation
*						but include other diagonal direction
* 05.10.2015 features changed: no roughnes, noise opnly incremtal
*					 correlation based on ACF, syngrad only single histogram 
* 29.04.2016 new: coding_method_isPassed, lfdNr2 instead of reusing lfdNr
* 26.08.2016 ladded: joint entropies and there calculation
******************************************************/
#include <assert.h>
#include <math.h>
#include "stats.h"
#include "imageFeatures.h"
#include "codec.h" /* for max()	*/
#include "hxc.h"	/* for MAX_PATTERN_SIZE, C_TREE_NODE_2	*/
#include "types.h"	/* for CALLOC */

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

#include "eig3.h"

//void get_colour_variance( PicData *pd, ImageFeatures *iF);
//#define invLOG2	1.442695041  /* 1 / ln(2)  */

extern unsigned long lfdNr2; /*already in getHXC2tolerances.c	*/

C_TREE_NODE_2 *create_next_item_2( C_TREE_NODE_2 *t_node, 
																	long  Y[4], unsigned int numOfElements);
void update_histogram( C_TREE_NODE_2 *t_node, 
											long  Y[], unsigned int numOfElements);
void free_tree( C_TREE_NODE_2 *t_root);
void free_tree_recursive( C_TREE_NODE_2 *t_node);

unsigned long getBlockNumPatterns( PicData *pd, 
																	unsigned int y_start, unsigned int y_end,
																	unsigned int x_start, unsigned int x_end);
unsigned long getBlockNumPatterns2( PicData *pd, 
																	 unsigned int y_start, unsigned int y_end,
																	 unsigned int x_start, unsigned int x_end);

/*--------------------------------------------------------
* getCoMoSyNoiCon()
* simultaneous computation of
* - correlation at m= 1
* - monotony
* - syntheticness based on gradients
* - noise
* - constance
* for all components separately
*-------------------------------------------------------*/
void getCoMoSyNoiCon( PicData *pd, ImageFeatures *iF)
{
	/* general	*/
	long val0, val1, val2, val3, val4;
	unsigned int comp, numChan;
	unsigned int x, y, off1, off2;
	unsigned int xstep = 1, ystep=1;
	unsigned long pos, py, cnt, cnt_glob;

	/* correlation	*/
	double energy0, energy1, energy2, energy3, energy4;
	double sum0, sum0sqared;
	double rxx0, rxx1, rxx2, rxx3, rxx4, corrcoeff;

	/* monotony	*/
	unsigned int up_flag1, up_flag2;
	unsigned long change_count;

	/* syn_grad	*/
	int	diff1, diff2;
	unsigned int i, hist_size;
	//unsigned int modal_i;
	//unsigned long modal_cnt;
	//unsigned long pixel_count, count_grad, old_cnt;
	unsigned long  *histo = NULL;

	/* noise, constance	*/
	int h, v, d1, d2, c1, c2, c3, c4;
	long cp, m;
	unsigned long count_const;
	unsigned long long sum;


	if (pd->channel >= 3) numChan = 3;	/* ignore alpha channel	*/
	else numChan = 1;
	off1 = pd->width + 1;
	off2 = pd->width - 1;


	xstep = max( 1, pd->width /500); /* take 500 columns	*/
	ystep = max( 1, pd->height / 500); /* take 500 rows	*/


	/* number of different values in component */
	hist_size = (1u << pd->bitperchannel);
	/* create histogram  */
	ALLOC( histo, hist_size, unsigned long); /* histogram of all gradients	*/
	memset( histo, 0, hist_size * sizeof(unsigned long)); /* reset histograms	*/

	cnt_glob = 0;
	count_const = 0;

	for (comp = 0; comp < numChan; comp++) /* for all components	*/
	{
		energy0 = energy1 = energy2 = energy3 = energy4 = 0.0;
		sum0 = 0.0;
		up_flag1 = up_flag2 = change_count = 0;
		sum = 0;
		cnt = 0;


		/* for all rows except the first and the last	*/
		for (y = 1, py = pd->width; y < pd->height-1; y+=ystep, py+=pd->width*ystep)
		{
			/* for all columns excluding first and the last	*/
			for (x = 1, pos = py+1; x < pd->width-1; x+=xstep, pos+=xstep)
			{
				cnt++; /* count number of processed pixels	*/
				val0 = pd->data[comp][pos];
				val1 = pd->data[comp][pos+off1];
				val2 = pd->data[comp][pos+off2];
				val3 = pd->data[comp][pos+1];
				val4 = pd->data[comp][pos+pd->width];


				/* correlation	*/
				energy0 += val0 * val0;
				energy1 += val0 * val1;
				energy2 += val0 * val2;
				energy3 += val0 * val3;
				energy4 += val0 * val4;
				sum0 += val0;

				/* monotony	*/
				if (val1 > val0)
				{
					if (up_flag1 == 0)
					{
						change_count++;
						up_flag1 = 1;
					}
				}
				else	if (val1 < val0)
				{
					if (up_flag1 == 1)
					{
						change_count++;
						up_flag1 = 0;
					}
				}
				if (val2 > val0)	/* also other diagonal direction	*/
				{
					if (up_flag2 == 0)
					{
						change_count++;
						up_flag2 = 1;
					}
				}
				else	if (val2 < val0)
				{
					if (up_flag2 == 1)
					{
						change_count++;
						up_flag2 = 0;
					}
				}

				/* evaluate horizontal difference	*/
				diff1 = val0 - val3;
				histo[ abs( diff1) ]++;
				/* evaluate vertical difference	*/
				diff2 = val0 - val4;
				histo[ abs( diff2) ]++;

				/* constance can be derived from final histogram of gradients */
				if ( diff1 == 0 && diff2 == 0)
				{
					count_const++;
				}

				/* noise */
				cp = - (val0 + val0);
				h = cp + pd->data[comp][pos-1] + pd->data[comp][pos+1];
				v = cp + pd->data[comp][pos-pd->width] + pd->data[comp][pos+pd->width];
				d1 = cp + pd->data[comp][pos-1-pd->width] + pd->data[comp][pos+1+pd->width];
				d2 = cp + pd->data[comp][pos-1+pd->width] + pd->data[comp][pos+1-pd->width];
				c1 = cp + pd->data[comp][pos-1] + pd->data[comp][pos-pd->width];
				c2 = cp + pd->data[comp][pos+1] + pd->data[comp][pos-pd->width];
				c3 = cp + pd->data[comp][pos+1] + pd->data[comp][pos+pd->width];
				c4 = cp + pd->data[comp][pos-1] + pd->data[comp][pos+pd->width];
				/* get minimum value of all comparisons	*/
				m = min( abs(h), abs(v));
				m = min( m, abs(d1));
				m = min( m, abs(d2));
				m = min( m, abs(c1));
				m = min( m, abs(c2));
				m = min( m, abs(c3));
				m = min( m, abs(c4));
				//sum += m;
				if (m) sum++;

			} /* for x	*/
		}

		if (cnt)
		{
			/* rxx0 = VAR(X) = E[X^2] - (E[X])^2	*/
			/* rxx1 = Rxx[1] = E[x[n] * x[n-1]] - (E[x[n]]*E[x[n+1]])
			/* E[x[n]] = E[x[n+1]]  for -infty < n < infty*/
			//mean0= sum0 / cnt;
			sum0sqared = sum0 * sum0;
			rxx0 = energy0 - sum0sqared / cnt;
			rxx1 = energy1 - sum0sqared / cnt;
			rxx2 = energy2 - sum0sqared / cnt; /* assuming that sum2 \approx sum1  etc. */
			rxx3 = energy3 - sum0sqared / cnt;
			rxx4 = energy4 - sum0sqared / cnt;
			//assert( rxx1 <= rxx0);
			if (rxx1 > rxx0)
			{
				rxx1 = rxx0;
			}
			if (rxx0 != 0.0 ) corrcoeff = (rxx1 + rxx2 + rxx3 + rxx4) / rxx0;
			else corrcoeff = 0.;
			iF->corrcoeff[comp] = (unsigned int)max( 0, floor( 25. * corrcoeff + 0.5));

			iF->monotony[comp]  = 100-(unsigned int)floor( ( 50.0 * change_count / cnt) + 0.5);
			//iF->constance[comp] = (unsigned long)(100*count_const / cnt);

			//if (pd->bitperchannel >= 8)
			//	iF->noise[comp] = _GETMIN( 200, (unsigned long)(( 10 * sum) / 
			//			((1ul<<(pd->bitperchannel-8)) * cnt)) );  /* why pd->bitperchannel ???? ==> normalisation to dynamic range */
			//else
			//	iF->noise[comp] =  _GETMIN( 200, (unsigned long)((256 * 10 * sum) / 
			//			((1ul << pd->bitperchannel) * cnt)) );

			iF->noise[comp] = _GETMIN( 200, (unsigned long)((200 * sum) / cnt) );
		}
		else
		{
			iF->corrcoeff[comp] = 0;
			iF->monotony[comp] = 0;
			iF->noise[comp] = 0;
		}
		///* determination of modal values in histogram	*/
		//pixel_count = 0;	/* accumulate involved gradients counts */
		//modal_i = 0;
		//modal_cnt = histo[0];
		//for (i = 1u; i < hist_size; i++)
		//{
		//	if (modal_cnt < histo[i])
		//	{
		//		modal_cnt = histo[i];
		//		modal_i = i;
		//	}
		//}
		//old_cnt = modal_cnt;

		//count_grad = 0;	
		//for (i = 0u; i < hist_size; i++)
		//{
		//	if (i > modal_i) /* exclude counts before mode value	*/
		//	{
		//		if (histo[i])/* exclude empty bins (can be removed via compaction)	*/
		//		{
		//			/* accumulate increasing bin values only	*/
		//			count_grad += max( 0, (long)histo[i] - (long)old_cnt);	
		//			old_cnt = histo[i];
		//		}
		//		pixel_count += histo[i]; /* accumulate contribution of this count	*/
		//	}
		//}
		//if (histo[0] == cnt ) /* all differences are equal to zero	*/
		//{
		//	iF->syn_grad[comp] = 201; /* constant signal, exception handling for average value	*/
		//}
		//else if (pixel_count) /* avoid division by zero	*/
		//{
		//	iF->syn_grad[comp] = (unsigned int)(200.0 * (double)count_grad / (double)pixel_count);
		//}
		//else
		//{
		//	if (count_grad) iF->syn_grad[comp] = 0;
		//	else iF->syn_grad[comp] = 202;	/*  exception handling for average value	*/
		//}

		cnt_glob += cnt;
	} /* for comp	*/

	if (cnt_glob)
	{
		iF->constance = (unsigned long)(100*count_const / cnt_glob);
	}
	else
	{
		iF->constance = 0;
	}

	/* syn_grad features
	* - it is expected for natural images that small differences are more frequent 
	*   than large differences
	* - differences == 0 correspond to constant signal values (background)
	* - if larger differences appear with higher count than smaller differences, 
	*   this is an indication that the image contains synthetic elements
	*/


	/* new syngrad value	*/
	/* compact histogram	*/
	if (histo[0] == cnt_glob ) /* all differences are equal to zero	*/
	{
		iF->syn_grad = 0; /* constant signal	*/
	}
	else
	{
		//int last_pos = 1;
		long synVal;
		unsigned int idx = 1;
		//unsigned int start=0, stop=0;
		//double sum;
		double b, Sx = 0., Sxx = 0., Sy = 0., Sxy = 0., S = 0.;
		for (i = 1u; i < hist_size; i++)
		{
			if (histo[i] == 0) continue;
			histo[idx] = histo[i];
			//last_pos = idx;
			S = S + 1;
			Sx = Sx + idx;
			Sxx = Sxx + idx*idx;
			//Sy = Sy + log( (double)histo[idx]);
			// Sxy = Sxy + idx*log( (double)histo[idx]);
			Sy = Sy + histo[idx];
			Sxy = Sxy + idx * histo[idx];
			idx++;
		}
		/* fitting of histo[1...last_pos] to a+ b* histo[i])		 */
		/* a = (Sxx * Sy - Sx * Sxy) / (S * Sxx - Sx*Sx); not required	*/
		b = (S * Sxy - Sx * Sy) / (S * Sxx - Sx*Sx);
		// maybe it is better to merge syn_grad with syn
		//sum = 0.;
		//for (i = 1u; i < S; i++)
		//{
		//	sum += histo[i];
		//	if (sum > Sy *0.50) 
		//	{
		//		stop = (int)S;
		//		start = min( i+1, stop);
		//		break;
		//	}
		//}
		//if ( stop > start) /* at least two observations (bins) must be included */
		//{
		//	/* fitting for only 50% of highest gradients */
		//	Sx = Sxx = Sy = Sxy = S = 0.;
		//	for (i = start; i <= stop; i++)
		//	{
		//		S = S + 1;
		//		Sx = Sx + i;
		//		Sxx = Sxx + i*i;
		//		Sy = Sy + histo[i];
		//		Sxy = Sxy + i * histo[i];
		//		idx++;
		//	}
		//	b = (S * Sxy - Sx * Sy) / (S * Sxx - Sx*Sx);
		//}
		//else b = 0.;
		synVal = min( 200, (long)(200 + b));
		iF->syn_grad = max( 0, synVal);
	}

	/* Aufräumen und return */
	FREE( histo);
}

/*---------------------------------------------------------------
*	getColourContSatVar()
*---------------------------------------------------------------*/
void getColourContSatVar( PicData *pd, ImageFeatures *iF)
{
	/* general	*/
	long valR0, valR1, valR2;
	long valG0, valG1, valG2, valG3;
	long valB0, valB1, valB2;

	/* contrast	*/
	int diff;
	double sum, relSum, relSumS, diff2;
	unsigned int xstep = 1, ystep=1;
	unsigned long x, y, pos, py, cnt, relCnt;

	/* saturation	*/
	double sumS;

	/* variance	*/
	double d[3], A[3][3], V[3][3];	/* eigen values and vectors*/
	double meanR, meanG, meanB;
	double var_RR, var_RG, var_RB;
	double var_GG, var_GB, var_BB;
	unsigned long	long sum_R, sum_G, sum_B;
	unsigned long	long energ_R, energ_G, energ_B;
	unsigned long	long energ_RG, energ_RB, energ_GB;


	xstep = max( 1, pd->width / pd->width); /* take all columns	*/
	ystep = max( 1, pd->height / pd->height); /* take all rows	*/

	xstep = max( 1, pd->width /500); /* take 500 columns	*/
	ystep = max( 1, pd->height / 500); /* take 500 rows	*/

	sum = sumS = 0;
	relSum = relSumS = 0.;
	cnt = relCnt = 0;
	sum_R = sum_G = sum_B = 0;
	energ_R = energ_G = energ_B = 0;

	if (pd->channel < 3) /* if not a RGB image	*/
	{
		iF->colourSaturation = 0;
		/* variances	*/
		iF->rgbStdDev[0] = 0;
		iF->rgbStdDev[1] = 0;

		/* for all rows excluding first	*/
		for (y = 1, py = pd->width; y < pd->height; y+=ystep, py+=pd->width*ystep)
		{
			/* for all columns excluding first	*/
			for (x = 1, pos = py+1; x < pd->width; x+=xstep, pos+=xstep)
			{
				cnt++; /* count number of processed pixels	*/

				valG0 = pd->data[0][pos];
				valG1 = pd->data[0][pos-1];
				valG2 = pd->data[0][pos-pd->width];
				valG3 = pd->data[0][pos-pd->width-1];
				diff = valG0 - valG1;
				diff2 = diff * diff;
				sum += diff2; 

				diff = valG0 - valG2; 
				diff2 = diff * diff;
				sum += diff2; 

				/* difference between current value and mean of 4 values	*/
				if (valG0 != valG1 || valG0 != valG2 || valG0 != valG3 )
				{
					/* ignore constant regions	*/
					relSum += abs( valG0 - ( (valG0 + valG1 + valG2 + valG3)>>2) );
					relCnt++;
				}

				/* variance	 */
				sum_R += valG0;
				energ_R += valG0 * valG0;
			} /* x */
		} /* y */

		if (cnt)  /* avoid division by zero	*/
		{
			/* sumMax = cnt * (255*255 + 255*255)	*/
			iF->colourContrast = _GETMIN( 200, (unsigned int)floor( 2*200.0 * sum / (2*255*255*cnt) + 0.5));
			iF->relColourContrast = _GETMIN( 200, (unsigned int)floor( 2.0 * relSum / relCnt + 0.5));

			/* variance	 */
			meanR = (double)sum_R / cnt;
			var_RR = energ_R / cnt - meanR * meanR;
			iF->rgbStdDev[2] = (unsigned int)( 2 * sqrt( var_RR));
		}
		else
		{
			iF->colourContrast = 0;
			iF->rgbStdDev[2] = 0;
		}
	}
	else /* if RGB image	*/
	{
		energ_RG = energ_RB = energ_GB = 0;
		/* for all rows excluding first	*/
		for (y = 1, py = pd->width; y < pd->height; y+=ystep, py+=pd->width*ystep)
		{
			/* for all columns excluding first	*/
			for (x = 1, pos = py+1; x < pd->width; x+=xstep, pos+=xstep)
			{
				cnt++; /* count number of processed pixels	*/

				valR0 = pd->data[0][pos];
				valG0 = pd->data[1][pos];
				valB0 = pd->data[2][pos];
				valR1 = pd->data[0][pos-1];
				valG1 = pd->data[1][pos-1];
				valB1 = pd->data[2][pos-1];
				valR2 = pd->data[0][pos-pd->width];
				valG2 = pd->data[1][pos-pd->width];
				valB2 = pd->data[2][pos-pd->width];

				/* contrast	*/
				diff = valR0 - valR1;
				diff2 = diff * diff;
				sum += diff2; 
				relSum += diff2 /( (valR0 + 1) * (valR1 + 1) );

				diff = valG0 - valG1; 
				diff2 = diff * diff;
				sum += diff2; 
				relSum += diff2 /( (valG0 + 1) * (valG1 + 1) );

				diff = valB0 - valB1; 
				diff2 = diff * diff;
				sum += diff2; 
				relSum += diff2 /( (valB0 + 1) * (valB1 + 1) );


				diff = valR0 - valR2; 
				diff2 = diff * diff;
				sum += diff2; 
				relSum += diff2 /( (valR0 + 1) * (valR2 + 1) );

				diff = valG0 - valG2; 
				diff2 = diff * diff;
				sum += diff2; 
				relSum += diff2 /( (valG0 + 1) * (valG2 + 1) );

				diff = valB0 - valB2; 
				diff2 = diff * diff;
				sum += diff2; 
				relSum += diff2 /( (valB0 + 1) * (valB2 + 1) );


				/* saturation	*/
				{
					long absSum;
					absSum = labs( valR0 - valG0) + labs( valR0 - valB0) + labs( valB0 - valG0);
					sumS += absSum; 
					relSumS += (double)absSum / (valR0 + valG0 + valB0 + 1);
				}

				/* variance	 */
				sum_R += valR0;
				sum_G += valG0;
				sum_B += valB0;
				energ_R += valR0 * valR0;
				energ_G += valG0 * valG0;
				energ_B += valB0 * valB0;
				energ_RG += valR0 * valG0;
				energ_RB += valR0 * valB0;
				energ_GB += valG0 * valB0;
			} /* x */
		} /* y */
		if (cnt)  /* avoid division by zero	*/
		{
			/* sumMax = cnt * 6* (255*255)	*/
			iF->colourContrast =	_GETMIN( 200, (unsigned int)floor(  4*200.0 * sum / (6.*255*255*cnt) + 0.5));
			iF->relColourContrast =	_GETMIN( 200, (unsigned int)floor(  2*200.0 * relSum / (6.*cnt) + 0.5));
			iF->colourSaturation = _GETMIN( 200, (unsigned int)floor(  200.0 * sumS / (512. * cnt) + 0.5));
			iF->relColourSaturation = _GETMIN( 200, (unsigned int)floor(  200.0 * relSumS / cnt + 0.5));

			/* co-variances	
			* covar = 1/N SUM (Ri-RM)*(Gi-GM)
			* covar = 1/N SUM (Ri*Gi + RM*GM - Ri*GM - Gi*RM)
			* covar = 1/N SUM (Ri*Gi) + 1/N SUM (RM*GM) - 1/N SUM (Ri*GM) - 1/N SUM (Gi*RM)
			* covar = 1/N SUM (Ri*Gi) + RM*GM*1/N SUM 1 - GM* 1/N SUM Ri - RM * 1/N SUM Gi
			* covar = 1/N SUM (Ri*Gi) + RM*GM - GM* RM - RM * GM
			* covar = 1/N SUM (Ri*Gi) - RM * GM
			*/
			meanR = (double)sum_R / cnt;
			meanG = (double)sum_G / cnt;
			meanB = (double)sum_B / cnt;
			var_RR = energ_R / cnt - meanR * meanR;
			var_GG = energ_G / cnt - meanG * meanG;
			var_BB = energ_B / cnt - meanB * meanB;
			var_RG = energ_RG / cnt - meanR * meanG;
			var_RB = energ_RB / cnt - meanR * meanB;
			var_GB = energ_GB / cnt - meanG * meanB;

			/* 
			* determine the eigenvectors
			*
			* (A - lambda_i*I)*a_i = 0
			* in LS: J*a = y
			* eigenvectors = columns of V
			*/

			A[0][0] = var_RR;
			A[0][1] = A[1][0] = var_RG;
			A[0][2] = A[2][0] = var_RB;
			A[1][1] = var_GG;
			A[1][2] = A[2][1] = var_GB;
			A[2][2] = var_BB;
			eigen_decomposition( A, V, d);
			iF->rgbStdDev[0] = (unsigned int)( 2 * sqrt( d[0]));
			iF->rgbStdDev[1] = (unsigned int)( 2 * sqrt( d[1]));
			iF->rgbStdDev[2] = (unsigned int)( 2 * sqrt( d[2]));
		}
		else
		{
			iF->colourContrast =  0;
			iF->colourSaturation = 0;
		}
	} /* if RGB */
}

#ifdef NOTUSED
/*---------------------------------------------------------------
*	get_information()
* take first appearance of each histogram item into account!!
* => information_content will be larger in very detailed, branched list
*
* H(hist) = - SUM_1..histsize  item_count/node_count * ld(node_count / item_count)
* H(tot) = (SUM_1..hist_number  H(hist) * node_count) / sum_node_counts
*---------------------------------------------------------------*/
void get_information( C_TREE_NODE_2 *t_node, int level, 
										 double *entropy, unsigned long *numOfLeaves,
										 unsigned long *sum_node_counts)
{
	if (t_node->next_level != NULL)
	{
		get_information( t_node->next_level, level+1, 
			entropy, numOfLeaves, sum_node_counts); /* recursive call	*/
	}
	else
	{
		double p;
		unsigned int i;
		HIST_LIST_ELEM2 *current_hist_element;

		(*numOfLeaves)++; /* number of histograms	*/
		(*sum_node_counts) += t_node->count; /* total count should be equal to image size	*/

		/* t_node->count is total number of counts in this histogram	*/

		current_hist_element = t_node->top_hist_elem;

		/* update overall information_content	*/
		for ( i = 0; i < t_node->hist_size; i++)
		{
			p = (double)current_hist_element->count / t_node->count;
			*entropy -= current_hist_element->count * log( p) * invLOG2;
			current_hist_element = current_hist_element->next;
		}

	}
	if (t_node->next_item != NULL)
	{
		get_information( t_node->next_item, level, 
			entropy, numOfLeaves, sum_node_counts); /* recursive call	*/
	}
}
#endif

/*---------------------------------------------------------------
*	next_level_node_()
*
*---------------------------------------------------------------*/
C_TREE_NODE_2 *next_level_node_( C_TREE_NODE_2 *t_node,	
																long  Y[], unsigned int numOfElements,
																unsigned long *depth)
{
	int match_flag, stop_flag;
	unsigned int i;

	assert( t_node != NULL);

	if (t_node->next_level == NULL) /* next level does not exist yet	*/
	{
		CALLOC( t_node->next_level, 1, C_TREE_NODE_2);
		if (ERRCODE == MALLOC_FAILED)	return NULL;
		(*depth)++; /* increment horizontal depth of tree */

		t_node = t_node->next_level;
		t_node->next_item = NULL;
		t_node->next_level = NULL;
		t_node->top_hist_elem = NULL;
		for ( i = 0; i < numOfElements; i++)
		{
			t_node->val[i] = Y[i];
		}
		/* count and lfdNr are not needed for getting the patternMeasure */
		t_node->count = 1;
		t_node->lfdNr = lfdNr2;
		lfdNr2++; /* global variable	*/
#ifdef CHECK_MEMORY_LEAKS
		printf( "\n a: lfdNr= %d", t_node->lfdNr);
		for ( i = 0; i< numOfElements; i++)
		{
			printf( ", %d", t_node->val[i]);
		}
#endif
		return t_node;
	}
	else
	{
		t_node = t_node->next_level;

		stop_flag = 0;
		do
		{	/* scan item for match	*/
			match_flag = 1;
			for ( i = 0; i < numOfElements; i++)
			{
				if (t_node->val[i] != (unsigned)Y[i])
				{
					match_flag = 0; /* does not match	*/
					break; /* exit for loop	*/
				}
			}
			if ( match_flag) break;  /* exit do-while loop	*/
			if (t_node->next_item == NULL)
			{
				stop_flag = 1;
			}
			else
			{
				t_node = t_node->next_item; /* go to next item at this level	*/
			}
			(*depth)++; /* increment horizontal depth of tree */
		} while (!stop_flag);

		if (match_flag)
		{
			t_node->count++; /* match found	*/
		}
		else /* no match could be found, allocate new item	*/
		{
			t_node = create_next_item_2( t_node, Y, numOfElements);
			(*depth)++; /* increment horizontal depth of tree */
		}

		return t_node;
	}
}

/*---------------------------------------------------------------
*	update_histogram_()
*
*---------------------------------------------------------------*/
void update_histogram_( C_TREE_NODE_2 *t_node, 
											 long  Y[], unsigned int numOfElements,
											 unsigned long *patternMeasure)
{
	HIST_LIST_ELEM2 *current_hist_element;

	/* pointer to top of histogramm list	*/
	current_hist_element = t_node->top_hist_elem;
	if (current_hist_element == NULL)
	{
#ifdef H_DEBUG
		fprintf( stdout, "\n\t\t histogram is still empty, start with actual X, count=1");
#endif
		(*patternMeasure)++; /* count all histograms => all different pattern	*/
		/* there is no histogram yet	*/
		t_node->top_hist_elem =
			create_histogram_list_element2( Y, numOfElements, 1);
		t_node->hist_size++; /* update the number of histogram items, not required here, actually	*/
	}
	else
	{	
		/* this part is not necessary for counting patternMeasure	!!!!!!!!!!!!!!!!*/ 
	}
}

/*--------------------------------------------------------
* getPatternMeasure()
*-------------------------------------------------------*/
unsigned long getPatternMeasure( PicData *pd, unsigned long *maxDepth,
																unsigned long numOfColours)
{
	int match_flag;
	int d[MAX_PATTERN_SIZE];	/* offsets for pixel addressing	*/
	//int pattern_size = 2; /* only A and B	*/
	int pattern_size = 4; /* only A and B, C, D	*/
	unsigned int i;
	unsigned int numOfElements;
	/* assume a maximum number of four components	*/
	long A[4], B[4], C[4], D[4], E[4], F[4], X[4];
	unsigned long r, c;
	unsigned long py, pos;
	unsigned long patternMeasure = 0, depth, maxLevelDepth[MAX_PATTERN_SIZE];
	C_TREE_NODE_2 *tree_root = NULL, *t_node = NULL;

	assert( pd->width > 3);
	assert( pd->height > 3);

	fprintf( stderr, "\n          get pattern measure ...");

	/* offsets	*/
	d[0] = -1;		/* A	*/
	d[1] =    - (long)pd->width;	/* B	*/
	d[2] = -1 - (long)pd->width;	/* C	*/
	d[3] = +1 - (long)pd->width;	/* D	*/
	d[4] = -2;										/* E	*/
	d[5] =    - 2*(long)pd->width;	/* F	*/


	numOfElements = pd->channel;

	lfdNr2 = 0;
	CALLOC( tree_root, 1, C_TREE_NODE_2);
	if (ERRCODE == MALLOC_FAILED)		goto endfunc;

	/* generate trees
	* path from root (= A value ) to leave (==F) determines a certain pattern
	* number of trees is equal to number of distinct values for A in this component
	* number of leaves is equal to number of distinct patterns in this component
	*/
	tree_root->next_item = NULL;
	tree_root->next_level = NULL;
	for ( i = 0; i < numOfElements; i++)
	{
		tree_root->val[i] = 0;
	}
	tree_root->count = 0;
	tree_root->top_hist_elem = NULL;
	tree_root->lfdNr = lfdNr2;
	lfdNr2++;

	/* reset depth of tree aka list length	*/
	for ( i = 0; i < MAX_PATTERN_SIZE; i++)
	{
		maxLevelDepth[i] = 0;
	}

	/* filling the tree */
	patternMeasure = 0;
	for ( r = 2, py = 2*pd->width; r < pd->height; r++, py+= pd->width)
	{
		for ( c = 2, pos = py + 2; c < pd->width-1; c++, pos++)
		{
			/* create tree counting of different contexts	*/
			for ( i = 0; i < numOfElements; i++)
			{
				A[i] = pd->data[i][pos + d[0]];
			}

			t_node = tree_root; /* start at top	A */

			/* search for the right A node	*/
			match_flag = 0; /* init, if while loop is not entered	*/
			depth = 0;
			while( t_node->next_item != NULL) /* scan list for matching node	*/
			{	/* scan items for match	*/
				t_node = t_node->next_item;
				depth++;
				match_flag = 1;
				for ( i = 0; i < numOfElements; i++)
				{
					if (t_node->val[i] != (unsigned)A[i])
					{
						match_flag = 0; /* does not match	*/
						break; /* exit for loop	*/
					}
				}
				if ( match_flag) break;  /* exit while loop	*/
			}
			if (match_flag)
			{
				t_node->count++; /* match found	*/
			}
			else /* no match could be found, allocate new item	*/
			{
				t_node = create_next_item_2( t_node, A, numOfElements);
				depth++;
			}
			if (maxLevelDepth[0] < depth) maxLevelDepth[0] = depth;

			/* go the levels down	*/
			/* B */
			for ( i = 0; i < numOfElements; i++)
			{
				B[i] = pd->data[i][pos + d[1]];
			}
			depth = 0;
			t_node = next_level_node_( t_node, B, numOfElements, &depth);
			if (maxLevelDepth[1] < depth) maxLevelDepth[1] = depth;

			if (pattern_size > 2)
			{
				/* C */
				for ( i = 0; i < numOfElements; i++)
				{
					C[i] = pd->data[i][pos + d[2]];
				}
				depth = 0;
				t_node = next_level_node_( t_node, C, numOfElements, &depth);
				if (maxLevelDepth[2] < depth) maxLevelDepth[2] = depth;

				/* D */
				for ( i = 0; i < numOfElements; i++)
				{
					D[i] = pd->data[i][pos + d[3]];
				}
				depth = 0;
				t_node = next_level_node_( t_node, D, numOfElements, &depth);
				if (maxLevelDepth[3] < depth) maxLevelDepth[3] = depth;
			}

			if (pattern_size > 4)
			{
				/* E */
				for ( i = 0; i < numOfElements; i++)
				{
					E[i] = pd->data[i][pos + d[4]];
				}
				depth = 0;
				t_node = next_level_node_( t_node,E, numOfElements, &depth);
				if (maxLevelDepth[4] < depth) maxLevelDepth[4] = depth;

				/* F */
				for ( i = 0; i < numOfElements; i++)
				{
					F[i] = pd->data[i][pos + d[5]];
				}
				depth = 0;
				t_node = next_level_node_( t_node, F, numOfElements, &depth);
				if (maxLevelDepth[5] < depth) maxLevelDepth[5] = depth;
			}
			/* now fill histogramm of this pattern X */
			/* F */
			for ( i = 0; i < numOfElements; i++)
			{
				X[i] = pd->data[i][pos];
			}
			update_histogram_( t_node, X, numOfElements, &patternMeasure);

			*maxDepth = /* maxLevelDepth[0] + */maxLevelDepth[1] + maxLevelDepth[2] +
				maxLevelDepth[3] + maxLevelDepth[4] + maxLevelDepth[5];
			/* slightly larger threshold than in codec_e.c*/
			//if (*maxDepth > 20000  || 
			//	 (numOfColours > 50000 && maxLevelDepth[0] > (numOfColours>>1))) 
			//{
			//	patternMeasure <<= 1;
			//	*maxDepth = (long)(*maxDepth * 1.3); /* uii ?? */
			if (lfdNr2 > 50000) 
			{
				/* stop processing as it is time consuming	*/
				/* assumes that depth at zero level is equal to number of colours 
				* => upscaling of the value
				*/
				double fac = 1.0 * numOfColours / maxLevelDepth[0];
				patternMeasure = (unsigned long)floor( patternMeasure * fac);
				/* can not be larger than number of pixels in image */
				patternMeasure = min( patternMeasure, pd->size);
				*maxDepth = (long)(*maxDepth * ( 1 + log( fac)  ));
				c = pd->width;
				r = pd->height; /* enforces leaving the for loops	*/
			}
		}
		fprintf( stderr, "\r%.2f%%", 100.0 * (r) / pd->height);
	}

endfunc:

	if (*maxDepth + numOfColours < 10000)
	{
		// free_tree( tree_root);
		free_tree_recursive( tree_root);
	}
	else
	{
		free_tree( tree_root);
	}

	return patternMeasure;
}

/*--------------------------------------------------------
* getJointEntropy()
* parameters:
* - The PicData of the image
* - The first color for the entropy
* - The second color for the entropy
* possible options for color1 and color2:
* - 0: red
* - 1: green
* - 2: blue
* - 3: alpha (only for images with channel >= 4)
*
* return:
* - The joint entropy with color1 and color2 of the image
*-------------------------------------------------------*/
double getJointEntropy( PicData *pd, unsigned int color1, unsigned int color2 )
{
	double jointEntropy, p;
	unsigned int i, colorUsedCount, color; //colorUsedCount is count the different color combination of color1 and color2
	/*
	* colorUsed
	* Is an array with value type uint.
	* The array includes all different color combination once (color1 * 256 + color2).
	* Example:
	* 1000000101110011 => 129 * 256 + 115 => color1: 129, color2: 115
	* Worst Case: array with 256*256 values. The picture has every color combination minimum once (256*256 different color combination).
	* Best Case: array with 1 value. The pictue has only one color combination.
	*/
	unsigned int *colorUsed = NULL;
	/*
	* colorCount
	* Is an array with key and values both of type uint.
	* The key is the color combination (0 <= key <= 256*256).
	* The value is the counter of the color combination.
	* Worst Case: 256*256 key-value-array.
	*/
	unsigned int *colorCount = NULL;
	
	//If the picture is smaller than 256*256 pixel, the worst case of colorUsed is only as high as the picture size.
	unsigned int allocate = 256 * 256;

	if (allocate > pd->size) 
	{
		allocate = pd->size;
	}
	
	//allocate ram for the two arrays (worst case)
	CALLOC_( colorUsed, allocate, uint);
	CALLOC_( colorCount, 256*256, uint);
	
	colorUsedCount = 0;
	//loop through the image
	for (i = 0; i < pd->size; i++)
	{
		//calculate the color combination (first color * 256 + second color)
		color = ((uint)(pd->data[color1][i]) << 8 ) + (uint)pd->data[color2][i];
		//test if the color was already in the picture before
		if (colorCount[color] == 0) 
		{
			//put the color to colorUsed array and add 1 to colorUsedCount if the color was not in the picture before
			colorUsed[colorUsedCount++] = color;
		}
		//add 1 to the counter of the color combination
		colorCount[color]++;
	}
	
	jointEntropy = 0.0;
	//loop through the colorUsed array
	for (i = 0; i < colorUsedCount; i++) 
	{
		//calculate the probability of the color combination (the counter of the color combination / the size of the image)
		assert( colorCount[colorUsed[i]] > 0);
		p = (double)colorCount[colorUsed[i]] / ((double)pd->size);
		//add the probability multiplied by logarithm of the probability to the base 2
		jointEntropy = jointEntropy + p * log(p) / log( 2.0);
	}
	
	//make the joint entropy positive
	if (jointEntropy < 0.0)
		jointEntropy = jointEntropy * (-1.0);
	
	//free the allocated ram
	FREE( colorUsed );
	FREE( colorCount );

	//return the joint entropy
	return jointEntropy;
}

/*--------------------------------------------------------
* getImageFeatures()
*-------------------------------------------------------*/
void getImageFeatures( PicData *pd, ImageFeatures *iF, 
											int coding_method_isPassed, 
											int coding_method)
{
	int comp;
	unsigned int syn[4];
	unsigned long maxCount;

	/* reset all values */
	for (comp = 0; comp < 4; comp++)
	{
		iF->corrcoeff[comp] = 0;
		iF->monotony[comp] = 0;
		iF->noise[comp] = 0;
	}
	iF->constance = 0;
	iF->syn_grad = 0;
	iF->syn = 0;
	iF->syn2 = 0; /* after histogram modification	*/
	iF->colourContrast = 0;
	iF->relColourContrast = 0;
	iF->colourSaturation = 0;
	iF->relColourSaturation = 0;
	iF->rgbStdDev[0] = 0;
	iF->rgbStdDev[1] = 0;
	iF->rgbStdDev[2] = 0;
	iF->maxDepth = 999;
	iF->patternMeasure = 0;

	iF->numColours = GetNumColours( pd, &maxCount, &(iF->colEntropy));
	iF->colperpix = (float)(iF->numColours) / pd->size;
	/* percentage of modal value of colors	*/
	iF->colModalPart = (unsigned int)floor( 200. * maxCount / pd->size + 0.5);

		//iF->roughness[0] = GetRoughness( pd);
		//iF->corrcoeff[0] = GetCorrelation( pd);
		//iF->monotony[0] = getMonotony( pd); 
		//get_syntheticness( pd, iF);
		//iF->noise[0] = estimateNoise( pd, iF); /* get also constance	*/
	/* monotony, syn_grad, and corrcoeff are required for determination of HXC2increaseFac */
	getCoMoSyNoiCon( pd, iF);


	//if (coding_method_isPassed == 0) /* saves some time, but properties are missing in log file	*/
	{
		//iF->colourSaturation = getColourSaturation( pd);
		//iF->colourContrast = getColourContrast( pd);
		//get_colour_variance( pd, iF);
		getColourContSatVar( pd, iF);
	}

	iF->syn = HowSynthetic( pd, syn);

	//iF->disruption = GetDisruption( pd);

	// getHaarFeatures( pd, &(iF->HaarFeatures));

	/* iF->patternMeasure is used for decision on aC->patternsize in autoconfig.c	*/
	{
		unsigned long val;
		/* must still becalled for getting iF->maxDepth	*/
//				fprintf(stderr, "\n getPatternMeasure() is disabled ##################\n\n");
	//
#ifdef _DEBUG
		val = 0;   /* takes too much time	*/
		lfdNr2 = 1;
		iF->maxDepth = 1; /* both, may not be zero, because of division somewhere	*/
		// blöd, weil iF->maxDepth zur Auswahl der Methode genutzt wird !!!!!
		fprintf( stderr, "\n getPatternMeasure() is deactivated!!!!\n");
#else
		val = getPatternMeasure( pd, &(iF->maxDepth), iF->numColours);
#endif
		/* getBlockNumPatterns based on ABCD only	*/
		//iF->patternMeasure = getBlockNumPatterns( pd, 0, pd->height-1, 0, pd->width-1);
		/* using unsigned long long array	*/
		iF->patternMeasure = getBlockNumPatterns2( pd, 0, pd->height-1, 0, pd->width-1);
		/* take higher value	*/
		if (lfdNr2 <= 50000) /* threshold in getPatternMeasure() above */
		{ /* patternMeasure has not been extrapolated in getPatternMeasure() */
			iF->patternMeasure = max( iF->patternMeasure, val);
		}
	}
	GetPatEntropy( pd, 0, pd->height-1, 0, pd->width-1,
										  iF->jointEntropy);

	/* joint entropies	*/
	//if (pd->channel >= 3 ) 
	//{
	//	iF->jointEntropy[0] = getJointEntropy( pd, 0, 1);	//Joint Entropy for red and green
	//	iF->jointEntropy[1] = getJointEntropy( pd, 1, 2);	//Joint Entropy for green and blue
	//	iF->jointEntropy[2] = getJointEntropy( pd, 0, 2);	//Joint Entropy for red and blue
	//}
	return;
}

