/****************************************************************
*
* Datei:	get_option.c
* Funktion:	Einlesen der Kommandozeilen-parameter
* Autor:	Tilo Strutz
* Datum: 	31.05.2000
*
* 06.05.2013 do not stop at arguments w/o "-"
****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char	*OPTIONSTRING;
extern char	*NEEDEDOPTIONS;

/* enthaelt Argument der Option, falls kein Arg ==NULL	*/
char	*OptArg=NULL;

char	CheckStr[256]; /* wird mit NEEDEDOPTIONS initialisiert
										 * alle benutzte Optionen werden geloescht;
										 * wenn eine Option uebrig bleibt,
										 * erfolgt Fehlermeldung
										 */
/* Prototyping	*/
void usage( int argc, char *argv[]);

/*---------------------------------------------------------------
*	check_opt()
*---------------------------------------------------------------*/
void check_opt( int argc, char *argv[])
{
	char	*ptr;
	int	i, len, err=0;

	len = (int)strlen( CheckStr);
	for( i = 0; i < len; i++) 
	{
		if ( (CheckStr[i]!=':') && (CheckStr[i]!=' ') )
		{
			ptr = &CheckStr[i];
			ptr = (char*)strpbrk( ptr, ";:");
			ptr[0] = '\0';
			err = 1;
			break;
		}
	}
	if (err) 
	{
		fprintf(stderr, "\n Missing Option for (-%s)!", &CheckStr[i]);
		usage( argc, argv);
	}
}

/*---------------------------------------------------------------
*	get_option()
*
*	opt_num gibt die Nummer der zu lesenden Option
*	Rueckgabe: Option-String 
* 	erforderlich: globaler String der alle Optionen enth"alt
* 	beim ersten Aufruf sollte opt_num gleich 1 sein!
*
*---------------------------------------------------------------*/
char* get_option( int argc, char *argv[], int *opt_num)
{
	char	optstr[256], *ptr, c, d, string[256];
	int	len, i, num;
	static flag = 1;

	if (flag) 
	{
		flag = 0;
		strcpy( CheckStr, NEEDEDOPTIONS);
	}
	if (*opt_num>(argc-1)) 
	{
		return( NULL);
	}
	else if (argv[*opt_num][0] != '-') 
	{
		fprintf(stderr, "\n Option-Error !!  *********** ");
		fprintf(stderr, "\n every Option must start with '-' (%s)!", 
			argv[*opt_num]);
		return NULL; /* end of arguments	*/
		//usage( argc, argv);
	}

	/***** kopiert ohne '-'	*****/
	num = *opt_num;
	strcpy( optstr, argv[num]);
	strcpy( string, &optstr[1]);

	len = (int)strlen( string);
	if (len == 0) 
	{ /****** einzelner '-'	******/
		fprintf( stderr, "\n Option-Error !!  *********** ");
		fprintf( stderr, "\n lonely dash !");
		usage( argc, argv);

	}
	ptr = OPTIONSTRING;
	do
	{
		/* Optionstring in OPTIONSTRING suchen	*/
		ptr = (char*)strstr( ptr, string);
		if (ptr == NULL) 
		{
			fprintf(stderr, "\n Option-Error !!  *********** ");
			fprintf(stderr, "\n Unknown Option (%s)!", optstr);
			usage( argc, argv);
		}
		c = ptr[len];		/* Nachfolgendes Zeichen merken	*/
		d = ptr[-1];		/* Vorgaenger merken 	*/

		/* diesen Eintrag ueberspringen, indem nach dem naechsten
		* : oder ; gesucht wird
		*/
		ptr = (char*)strpbrk( ptr, ";:");
	} while ( ((c!=':') && (c!=';')) || ((d!=':') && (d!=';')) );


	if (c==';')			/* Info ob Argument folgt)	*/
	{
		OptArg = NULL; 
		(*opt_num)++;
	}
	else 
	{
		(*opt_num)++;
		if (*opt_num>(argc-1))
		{
			fprintf(stderr, "\n Option-Error !!  *********** ");
			fprintf(stderr, "\n Missing Argument for (%s)!", optstr);
			usage( argc, argv);
		}
		else if (argv[*opt_num][0]=='-')
		{
			fprintf(stderr, "\n Option-Error !!  *********** ");
			fprintf(stderr, "\n Missing Argument for (%s)!", optstr);
			usage( argc, argv);
		}

		OptArg = argv[*opt_num];
		(*opt_num)++;
	}
	strcpy( string, ":");
	strcat( string, &optstr[1]);
	strcat( string, ":");
	ptr = (char*)strstr( CheckStr, string);
	if (ptr!=NULL) for( i=0; i<len; i++) ptr[i+1] = ' ';

	return( argv[num]);
}
