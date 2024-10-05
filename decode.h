#ifndef DECODE_H
#define DECODE_H
#include "types.h"

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DecodeInfo
{
	/* Source Information */
	char *source_image_fname;
	FILE *fptr_source_image;
    
	/* Output Information */
	char output[10];
	FILE *fptr_output;
}DecodeInfo;

/* Encoding function prototype */

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status files_open(DecodeInfo *decInfo);

/* Magic string decoding*/
Status decode_magic_string(FILE *fptr_source_image);

/* Decode from lsb to byte*/
Status decode_lsb_to_byte(char *imagedata, char *decodedstring);

/* Decode secret file Extension size */
Status decode_Secret_file_Extension_size(uint *extension_size, FILE *fptr_source_image);

/* Decode Extension size from lsb */
uint decode_Extension_size_from_lsb(unsigned char *temparr);

/* Decode secret file extension */
Status decode_secret_file_extension(DecodeInfo *decInfo, char *extension, uint extension_size, FILE *fptr_source_image);

/* Decode secret file size */
Status decode_secret_file_size(uint *secret_size, FILE *fptr_source_image);

/* Decode secret file data */
Status decode_secret_file_data(char *filedata, uint *secret_size, FILE *fptr_source_image); 

#endif
