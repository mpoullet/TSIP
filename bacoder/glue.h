
#ifndef _GLUE_H_
#define _GLUE_H_

// Strutz #include "externpred\prediction.h"
#include "prediction.h"

/* Struktur zur Parameter�bergabe */
typedef struct
{
	PARAMETER *predparam;
	char *ctxfile;
	bool usextfile;
} CustomParam;

#endif
