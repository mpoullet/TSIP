/**********************************************************************
 * File...: flipExchange.c
 * Purpose: flipExchange algorithm
 * Author.: Tilo Strutz
 * Date:..: 25.02.2015
 * changes:
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

int *put_segment(int N, int *vector, int pos, int *segment, int len_seg);
int *get_segment(int N, int *vector, int first, int last, int reverse_flag, 
								 int *len_seg);

/*----------------------------------------------------
 * flipExchange_alg()
 *----------------------------------------------------*/
int flipExchange_alg( int inN, int **C, int *permutation_v)
{
	int res = 0;
	int N = inN;
	// initial permutation
	int *pv, i, last_position, last_flipExchange_pos, max_segmentLength, stop_2opt, stop_cityMove, stop_flipExchange;
	int cnt_doubleFlip, cnt_leftFlipExchange, cnt_rightFlipExchange, cnt_pureExchange, pos, ii;

	pv = (int*) malloc((N+1) * sizeof(int));
	for (i = 1; i <= N; i++)
	{
		pv[i] = permutation_v[i];
	}
	last_position = 1; 
	last_flipExchange_pos = last_position;
	max_segmentLength = N;
	stop_2opt = 0;
	stop_cityMove = 0;
	stop_flipExchange = 0;
	while ((stop_2opt != 1) || (stop_cityMove  != 1) || (stop_flipExchange != 1))
	{
		int modified_by_2opt_or_cityMove = 0;
		while ((stop_2opt != 1)  || (stop_cityMove != 1))
		{
			while (stop_2opt != 1)
			{
				int pos, ii;

				// initial permutation is given
				// substitute two edges with shorter ones
				// start with longest edge
				stop_2opt = 1;
				pos = (int)((last_position/2) + 1);

				for(ii = pos; ii<= N + pos-1; ii++)
				{
					int i = ((ii-1) % N) + 1;
					// get cities at end of this edge
					int c = pv[i];
					int c1 = pv[(i % N) + 1];
					int dist_c = C[c][c1];
					int change_flag = 0;
					// for all other edges, starting from j
					int j;
					for(j = 1; j <= N; j++)
					{
						int d, d1, dist_d, dist_cd, dist_c1d1;

						if (j == i)
						{
							continue;
						}
						// get cities at end of this edge
						d = pv[j];
						d1 = pv[(j % N) + 1];
						dist_d = C[d][d1];
						dist_cd = C[c][d];
						dist_c1d1 = C[c1][d1];
						if ((dist_cd + dist_c1d1) < (dist_c + dist_d))
						{
							int idx_j, idx, mid, k;
							// remember the current position as next method
							// should start in this vicinity
							last_position = i;
							// do the change
							modified_by_2opt_or_cityMove = 1;
							change_flag = 1;
							stop_2opt = 0;// another loop could be benefitial
							idx_j;
							idx;   
							if (j < i)
							{
								idx_j = i;
								idx = j;
							}
							else
							{
								idx_j = j;
								idx = i;
							}
							// reverse order of segment
							mid = (int)((idx_j - idx) / 2 );

							for(k = 1; k<= mid; k++)
							{
								int tmp = pv[idx+k];
								pv[idx+k] = pv[idx_j+1-k];
								pv[idx_j+1-k] = tmp;
							}
						}
						if (change_flag == 1)
						{					
							// essential: if change was successful, go to next i
							break;
						}
					} // for j = 1:N
				} // for i
				if (stop_2opt == 0)
				{
					stop_flipExchange = 0;
					stop_cityMove = 0; // another 2-opt loop could be benefitial
				}
			} // while stop_2opt
			while (stop_cityMove != 1)
			{
				int pos, ii;
				stop_cityMove = 1;
				// initial permutation is given
				// exchange two cities if the path will shortened
				pos = (int)((last_position/2) +1);

				for(ii = pos; ii<= N + pos-1; ii++)
				{
					int i = ((ii-1) % N) + 1;
					int c = pv[i];
					int c1 = pv[((i-2 + N) % N) + 1];  // left neighbour
					int c2 = pv[(i % N) + 1];        // right neighbour
					// removal of city i
					// add new segment betwee neighbours
					// subtract distances between current city and neighbours
					int distChange1 = C[c1][c2] - C[c1][c] - C[c2][c];
					int change_flag = 0;
					// for all edges, starting from j
					int j;
					for(j = 1; j<= N; j++)
					{
						int d1, d2, distChange2, sumChange;
						d1 = pv[j];
						if((d1 == c1) || (d1 == c))
						{
							continue; // avoid same (possible new) segment
						}
						d2 = pv[(j % N) + 1]; // right neighbour
						// add city i at position  j
						distChange2 =  C[d1][c] + C[d2][c] - C[d1][d2];
						sumChange = distChange1 + distChange2;
						if (sumChange < 0)
						{     
							// remember the current position as next method
							// should start in this vicinity
							last_position = i;
							// do the move
							modified_by_2opt_or_cityMove = 1;
							change_flag = 1;
							stop_cityMove = 0;
							if (j < i)
							{
								// shift
								int k;
								for(k = i;k >= j+2; k--)
								{
									pv[k] = pv[k-1];
								}
								pv[j+1] = c;
							}
							else
							{
								// shift 
								int k;
								for(k = i; k<=j-1; k++)
								{
									pv[k] = pv[k+1];
								}
								pv[j] = c;
							}
						}
						if (change_flag == 1)
						{					
							break;
						}  
					} // for j
				} // for i
				if (stop_cityMove == 0)
				{
					stop_2opt = 0; // another 2-opt loop could be benefitial
					stop_flipExchange = 0; // another 2-opt loop could be benefitial
				}      
			} // while ~stop_cityMove
		} // while ~stop_cityMove || stop_2opt



		cnt_doubleFlip = 0;
		cnt_leftFlipExchange = 0;
		cnt_rightFlipExchange = 0;
		cnt_pureExchange = 0;
		// initial permutation is given
		// substitute two edges with shorter ones
		// start with longest edge
		stop_flipExchange = 1;

		if (modified_by_2opt_or_cityMove == 1)
		{
			// there was a modification with 2opt or cityMove
			// go some steps back	
			pos = (int)(4*last_position/8 +1);
		}
		else
		{
			// last modification bay flip& exchange
			// continue at last position
			pos = last_flipExchange_pos + 1;
		}

		for(ii = pos; ii <= N + pos-2; ii++)
		{							// start pos of 1st segment
			int i = ((ii-1) % N) + 1;
			// get cities at end of this edge
			int c = pv[i];
			int c1 = pv[(i % N) + 1];
			int dist_c = C[c][c1];
			int change_flag = 0;
			int jj;
			for(jj = i+2; jj<= i+max_segmentLength; jj++)
			{
				int j, d, d1, dist_d, dist_cd, dist_c1d1, endpos, kk;

				j = ((jj-1) % N) + 1;
				if (j == i)
				{
					continue;
				}
				// get cities at end of this edge
				d = pv[j];
				d1 = pv[(j % N) + 1];
				dist_d = C[d][d1];
				dist_cd = C[c][d];
				dist_c1d1 = C[c1][d1];
				// try two-segment modification (flip & exchange)
				endpos = min( N+1-1, jj + max_segmentLength);
				// investigate segments of at least 2 cities

				for(kk = jj+2; kk<= endpos; kk++)
				{	
					int k, e, e1, dist_e, dist_c1e, dist_c1e1, dist_d1e1, dist_de1, dist_cd1, dist_ce, dist_de;
					int dist_doubleFlip, dist_leftFlipExchange, dist_rightFlipExchange, dist_pureExchange, dist_best;
					// end pos of 2nd segment
					k = ((kk-1) % N) + 1;
					if ((k == i) || (k == j))
					{
						continue;
					}
					e = pv[k];
					e1 = pv[(k % N) + 1];
					dist_e = C[e][e1];
					dist_c1e = C[c1][e];
					dist_c1e1 = C[c1][e1];
					dist_d1e1 = C[d1][e1];
					dist_de1 = C[d][e1];
					dist_cd1 = C[c][d1];
					dist_ce = C[c][e];
					dist_de = C[d][e];

					dist_doubleFlip = dist_cd + dist_c1e + dist_d1e1;
					dist_leftFlipExchange = dist_cd1 + dist_de + dist_c1e1;
					dist_rightFlipExchange = dist_ce + dist_c1d1 + dist_de1;
					dist_pureExchange = dist_cd1 + dist_c1e + dist_de1;

					dist_best = min( dist_doubleFlip, dist_leftFlipExchange);
					dist_best = min( dist_best, dist_rightFlipExchange);
					dist_best = min( dist_best, dist_pureExchange);
					if (dist_best < (dist_c + dist_d + dist_e))
					{
						// remember the current position as next method
						// should start in this vicinity
						change_flag = 1;
						last_flipExchange_pos = i;
						if (dist_doubleFlip == dist_best)
						{
							int len_seg1, *seg1, len_seg2, *seg2;
							// double flip
							cnt_doubleFlip = cnt_doubleFlip + 1;
							stop_flipExchange = 0;// another loop could be benefitial
							// copy of 1st segment in reverse order
							len_seg1 = 0;
							seg1 = get_segment( N, pv,(i % N)+1, j, 1, &len_seg1);
							// copy of 2nd segment in reverse order
							len_seg2 = 0;
							seg2 = get_segment( N, pv,(j % N)+1, k, 1, &len_seg2);
							pv = put_segment( N, pv,(i % N)+1, seg1, len_seg1);
							pv = put_segment( N, pv,(j % N)+1, seg2, len_seg2);
						}
						else
						{
							if (dist_leftFlipExchange == dist_best)
							{
								int len_seg1, *seg1, len_seg2, *seg2;
								// left flip + exchange
								cnt_leftFlipExchange = cnt_leftFlipExchange + 1;
								stop_flipExchange = 0;// another loop could be benefitial
								// copy of 1st segment in reverse order
								len_seg1 = 0;
								seg1 = get_segment( N, pv,(i % N)+1, j, 1, &len_seg1);
								// copy of 2nd segment in native order
								len_seg2 = 0;
								seg2 = get_segment( N, pv,(j % N)+1, k, 0, &len_seg2);
								pv = put_segment( N, pv,(i % N)+1, seg2, len_seg2);
								pv = put_segment( N, pv,((i + len_seg2) % N)+1, seg1, len_seg1);
							}
							else
							{
								if (dist_rightFlipExchange == dist_best)
								{
									int len_seg1, *seg1, len_seg2, *seg2;
									// right flip + exchange
									cnt_rightFlipExchange = cnt_rightFlipExchange + 1;
									stop_flipExchange = 0;// another loop could be benefitial
									// copy of 1st segment in native order
									len_seg1 = 0;
									seg1 = get_segment( N, pv,(i % N)+1, j, 0, &len_seg1);
									// copy of 2nd segment in reverse order
									len_seg2 = 0;
									seg2 = get_segment( N, pv,(j % N)+1, k, 1, &len_seg2);
									pv = put_segment( N, pv,(i % N)+1, seg2, len_seg2);
									pv = put_segment( N, pv,((i + len_seg2) % N)+1, seg1, len_seg1);
								}
								else
								{
									if (dist_pureExchange == dist_best)
									{
										int len_seg1, *seg1, len_seg2, *seg2;
										//pur exchange
										cnt_pureExchange = cnt_pureExchange + 1;
										stop_flipExchange = 0;// another loop could be benefitial
										// copy of 1st segment in native order
										len_seg1 = 0;
										seg1 = get_segment( N, pv,(i % N)+1, j, 0, &len_seg1);
										// copy of 2nd segment in native order
										len_seg2 = 0;
										seg2 = get_segment( N, pv,(j % N)+1, k, 0, &len_seg2);
										pv = put_segment( N, pv,(i % N)+1, seg2, len_seg2);
										pv = put_segment( N, pv,((i + len_seg2) % N)+1, seg1, len_seg1);
									}
								}
							}
						}
					}
					if (change_flag == 1)
					{
						// essential: if change was successful, go to next i
						// because c1, d, d1, e are not uptodate anymore
						break;
					}
				} // for k = 1:N
				if (change_flag == 1)
				{
					// essential: if change was successful, go to next i
					// because c1, d, d1, e are not uptodate anymore
					break;
				}
			} // for j = 1:N
			if (change_flag == 1)
			{
				// switch back to faster ops
				break;
			}
		} // for i
		if (stop_flipExchange == 0)
		{
			stop_2opt = 0; // another 2-opt loop could be benefitial
			stop_cityMove = 0; // another 2-opt loop could be benefitial
		}
		stop_flipExchange = 1; // only one loop through flip&exchange
	} // while ~stop_flipExchange || stop_2opt || ~stop_flipExchange
	for (i = 1; i <= N; i++)
	{
		permutation_v[i] = pv[i];
	}
	free( pv);
	return res;
}
