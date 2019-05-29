/*******************************************************************
 * File....: adjmap.h
 * Function: Contains the functions implementing the adjacency-mapping
 *           algorithm.
 * Author..: Toia-Cret Titus-Ruben
 * Date....: 09.05.2012
 * 30.07.2012 Strutz: changes for TSIP v1.14
 ********************************************************************/

#include "plist.h"
#include "tstypes.h"
#include "ginterface.h"
#include "tstypes.h"
#include <assert.h>
#include <stdio.h>

/* Performs the adjacency-based remapping algorithm. First, the colour
 * palette is constructed, and the pixel RGB values are replaced by
 * the corresponding palette values.
 * Afterwards, the algorithm procceeds to the remapping itself, replacing
 * the map values with re-calculated values. The original map can be
 * reconstructed using only information from the palette */
uint Adjmap_perform_map(PicData* pd, uint* palette, uint numColours);
uint Adjmap_perform_map2( PicData* pd, unsigned char *palette, uint numColours);
/* Rebuilds the original image, initially reconstructing the mapped
 * pixel values and then replacing the indexes with colours, based
 * on the colour palette */
void Adjmap_perform_rebuild(PicData* pd, uint* palette, uint numColours);
void Adjmap_perform_rebuild2( PicData* pd, unsigned char* palette, 
														 uint numColours);
