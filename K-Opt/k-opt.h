/**********************************************************************
 * File...: k-opt.h
 * Purpose: heuristic for optimisation problem 
 * Author.: Tilo Strutz,
 * Date:..: 18.05.2015
 * changes:
 ***********************************************************************/

#define min(a, b) (((a) < (b)) ? (a) : (b))

/*** Speicher belegen und Rückgabe prüfen ***/
#ifndef CALLOC
#define CALLOC(_ptr, _num, _type)								\
{																								\
	_ptr = (_type *)calloc((_num), sizeof(_type));\
	if (_ptr == NULL)															\
	{																							\
		fprintf(stderr, "calloc failed for %s %s\n", #_num, #_type);	\
	}																							\
}
#endif
/*** Speicher freigeben ***/
#ifndef FREE
#define FREE(_ptr)														\
{																					\
	if (_ptr != NULL) free(_ptr);						\
	_ptr = NULL;														\
}
#endif


unsigned long get_costs( int N, int *C, int *pv);
void k_opt_heuristic( int *permu_v, int optim_mode, int N, int *C, 
										int *duration, unsigned long *bestCosts, int *inbest_trial, 
										int *innumTrials);
unsigned long two_opt_heuristic( int N, int *C, int *pv, unsigned long bestCosts);

void put_segment( /*int N,*/ int *vector, int pos, int *segment, 
								 int len_seg, int reverse_flag);
void get_segment( /*int N,*/ int *vector, int pos, int *segment, 
								 int len_seg);
int get_seg_length( int N, int first, int last);
void randperm( int N, int *randPerm);
