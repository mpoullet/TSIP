/*****************************************************************
 *
 * File........:	prototypes.h
 * Function....:	proto typing for different functions
 * Author......:	Tilo Strutz
 * last changes:	27.01.2010, 30.3.2011
 *
 * LICENCE DETAILS: see software manual
 *	free academic use
 *  cite source as 
 * "Strutz, T.: Data Fitting and Uncertainty. Vieweg+Teubner, 2010"
 *
 *****************************************************************/

#ifndef PROTO_H
#define PROTO_H


typedef struct {
	int linear;			/* linear model */
	int svd;				/* special computation for linear models */		
	int LM;					/* 0 .. Gaus-Newton, 1 .. Levenberg-Marquardt	*/
	int chisq_target;	/* indicates that 'chisq_target' was set	*/
	int trueH;				/* use true Hessian matrix	*/
} LS_FLAG;

/* least squares routine */
int
ls( double (*funct) (int,double*,double*), 
	  double (*funct_deriv) (double(*)(int,double*,double*), 
					int,int,int,double*,double*), 
	  double (*funct_deriv2) (double(*)(int,double*,double*), 
	  int,int,int,int,double*,double*), 
		int (*init)(int, double*,double*,double*,
						unsigned char*,FILE*),
		int N, int M, double *obs, double *cond, double **jacob,
		double *weights, double *a, unsigned char* a_flag,
		int algo_mode, LS_FLAG *ls_flag,
		double chisq_target, double **covar, FILE *out);

int svd_inversion( int N, double **normal, double **normal_i, 
									FILE *out);
int IsFiniteNumber(double x);
int
solve_lin( int N, int M, double *obs, double **weights, 
							double **jacob, double **covar, double *a, 
							FILE *out);

/* parsing of command-line parameters */
char* get_nth_field( char *line, int n);
int is_data_line( char *line, int N);

/* matrix inversion */
int	singvaldec( double **a, int N, int M, double w[], double **v);
void	backsub_LU( double **lu, int N, int *indx, double back[]);
int	decomp_LU( double **normal, int M, int *indx, int *s);
void heap_sort_d_(unsigned long N, double ra[], int idx[]);
void heap_sort_d(unsigned long N, double ra[]);

/* estimation of weights */
void est_weights1( int N, double *deltasq, 
					double *weights, FILE *out);
void est_weights2( int N, double *cond, double *obs, 
							double *weights, int obs_per_bin, FILE *out);
int outlier_detection1( int N, double sigma_y, double *deltasq, 
					double *weights, double nu, FILE *out);
int outlier_detection2( int N, double *deltasq, 
					double *weights, FILE *out);
int outlier_detection3( int N, double *deltasq, 
					double *weights, double nu, FILE *out);

int
ransac( double (*funct) (int,double*,double*), 
	 double (*funct_deriv) (double(*)(int,double*,double*), 
	 int,int,int,double*,double*), 
	 double (*funct_deriv2) (double(*)(int,double*,double*), 
	 int,int,int,int,double*,double*), 
	 int (*init)(int, double*,double*,double*,
	 unsigned char*,FILE*),
	 int N, int M, double *obs, double *cond, double **jacob,
	 double *weights, double *a, unsigned char* a_flag,
	 int algo_mode, LS_FLAG *ls_flag,
	 double chisq_target, double **covar, FILE *out,
	 double *deviates_abs,
	 int cond_dim,
	 int obs_dim);

	 void
ls_straightline( 
	int N, /* number of entries */
	double cond[], /* vector of conditions */
	double obs[],		/* vector of observations */
	double a[]			/* container for parameters to be estimated */
	);

#endif
