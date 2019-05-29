/*********************************************************************
 *
 * Datei:	util.c
 * Funktion:	utilities for file and filename handling
 * Autor:	Tilo Strutz
 * Datum: 	31.05.2000
 *
 *********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------------------------------------------------------------------
 *
 * make_filename2()
 *
 * wenn keine Extension vorhanden, eine anhaengen
 *---------------------------------------------------------------------*/
void make_filename2( char *filename, char *extension)
{
  char *pointer1, *pointer2;

    pointer1 = strrchr( filename, '.'); 	/*  a dot somewhere ?	*/
    if (pointer1 == NULL)		/* no */
    { 						/* no extension */
			strcat( filename, ".");
			strcat( filename, extension);
    }
    else /* there is a dot	*/
    {
			pointer2 = strstr( pointer1, "./"); 	/* is it a dot in a path ?	*/
			if (pointer2 != NULL) /* yes, i.e. there is no extension */
			{ 			/* yes path but no extension 	*/
					strcat( filename, ".");
					strcat( filename, extension);
			}
			else /* no */
			{
				pointer1[1] = '\0'; /* cut extension */
				strcat( filename, extension); /* append new extension	*/
			}
    }
}
/*---------------------------------------------------------------------
 *
 * remove_extension()
 *
 * remove extension if present
 *---------------------------------------------------------------------*/
void remove_extension( char *filename)
{
  char *pointer1, *pointer2;

  pointer1 = strrchr( filename, '.'); 	/*  a dot somewhere ?	*/
  if (pointer1 != NULL)		/* yes */
  {
		pointer2 = strstr( pointer1, "./"); 	/* is it a dot in a path ?	*/
		if (pointer2 == NULL) /* no, it is an extension */
		{
			pointer1[0] = '\0'; /* cut extension */
		}
  }
}
