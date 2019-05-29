/*****************************************************
* File..: check_run_mode.c
* Desc..: for SCC, testing of structures
* Author: T. Strutz
* Date..: 09.11.2015
******************************************************/
#include <assert.h>
#include <math.h>
#include "stats.h"
#include "codec.h" /* for max()	*/
#include "scc.h"	/* 	*/
#include "types.h"	/* for CALLOC */


/*---------------------------------------------------------
* check_run_mode()
* 
*---------------------------------------------------------*/
unsigned char check_run_mode( PicData *pd, 
															 unsigned int r, unsigned int r)
{
	unsigned char predHor_flag, predVer_flag;
	unsigned int y, x, comp;
	unsigned long pos, py, posH, posV;

					{
						int match_flag, match_cnt;
						unsigned int V[4], Vl[4], Vr[4], H[4];
						unsigned int x, y;

						match_cnt = 0;

						x = max( 0, (signed)c-1);	/* one column	to the left*/
						y = max( 0, (signed)r-1); /* one row up, keep inside array	*/
						for( comp = 0; comp < pd->channel; comp++)
						{
							Vl[comp] = pd->data[comp][x + y * pd->width];
						}
						match_flag = 1;
						y = max( 0, (signed)r-2); /* one row up, keep inside array	*/
						for( comp = 0; comp < pd->channel; comp++)
						{
							if (Vl[comp] != pd->data[comp][x + y * pd->width])
							{
								match_flag = 0; break;
							}
						}
						if (match_flag) match_cnt++;

						x = min( pd->width-1, c+1);	/* one column	to the right*/
						y = max( 0, (signed)r-1); /* one row up, keep inside array	*/
						for( comp = 0; comp < pd->channel; comp++)
						{
							Vr[comp] = pd->data[comp][x + y * pd->width];
						}
						match_flag = 1;
						y = max( 0, (signed)r-2); /* one row up, keep inside array	*/
						for( comp = 0; comp < pd->channel; comp++)
						{
							if (Vr[comp] != pd->data[comp][x + y * pd->width])
							{
								match_flag = 0; break;
							}
						}
						if (match_flag) match_cnt++;

						x = c;	/* same column	*/
						y = max( 0, (signed)r-1); /* one row up, keep inside array	*/
						for( comp = 0; comp < pd->channel; comp++)
						{
							V[comp] = pd->data[comp][x + y * pd->width];
						}
						match_flag = 1;
						y = max( 0, (signed)r-2); /* two rows up, keep inside array	*/
						for( comp = 0; comp < pd->channel; comp++)
						{
							if (V[comp] != pd->data[comp][x + y * pd->width])
							{
								match_flag = 0; break;
							}
						}
						if (match_flag) match_cnt++;

						if (match_cnt == 3)
						{ /* possible vertical structure	*/
							/* avoid flat regions	*/
							match_cnt = 0;
							for( comp = 0; comp < pd->channel; comp++)
							{
								if (V[comp] != Vl[comp])
								{
									match_cnt = 3; break;
								}
								if (V[comp] != Vr[comp])
								{
									match_cnt = 3; break;
								}
							}
						} /* if (match_cnt == 3)*/

						if (match_cnt == 3)
						{ /* possible vertical structure	*/
							//M.K = 2; /* prepare distribution	*/
							//M.H[1] = verticalHit_true;
							//M.H[2] = verticalHit_false;
							//create_cumulative_histogram( M.H, M.K);

							//if (encoder_flag)
							//{
							//	match_flag = 1;
							//	/* compare to current position	*/
							//	for( comp = 0; comp < pd->channel; comp++)
							//	{
							//		if (V[comp] != pd->data[comp][c + r * pd->width])
							//		{
							//			match_flag = 0; break;
							//		}
							//	}
							//	encode_AC( &M, acSt, match_flag, bitbuf);
							//}
							//else
							//{
							//	match_flag = decode_AC( &M, acSt, bitbuf);
							//	if (match_flag)
							//	{ /* copy colour from position above	*/
							//		for( comp = 0; comp < pd->channel; comp++)
							//		{
							//			pd->data[comp][c + r * pd->width] = V[comp];
							//		}
							//	}
							//}
							//if (match_flag) verticalHit_true++;
							//else	verticalHit_false++;
							//if (verticalHit_true + verticalHit_false > 1000)
							//{ /* downscaling	*/
							//	verticalHit_true >>= 1;
							//	verticalHit_false >>= 1;
							//}
							//if (match_flag) continue; /* skip other processing	*/
						} /* if (match_cnt == 3)*/

}

