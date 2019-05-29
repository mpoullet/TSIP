/****************************************************************
 *
 * File........:	get_option.h
 * Function....:	prototyping etc. for get_option.c
 * Author......:	Tilo Strutz
 * last changes:	30.03.2006
 *
 * LICENCE DETAILS: see software manual
 *	free academic use
 *  cite source as 
 * "Strutz, T.: Data Fitting and Uncertainty. Vieweg+Teubner, 2010"
 *
 ****************************************************************/

#ifndef GETOPT_H
#define GETOPT_H

/* defined in get_option.c */
extern char *OptArg;
extern char CheckStr[256];	
/* CheckStr will be initialised with NEEDEDOPTIONS,
 * all used optionen are deleted;
 * if at least one option remains, an error message is output
 */
extern char *optstr;
/* can be used in main file to point to filenames */
extern int opt_num; /* is defined in get_option.c */

/* defined in usage.c */
extern char *title;
extern char *OPTIONSTRING;
extern char *NEEDEDOPTIONS;

/* Prototyping	*/
void usage( int argc, char *argv[]);
int check_opt(int argc, char *argv[]);
char *get_option( int argc, char *argv[], int *opt_num);

#endif
