/**********************************************************************
 * File...: utils.c
 * Purpose: utils for (TSP)
 * Author.: Tilo Strutz
 * Date:..: 10.01.2014
 * changes:
 * 23.12.2014 transcription from Matlab to C by PVL team BonnerBrothers bM_IKT_14
 * 25.02.2015 Strutz
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

/*----------------------------------------------------
 * strtrim()
 * remove leading whitespaces
 *----------------------------------------------------*/
char *strtrim (char *ptr)
{
  while( (*ptr == ' ') || (*ptr == '\t') || *ptr == '\r' || *ptr == '\n')
	{
		ptr++;
	}
  return ptr;
}


/*----------------------------------------------------
 * get_euclidean_dist()
 *----------------------------------------------------*/
int get_euclidean_dist( float x1, float y1, float x2, float y2) 
{
	int distance;
	double dx2x1;
	double dy2y1;

	dx2x1 = x2 - x1;
	dy2y1 = y2 - y1;

	distance = (int)floor( sqrt( dx2x1 * dx2x1 + dy2y1 * dy2y1) + 0.5);

	//printf("x1: %i\n", x1);
	//printf("y1: %i\n", y1);
	//printf("x2: %i\n", x2);
	//printf("y2: %i\n", y2);

	return distance;
}

/*----------------------------------------------------
 * matrix_befuellen()
 *----------------------------------------------------*/
void matrix_befuellen(int N, float **matrix, int **d_matrix) 
{
	int temp, zeile, spalte;    
	zeile=0;
	spalte=0;

	while(zeile < N) 
	{
		while(spalte < N) 
		{           
			temp = get_euclidean_dist( matrix[zeile][1], matrix[zeile][2], matrix[spalte][1], matrix[spalte][2]);          
			d_matrix[zeile+1][spalte+1] = temp;
			d_matrix[spalte+1][zeile+1] = temp;
			spalte++;
		}
		zeile++;
		spalte = zeile+1;
	}     
}

/*----------------------------------------------------
 * datei_auslesen()
 *----------------------------------------------------*/
void datei_auslesen( float **matrix, char *path, int N) 
{
	FILE *datei_matrix_befuellen; 
	//anzahl knoten
	int schreibe; //schreibetempvariable
	char puf[80], *puffer;
	char *start_string = "NODE_COORD_SECTION\n";
	char *eof = "EOF\n";
	char *y_coord, *x_coord; /* wrong names	*/
	char *token = NULL;
	char *next_token = NULL;
	
	//FILE Pointer setzen
	datei_matrix_befuellen = fopen( path, "rt");
	schreibe = 0;

	//Datei wird Zeilenweise eingelesen
	while(!feof(datei_matrix_befuellen)) 
	{
		if (fgets( puf, 80, datei_matrix_befuellen) == NULL)
		{
			fprintf( stderr, "###  early end of file, check data! ###\n");
			break; 
		}

		puffer = strtrim( puf); /* remove white space	*/
		//Überprüfe auf String "EOF" -> dann Schleife beenden
		if (strcmp( eof, puffer) == 0)
			break;

		if (schreibe == 1) 
		{
			token = strtok_s( puffer, " ", &next_token); /* read running number	*/
			//String puffer nach " " unterteilen und \n am Ende entfernen
			x_coord = strtok_s( NULL, " \t", &next_token);
			y_coord = strtok_s( NULL, " \t\n\r", &next_token); /* gets next token from puffer	*/
			//y_coord = strtok_s( y_coord, " "); /* removes white spaces at end	*/
			//y_coord = strtok( y_coord, "\t");
			//y_coord = strtok( y_coord, "\r");
			//y_coord = strtok( y_coord, "\n");

			//wenn anfang mit 1 benötigt wird dann num_city+1 schreiben
			//und rückgabestring in integer wandeln und in matrix speichern
			//matrix[num_city][0] = (float)num_city;
			//matrix[num_city][1] = (float)atof( x_coord);
			//matrix[num_city][2] = (float)atof( y_coord);

			//num_city++;
		}

		//Überprüfe ob Koordinaten starten
		//steht hinter der Schleife, damit "NORD_COORD_... nicht eingelesen wird
		if (strcmp( start_string, puffer) == 0)
		{
			int i; 
			for (i = 0; i < N; i++)
			{
				fgets( puf, 80, datei_matrix_befuellen); 
				token = strtok_s( puffer, " ", &next_token); /* read running number	*/
				x_coord = strtok_s( NULL, " \t", &next_token);
				y_coord = strtok_s( NULL, " \t\n\r", &next_token); /* gets next token from puffer	*/
				matrix[i][0] = (float)i;
				matrix[i][1] = (float)atof( x_coord);
				matrix[i][2] = (float)atof( y_coord);
			}
			break;
		}
	}

	fclose( datei_matrix_befuellen);
}

/*----------------------------------------------------
 * get_numberOfCities()
 * quick & dirty code for counting coordinates
 *----------------------------------------------------*/
int get_numberOfCities( char *path) 
{
	FILE *datei_anzahl_ermitteln;
	int num_city, schreibe;
	int edge_flag = 0;
	int dim_flag = 0;
	int coord_flag = 0;
	char *start_string = "NODE_COORD_SECTION\n";
	char *edge_string = "EDGE_WEIGHT_TYPE";
	char *edge_type = "EUC_2D";
	char *eof = "EOF\n";
	char puf[80], *puffer;
	char *token = NULL;
	char *next_token = NULL;


	datei_anzahl_ermitteln = fopen( path, "rt");
	//anzahl knoten
	num_city = 0;
	//tempvaribable
	schreibe = 0;

	//Datei öffnen und auf Erfolg überprüfen
	if (!datei_anzahl_ermitteln) 
	{
		printf("Cannot open file\n");
		return 0;
	}
	//printf("file open succesfull\n"); 

	//Anzahl der Knoten ermitteln
	while (!feof(datei_anzahl_ermitteln)) 
	{
		//Zeilenweise einlesen
		fgets( puf, 80, datei_anzahl_ermitteln);  
		puffer = strtrim( puf); /* remove white space	*/

		//Überprüfe auf String "EOF" -> dann Schleife beenden
		if (strcmp( eof, puffer) == 0)
			break;

		/* check EDGE_WEIGHT_TYPE, must be equal to EUC_2D */
		token = strtok_s( puffer, " :\t\n\r", &next_token); /* get first token */
		if (strcmp( edge_string, token) == 0)
		{
			token = strtok_s( NULL, " :\t\r\n", &next_token); /* next token, remove whitespaces	an colon */
			if (strcmp( edge_type, token) != 0)
			{
				fprintf( stderr, "## wrong %s: %s", edge_string, token);
				num_city = 0;
				break;
			}
			else edge_flag = 1;
		}
		
		/* check DIMENSION, get number of cities */
		else if (strcmp( "DIMENSION", token) == 0)
		{
			token = strtok_s( NULL, " :\t\r\n", &next_token); /* next token, remove whitespaces	an colon */
			num_city = atoi( token);
			dim_flag = 1;
		}

		/* check NODE_COORD_SECTION	*/
		else if (strcmp( "NODE_COORD_SECTION", token) == 0)
		{
			coord_flag = 1;
		}

		/* XXX check number of cities using "DIMENSION" XXXX	*/
		/* check proper edge mode	*/
		if (0 && schreibe == 0)
		{
			token = strtok_s( puffer, " :", &next_token); /* get first token */
			if (strcmp( edge_string, token) == 0)
			{
				token = strtok_s( NULL, " :\t\r\n", &next_token); /* next token, remove whitespaces	an colon */
				if (strcmp( edge_type, token) != 0)
				{
					fprintf( stderr, "## wrong %s: %s", edge_string, token);
					num_city = 0;
					break;
				}
			}
			//Überprüfe ob Koordinaten starten
			//steht hinter der Schleife, damit "NORD_COORD_... nicht eingelesen wird
			if (strcmp( start_string, puffer) == 0)
				schreibe = 1; 
		}

		if (dim_flag && edge_flag && coord_flag) break;
	}

	fclose( datei_anzahl_ermitteln);

	return num_city;
}
