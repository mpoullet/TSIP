/*****************************************************
* File...: ifc.h
* Descr..: incremental Frequency Count
* Author.: Tilo Strutz
* Date...: 01.08.2012 
******************************************************/

#ifndef _IFC_H_
#define _IFC_H_

#include "tstypes.h"

/* WFC anwenden und rückwandeln */
void IFCencode(uint *, uint, uint reset_val, uint max_count);
void IFCdecode(uint *, uint, uint reset_val, uint max_count);


#endif

