/*****************************************************************
 *
 * File........:	errmsg.c
 * Function....:	error messages
 * Author......:	Tilo Strutz
 * last changes:	20.10.2007, 1.4.2011
 *
 * LICENCE DETAILS: see software manual
 *	free academic use
 *  cite source as 
 * "Strutz, T.: Data Fitting and Uncertainty. Vieweg+Teubner, 2010"
 *
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <float.h> /* for DBL_MAX */
#include "errmsg.h"

int
errmsg( int err, char *rtn, char *text, int value)
{
	switch (err)
	{
		case ERR_CALL:
			fprintf( stderr, ERR_CALL_MSG, rtn, text);
			break;
		case ERR_OPEN_READ:
			fprintf( stderr, ERR_OPEN_READ_MSG, rtn, text);
			perror( "\nReason");
			break;
		case ERR_OPEN_WRITE:
			fprintf( stderr, ERR_OPEN_WRITE_MSG, rtn, text);
			perror( "\nReason");
			break;
		case ERR_ALLOCATE:
			fprintf( stderr, ERR_ALLOCATE_MSG, rtn, text);
			perror( "\nReason");
			break;
		case ERR_NOT_DEFINED:
			fprintf( stderr, ERR_NOT_DEFINED_MSG, rtn, value, text);
			break;
		case ERR_IS_INFINITE:
			fprintf( stderr, ERR_IS_INFINITE_MSG, rtn, text);
			break;
		case ERR_IS_ZERO:
			fprintf( stderr, ERR_IS_ZERO_MSG, rtn, text);
			break;
		case ERR_IS_SINGULAR:
			fprintf( stderr, ERR_IS_SINGULAR_MSG, rtn, text);
			break;
		default:
			fprintf( stderr, "\nerrmsg: error %d is not defined\n", err);
			break;
	}
	return err;
}

/*---------------------------------------------------------------
 *	testing values()
 * from http://www.johndcook.com/IEEE_exceptions_in_cpp.html
 *--------------------------------------------------------------*/
int IsNumber(double x) 
{
    // This looks like it should always be true, 
    // but it's false if x is a NaN.
    return (x == x); 
}
int IsFiniteNumber(double x) 
{
    return (x <= DBL_MAX && x >= -DBL_MAX); 
}
