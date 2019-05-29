/*****************************************************
* File..: quadtree_structure.c
* Desc..: determination of image quadrtree
* Author: T. Strutz
* Date..: 09.11.2015
* changes:
* 04.12.2015 avoid islands of SCF_VERTICAL inside SCF_PHOTO 
*					disabled again as Stage2 performance drops more than Stage 1 performance increases
* 03.06.2016 use only global array for quadree_structure no level specific
******************************************************/
#include <assert.h>
#include <math.h>
#include "codec.h"	/* for min( a, b);	*/
#include "stats.h"
#include "scf.h"	/* 	*/
#include "types.h"	/* for CALLOC */
#include "bitbuf.h"
#include "arithmetic.h"

//#define MODE_ARR /* output of mode_arr.pgm	*/

/*--------------------------------------------------------
* quadtree_structure()
*-------------------------------------------------------*/
void quadtree_structure( PicData *pd, unsigned char *mode_arr,
												int encoder_flag,
												acModell *M, acState *acSt, BitBuffer *bitbuf)
	{
		int level, top_level, bottom_level;
		unsigned char /**arr, */predMode, mode=9;
		unsigned int s_cnt, p_cnt, p2_cnt, h_cnt, v_cnt;
		unsigned int qw, off, th, th2;
		unsigned int r, c, y, x, rr, cc;
		unsigned int y_end, x_end;
		unsigned int numOfBlockColours = 0;/*, numOfBlockPatterns*/;
		unsigned long symbol;
		unsigned long pos, py;
		unsigned long pc, pr;
		QUADTREE_ELEM *quadstruc;


		ALLOC( quadstruc, 5, QUADTREE_ELEM);

		/* prepares hierarchical structure, top level depends on original image size */
		top_level = quadtree( pd, quadstruc);

		/* do the en/decoding of the quadtree structure	*/

		
		bottom_level = max( 0, top_level-1); /* allow always 64 and 32 blocks	*/
		/* process each level beginning from the top	*/
		for (level = top_level; level >= bottom_level; level--)
		{
			/* short cut to parameters at this level	*/
			qw = quadstruc[level].quad_width;
			off = qw - 1;
			th = quadstruc[level].threshold;
			th2 = quadstruc[level].threshold2;
#ifdef _DEBUG
			printf( "\n level=%d, %d-block", level, qw);
#endif

			//arr = quadstruc[level].arr; /* get pointer to array of actual level	*/
			/* r, c pr, pc point to original image data; y,x, py, pos point to quadtree arrays	*/
			for (r = 0, y = 0, pr = 0; r < pd->height; 
				r += qw, y++, pr += qw*pd->width)
			{
#ifdef _DEBUG
			printf( "\n%4d", r/qw);
#endif
				for (c = 0, x = 0, pc = pr; c < pd->width; c += qw, x++, pc+=qw)
				{
					y_end = min( r + off, pd->height - 1);
					x_end = min( c + off, pd->width - 1);

					//pp = c + r * pd->width;

					/* mode_arr has been reset by calling function;
					 * this branch is entered first time for the top level
					 */ 
					if (mode_arr[pc] == SCF_COLOURBASED || 
						mode_arr[pc] == SCF_SYNTHETIC|| 
						mode_arr[pc] == SCF_PHOTO || mode_arr[pc] == SCF_PHOTO2 || 
						level == top_level /* top level	*/)
					{
						/* determine+encode or decode decision colour-based vs. prediction	*/
						if (encoder_flag)
						{ /* check, whether block can be predicted	*/
							predMode = getBlockPredMode( pd, r, y_end, c, x_end);
							switch (predMode)
							{
							case 0: /* no */
								mode = (unsigned char)SCF_COLOURBASED;
								break;
							case 1: /* only horizontal prediction is possible	*/
								mode = (unsigned char)(SCF_HORIZONTAL);
								break;
							case 2: /* only vertical prediction is possible	*/
								mode = (unsigned char)(SCF_VERTICAL);
								break;
							case 3:
								/* if prediction is possible in both directions 
								* then prefer direction of neighbours;  
								*/
								h_cnt = v_cnt = 0; 
								if (c > 0)
								{
									if (mode_arr[pc-1] == SCF_HORIZONTAL ) h_cnt++;
									else if (mode_arr[pc-1] == SCF_VERTICAL ) v_cnt++;
								}
								if (r > 0)
								{
									if (mode_arr[pc-pd->width] == SCF_HORIZONTAL ) h_cnt++;
									else if (mode_arr[pc-pd->width] == SCF_VERTICAL ) v_cnt++;
									if (c > 0)
									{
										if (mode_arr[pc-pd->width-1] == SCF_HORIZONTAL ) h_cnt++;
										else if (mode_arr[pc-pd->width-1] == SCF_VERTICAL ) v_cnt++;
									}
									if (c < pd->width-qw)
									{
										if (mode_arr[pc-pd->width+qw] == SCF_HORIZONTAL ) h_cnt++;
										else if (mode_arr[pc-pd->width+qw] == SCF_VERTICAL ) v_cnt++;
									}
								}
								if (v_cnt >= h_cnt) mode = (unsigned char)(SCF_VERTICAL);
								else mode = (unsigned char)(SCF_HORIZONTAL);
								break;
							default:
								mode = SCF_NOTSET;
							}
	/* is this required here ?? it is only excuted for encoder !!	*/
							//for (rr = r, py = r * pd->width; rr < r+qw && rr < pd->height; 
							//	rr++, py += pd->width)
							//{
							//	for (cc = c, pos = py+c; cc < c+qw && cc < pd->width; cc++, pos++)
							//	{
							//		mode_arr[pos] = mode;
							//	}
							//}
						} /* if (encoder_flag) */

						/********************************************************
						* modelling  1
						*/
						s_cnt = p_cnt = p2_cnt = h_cnt = v_cnt = 0; /* reset distribution */ 
						if (c > 0)
						{ /* look to the left	*/
							if (mode_arr[pc-1] == SCF_SYNTHETIC) s_cnt++;
							else if (mode_arr[pc-1] == SCF_PHOTO) p_cnt++;
							else if (mode_arr[pc-1] == SCF_PHOTO2) p2_cnt++;
							else if (mode_arr[pc-1] == SCF_HORIZONTAL) h_cnt++;
							else if (mode_arr[pc-1] == SCF_VERTICAL) v_cnt++;
						}
						if (c < pd->width - qw)
						{ /* info to the right can be there if block was defined
							* to be predictive on an upper level
							*/
							if			(mode_arr[pc+qw] == SCF_HORIZONTAL)
								h_cnt++;
							else if (mode_arr[pc+qw] == SCF_VERTICAL) 
								v_cnt++;
						}
						if (r > 0)
						{ /* look up */
							if      (mode_arr[pc-pd->width] == SCF_SYNTHETIC) s_cnt++;
							else if (mode_arr[pc-pd->width] == SCF_PHOTO)			p_cnt++;
							else if (mode_arr[pc-pd->width] == SCF_PHOTO2)		p2_cnt++;
							else if (mode_arr[pc-pd->width] == SCF_HORIZONTAL) h_cnt++;
							else if (mode_arr[pc-pd->width] == SCF_VERTICAL)	v_cnt++;
							if (c > 0)
							{
								if      (mode_arr[pc-pd->width-1] == SCF_SYNTHETIC) s_cnt++;
								else if (mode_arr[pc-pd->width-1] == SCF_PHOTO)			p_cnt++;
								else if (mode_arr[pc-pd->width-1] == SCF_PHOTO2)		p2_cnt++;
								else if (mode_arr[pc-pd->width-1] == SCF_HORIZONTAL) h_cnt++;
								else if (mode_arr[pc-pd->width-1] == SCF_VERTICAL)	v_cnt++;
							}
							if (c < pd->width - qw)
							{
								if      (mode_arr[pc-pd->width+qw] == SCF_SYNTHETIC) s_cnt++;
								else if (mode_arr[pc-pd->width+qw] == SCF_PHOTO)			p_cnt++;
								else if (mode_arr[pc-pd->width+qw] == SCF_PHOTO2)			p2_cnt++;
								else if (mode_arr[pc-pd->width+qw] == SCF_HORIZONTAL) h_cnt++;
								else if (mode_arr[pc-pd->width+qw] == SCF_VERTICAL)	v_cnt++;
							}
						}
						if (r < pd->height-qw)
						{ /* look down, this info is based on the upper level			 */
							if      (mode_arr[pc+qw*pd->width] == SCF_SYNTHETIC)	s_cnt+=1;
							else if (mode_arr[pc+qw*pd->width] == SCF_PHOTO)			p_cnt+=1;
							else if (mode_arr[pc+qw*pd->width] == SCF_PHOTO2)			p2_cnt+=1;
							else if (mode_arr[pc+qw*pd->width] == SCF_HORIZONTAL)	h_cnt+=1;
							else if (mode_arr[pc+qw*pd->width] == SCF_VERTICAL)  	v_cnt+=1;
							if (c > 0)
							{
								if      (mode_arr[pc-1+qw*pd->width] == SCF_SYNTHETIC) s_cnt+=1;
								else if (mode_arr[pc-1+qw*pd->width] == SCF_PHOTO)		 p_cnt+=1;
								else if (mode_arr[pc-1+qw*pd->width] == SCF_PHOTO2)		 p2_cnt+=1;
								else if (mode_arr[pc-1+qw*pd->width] == SCF_HORIZONTAL)h_cnt+=1;
								else if (mode_arr[pc-1+qw*pd->width] == SCF_VERTICAL)  v_cnt+=1;
							}
							if (c < pd->width -qw)
							{
								if      (mode_arr[pc+qw+qw*pd->width] == SCF_SYNTHETIC) s_cnt+=1;
								else if (mode_arr[pc+qw+qw*pd->width] == SCF_PHOTO)			p_cnt+=1;
								else if (mode_arr[pc+qw+qw*pd->width] == SCF_PHOTO2)		p2_cnt+=1;
								else if (mode_arr[pc+qw+qw*pd->width] == SCF_HORIZONTAL)h_cnt+=1;
								else if (mode_arr[pc+qw+qw*pd->width] == SCF_VERTICAL)  v_cnt+=1;
							}
						}
						M->K = 2; /* SCF_COLOURBASED or predictive	*/
						M->H[1] = s_cnt + p_cnt + p2_cnt;
						M->H[2] = h_cnt + v_cnt;
						create_cumulative_histogram( M->H, M->K);

						if (encoder_flag)
						{
							if (mode == SCF_COLOURBASED) symbol = 0;
							else symbol = 1;
							/* do the arithmetic encoding	*/
							encode_AC( M, acSt, symbol, bitbuf);
						}
						else
						{
							/* decode, whether the block can be predicted	*/
							symbol = decode_AC( M, acSt, bitbuf);
						}
#ifdef _DEBUG
			printf( ", symbol=%d culm.distr(%d,%d)", symbol, M->H[1], M->H[2]);
#endif


						if (symbol) /* predictive, decision vertical vs. horizontal	*/
						{
							if (c == 0)
							{/* first col ==> cannot be horizontal mode	*/
								mode = SCF_VERTICAL;
							}
							else if (r == 0)
							{/* first col ==> cannot be horizontal mode	*/
								mode = SCF_HORIZONTAL;
							}
							else
							{
								M->K = 2; /* vertical or horizontal	*/
								M->H[1] = h_cnt;
								M->H[2] = v_cnt;
								create_cumulative_histogram( M->H, M->K);
								if (encoder_flag)
								{
									if (mode == SCF_HORIZONTAL) symbol = 0;
									else symbol = 1;
									/* do the arithmetic encoding	*/
									encode_AC( M, acSt, symbol, bitbuf);
								}
								else /* decode	*/
								{
									symbol = decode_AC( M, acSt, bitbuf);
									if (symbol == 0)	mode = SCF_HORIZONTAL;
									else							mode = SCF_VERTICAL;
								}
							}
#ifdef _DEBUG
			printf( " pred-sym=%d culm.distr(%d,%d)", symbol, M->H[1], M->H[2]);
#endif
						}
						else /* symbol == 0	*/
						{
							/* SCF_COLOURBASED, decision synthetic vs. photo
							 * skip if max number of colours in a block is smaller than colour threshold for 64x64-block
							 * but not in top_level
							 */
							{								 
								/* SCF_COLOURBASED, decision synthetic vs. photo
								 * skip if upper level has already indicated a colour based mode
								 */
								if (mode_arr[pc] != SCF_SYNTHETIC)
								{ /* accept SCF_SYNTHETIC decision from upper level and skip
									 * otherwise check, whether SCF_PHOTO can be split in smaller blocks */
									M->K = 2; /* synthetic, photo or photo2	*/
									M->H[1] = s_cnt;
									M->H[2] = p_cnt + p2_cnt;
									create_cumulative_histogram( M->H, M->K);

									if (encoder_flag)
									{
										numOfBlockColours = getBlockNumColours( pd, r, y_end, c, x_end);
										if ( numOfBlockColours > th2)
										{
											mode = SCF_PHOTO;
										}
										else
										{
											mode = SCF_SYNTHETIC;
										}
										/* number of colours corresponds to probability of exception
										 * handling symbol (ESC) which is manipulated in stage 1 and stage 2
										 * the number of patterns can be based on few different colours in
										 * different arrangements
										 * ==> similarity of patterns could be based on common colours
										 */
										//numOfBlockPatterns = getBlockNumPatterns( pd, r, y_end, c, x_end);
										//if ( numOfBlockPatterns <= th)	arr[pos] = SCF_SYNTHETIC;
										//else	
										//	arr[pos] = SCF_PHOTO;

										if (mode == SCF_SYNTHETIC) symbol = 0;
										else  symbol = 1;
										/* do the arithmetic encoding	*/
										encode_AC( M, acSt, symbol, bitbuf);
									}
									else /* decode	*/
									{
										symbol = decode_AC( M, acSt, bitbuf);
										if (symbol == 0)
										{
											mode = SCF_SYNTHETIC;
										}
										else 
										{
											mode = SCF_PHOTO;
										}
									}
#ifdef _DEBUG
			printf( " col-sym=%d culm.distr(%d,%d),", symbol, M->H[1], M->H[2]);
#endif
									if (symbol) /* kind of PHOTO	*/
									{
										M->K = 2; /* photo or photo2	*/
										M->H[1] = p_cnt;
										M->H[2] = p2_cnt;
										create_cumulative_histogram( M->H, M->K);
										if (encoder_flag)
										{
											if ( numOfBlockColours > th)
											{
												mode = SCF_PHOTO;
											}
											else
											{
												mode = SCF_PHOTO2;
											}
											if (mode == SCF_PHOTO) symbol = 0;
											else  symbol = 1;
											/* do the arithmetic encoding	*/
											encode_AC( M, acSt, symbol, bitbuf);
										}
										else /* decode	*/
										{
											symbol = decode_AC( M, acSt, bitbuf);
											if (symbol == 0)
											{
												mode = SCF_PHOTO;
											}
											else 
											{
												mode = SCF_PHOTO2;
											}
										}
									}
									if (mode == SCF_PHOTO )
									{
										/* the idea is do find finer blocks that can be copied	*/
										bottom_level = max( 1, level-1); /* include next smaller level	*/
			// printf( " P%d", level);
									}
								} /* if (mode_arr[pc] !*/
								else
								{ /* copy colour mode from upper level	*/
									mode = mode_arr[pc];
#ifdef _DEBUG
	//		printf( ", mode %d copied from upper level", arr[pos]);
#endif
								}
							} /* if ( numOfImageColours >= th)*/
						}

						for (rr = r, py = r * pd->width; rr < r+qw && rr < pd->height; 
							rr++, py += pd->width)
						{
							for (cc = c, pos = py + c; cc < c+qw && cc < pd->width; cc++, pos++)
							{
								mode_arr[pos] = mode;
							}
						}
					}
					else /* not SCF_PHOTO, not SCF_SYNTHETIC, not SCF_COLOURBASED */
					{ /* SCF_HORIZONTAL or SCF_VERTICAL*/
						/* pass prediction mode from upper level	*/
						//arr[pos] = mode_arr[pc] | SCF_PASSED_FLAG; /* add passed flag	*/
						// mode_arr[pc] = mode_arr[pc] | SCF_PASSED_FLAG; /* add passed flag	*/
#ifdef _DEBUG
	//					printf( ", mode %d copied from upper level", arr[pos]);
#endif
					}

				}/* for (c = 0, */
			} /* for (r = 0,*/
#ifdef MODE_ARR
			{
				FILE *out=NULL;
				char filename[512];
				unsigned int i;


				sprintf( filename, "mode_arr_%d_%d.pgm", encoder_flag,level);
				out = fopen( filename, "wt");
				fprintf( out,"P2\n");
				fprintf( out,"# created by %s\n", thisVersion);
				fprintf( out,"%d %d\n", pd->width, pd->height);
				fprintf( out,"%d", 255);

				for ( i = 0; i < pd->size; i++) 
				{
					if ((i % pd->width) == 0)		fprintf( out, "\n");
					fprintf( out,"%3d ", mode_arr[i]*30);
				}
				fprintf( out,"\n");
				fclose( out);
			}
#endif 
		} /* for ( level = 0*/


#ifdef _DEBUG_NIXXXX
		{
			FILE *out=NULL;
			char filename[512];
			unsigned int i;
			for (level = top_level; level >= bottom_level; level--)
			{
				sprintf( filename, "arr%0d.pgm", level);
				out = fopen( filename, "wt");
				fprintf( out,"P2\n");
				fprintf( out,"# created by %s\n", thisVersion);
				fprintf( out,"%d %d\n", quadstruc[level].width, quadstruc[level].height);
				fprintf( out,"%d", SCF_PASSED_FLAG + SCF_PHOTO);

				for ( i = 0; i < quadstruc[level].size; i++) 
				{
					if ((i % quadstruc[level].width) == 0)		fprintf( out, "\n");
					fprintf( out,"%5d ", quadstruc[level].arr[i]);
				}
				fprintf( out,"\n");
				fclose( out);
			}
		}
#endif

		/* free allocated levels	*/
		//for ( level = 0; level <= top_level; level++)
		//{
		//	FREE( quadstruc[level].arr);
		//}
		FREE( quadstruc);
	}