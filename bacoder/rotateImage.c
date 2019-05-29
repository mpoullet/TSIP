/*****************************************************
* File..: rotateImage.c
* Desc..: rotates the image according to passed rotateMode 
* Author: Phillip Möller
* Date..: 15.05.2018
* Descr. rotateMode == 0 --> 00 degree (no rotation)
*		 rotateMode == 1 --> 90 degree (90 degree rotation)	
*		 rotateMode == 2 --> 180 degree (180 degree rotation)
*		 rotateMode == 3 --> 270 degree (270 degree rotation)
*		 for decoder rotateMode is adapted, e.g. if 
*		 image was rotated while encoding by 90 degree (rotateMode = 1), 
#		 it has to be rotated 3 times back (rotateMode = 3)
******************************************************/
#include "scf.h"
#include "ginterface.h"
#include "pdconvert.h"

void rotateImage( PicData *pd, unsigned int rotateMode, int encodeDecodeFlag){


		unsigned int comp = 0;
		unsigned int r, c;
		unsigned long pos, pos_new;

		if(encodeDecodeFlag == 0){
		//adjuste rotateMode for undoing of rotation of encoder
			if(rotateMode == 1){
				rotateMode = 3;
			}

			if(rotateMode == 3){
				rotateMode = 1;
			}
		}

		if(rotateMode != 0){

		for(int i = 0; i < (signed)rotateMode; i++){

		PicData *pdNew = CopyPicData(pd);

		long heightOld = pd->height;
		long widthOld = pd->width;
		long heightNew = widthOld;
		long widthNew = heightOld;

		pdNew->height = heightNew;
		pdNew->width = widthNew;

		for (comp = 0; comp < pd->channel; comp++){
			for (r = 0; r < (unsigned)heightNew; r++){
				for (c = 0; c < (unsigned)widthNew; c++){
					pos = c + r * widthNew;

					long cNeu = c + 1;
					if( cNeu < heightOld){
						pos_new = r + ( (heightOld*widthOld) - cNeu*widthOld) ;
					}
					else{
						pos_new = c;
					}
					pdNew->data[comp][pos]= pd->data[comp][pos_new];

				}
			}
		}

		pd->width = widthNew;
		pd->height = heightNew;

		for (comp = 0; comp < pd->channel; comp++){
			for (r = 0; r < pd->height; r++){
				for (c = 0; c < pd->width; c++){
					pos = c + r * pd->width;
					pd->data[comp][pos]= pdNew->data[comp][pos];
				}
			}
		}

		fprintf(stderr, "\n\n rotation done!");

		}
	 }

}