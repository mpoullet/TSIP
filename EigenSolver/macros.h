/*****************************************************************
 *
 * File........:	macros.h
 * Function....:	general purpose macros
 * Author......:	Tilo Strutz
 * last changes:	25.01.2010
 *
 * LICENCE DETAILS: see software manual
 *	free academic use
 *  cite source as 
 * "Strutz, T.: Data Fitting and Uncertainty. Vieweg+Teubner, 2010"
 *
 *****************************************************************/

#ifndef MACROS_H
#define MACROS_H

#define MAX(a,b) ((a) > (b) ?   (a) : (b))
#define MIN(a,b) ((a) < (b) ?   (a) : (b))

#define M_PI        3.14159265358979323846       /* pi */
#define TOL 1.0e-10		/* for convergence criterion */
#define TOL_S2 1.0e-14		/* for test of singular values */
#define TOL_S 1.0e-12		/* for test of singular values */
	/* this small value is required, for example, 
			for the Bennett5 data set */

#endif
