
#ifndef _GLUE_H_
#define _GLUE_H_

// Strutz #include "externpred\prediction.h"
#include "prediction.h"

/* Struktur zur Parameterübergabe */
typedef struct
{
	PARAMETER *predparam;
	char *ctxfile;
	bool usextfile;
} CustomParam;

#endif
