/*****************************************************************
 *
 * File........:	errmsg.h
 * Function....:	error messages
 * Author......:	Tilo Strutz
 * last changes:	25.01.2010, 1.4.2011
 *
 * LICENCE DETAILS: see software manual
 *	free academic use
 *  cite source as 
 * "Strutz, T.: Data Fitting and Uncertainty. Vieweg+Teubner, 2010"
 *
 *****************************************************************/

#ifndef ERRMSG_H
#define ERRMSG_H


#define ERR_CALL	1
#define ERR_CALL_MSG	"\n### %s: Wrong command-line parameters. \n  %s\n"
#define ERR_OPEN_READ	2
#define ERR_OPEN_READ_MSG	"\n### %s: Cannot open %s for reading\n"
#define ERR_OPEN_WRITE		3
#define ERR_OPEN_WRITE_MSG	"\n### %s: Cannot open %s for writing\n"
#define ERR_ALLOCATE		4
#define ERR_ALLOCATE_MSG		"\n### %s: Cannot allocate %s\n"
#define ERR_NOT_DEFINED	5
#define ERR_NOT_DEFINED_MSG	"\n### %s: Value %d for %s is not defined\n"
#define ERR_IS_ZERO	6
#define ERR_IS_ZERO_MSG	"\n### %s: Value for %s is zero\n"
#define ERR_IS_SINGULAR	7
#define ERR_IS_SINGULAR_MSG	"\n### %s: Matrix %s is singular\n"
#define ERR_IS_INFINITE	8
#define ERR_IS_INFINITE_MSG	"\n### %s: Variable %s is infinite\n"


int errmsg( int err, char *rtn, char *text, int value);

#endif
