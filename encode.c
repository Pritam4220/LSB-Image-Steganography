#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>
//Here i declare Extension_size to zero
uint Extension_size = 0;
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_src_image)
{
	uint width, height;
	// Seek to 18th byte
	fseek(fptr_src_image, 18, SEEK_SET);

	// Read the width (an int)
	fread(&width, sizeof(int), 1, fptr_src_image);
	printf("width = %u\n", width);

	// Read the height (an int)
	fread(&height, sizeof(int), 1, fptr_src_image);
	printf("height = %u\n", height);

	// Return image capacity
	return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status open_files(EncodeInfo *encInfo)
{
	// Src Image file
	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
	// Do Error handling
	if (encInfo->fptr_src_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

		return e_failure;
	}

	// Secret file
	printf("%s\n", encInfo -> secret_fname);
	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
	// Do Error handling
	if (encInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

		return e_failure;
	}

	// Stego Image file
	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
	// Do Error handling
	if (encInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

		return e_failure;
	}

	// No failure return e_success
	return e_success;
}


Status do_encoding(EncodeInfo *encInfo) 
{
	//step1 : call openfiles(encInfo)
	if(open_files(encInfo) == e_success)
	{
		printf("INFO : files are opened successfully\n");
	}
	else
	{
		printf("INFO : Files are not opened successfully\n");
		return e_failure; 
	}
    //step2 : call checkCapacity
	if(check_capacity(encInfo) == e_success)
	{
		printf("INFO : Capacity is available for Encoding\n");
	}
	else
	{
		printf("INFO : Capacity is not available for Encoding\n");
	}
    //step3 : call copybmpheader function 
	if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		printf("INFO : BMP header of source image will be copied\n");
	}
	else
	{
		printf("INFO : BMP header of source image will be copied\n");
	}
	//step4 : call encodemagicstring function
	if (encode_magic_string(MAGIC_STRING, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		printf("INFO : MAGIC_STRING will be Encoded\n");
	}
	else
	{
		printf("INFO : MAGIC_STRING not Encoded\n");
	}

	//step5 : call encode_secret_file_extension_size function
	if(encode_secret_file_extension_size(Extension_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		printf("INFO : Exten size successfully Encode\n");
	}
	else
	{
		printf("INFO : Exten size failed to  Encode\n");
	}
	//Here i use strstr to find the extension
	char *exten = strstr(encInfo->secret_fname, ".");
	//step 6 : call encode_secret_file_extn function
	if(encode_secret_file_extn(exten, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		printf("INFO : Secret file Extension successfully Encoded\n");
	}
	else
	{
		printf("INFO : Secret file Extension failed to Encoded\n");
	}
	//step7 : call encode_secret_file_size function
	if(encode_secret_file_size(encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		printf("INFO : Secret file size successfully Encoded\n");
	}
	else
	{
		printf("INFO : Secret file size failed to Encoded\n");
	}
	//step8 : call encode_secret_file_data function
	if(encode_secret_file_data(encInfo->fptr_secret, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
		printf("INFO : secret file data successfully completed\n");
	}
	else
	{
		printf("INFO : secret file data failed to encoded\n");
	}
	//step9 : Atlast i call the copy_remaining_img_data to copy all remaining data
	if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo) == e_success)
	{
		printf("INFO : Remaining data will be copied successfully\n");
	} 
	else
	{
		printf("INFO : Remaining data will be failed to copy\n");
	}
	//Atlast encoding is completed and e_success will be return
   return e_success;	
}

//Here function to checkCapacity
Status check_capacity(EncodeInfo *encInfo)
{
	//Here we store imagesize in bitsperpixel and also here i call one function to find the image size
	encInfo->bits_per_pixel = get_image_size_for_bmp(encInfo->fptr_src_image);
	//Here we store the extension size and call one function to find sizze of Extension
	 Extension_size = size_Extension(encInfo->secret_fname);
	 //Here we store secretfilesize and we call function to find size of secret file
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    //Here we store encoding things which things i encode i store in image capacity
	encInfo->image_capacity  = (54 + (strlen(MAGIC_STRING)*8) + (Extension_size) + (Extension_size * 8) + (encInfo->size_secret_file) + ((encInfo->size_secret_file) * 8));	
    //Here we check this condition to check encoding things are encoded or not like i just check if my image capcity is high and beautifl.bmp has less size then we cant encode these things
	if (encInfo->bits_per_pixel >= encInfo->image_capacity)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}
//Here function to check size of extension
uint size_Extension(const char *secret_fname)
{
	//Here i store the extension of secret file in extn by using strstr
	const char *extn = strstr(secret_fname, ".");
	//Here i just find length of extension and we return this extension size 
	int Extension = strlen(extn);
	return Extension;
}
//Here function to find the size of file
Status get_file_size(FILE *fptr_secret)
{
	//Here i rewind the secret file to send my cursor to beginning as zeroth position
	rewind(fptr_secret);
	fseek(fptr_secret , 0 , SEEK_END);
	//Here i  store the size of secret file by using ftell because ftell tell us cursor position
	int s = ftell(fptr_secret);
	//Annd we return the s
	return s;
}
//Here function to copybmpheader
Status copy_bmp_header(FILE *fptr_src_image, FILE  *fptr_stego_image)
{
	//Here we create one char temparr to store src_image byte
	unsigned char temparr[54];
	//Here i am jsut use the rewind to move my cursor at 0th position
	rewind(fptr_src_image);
	//Here i read the 54 data at one time to fptr source image into temparr
	fread(temparr , 54, 1, fptr_src_image);
	//Here i am writing data from temparr into stegoimage file and we just check this conditon to check whether my data will be written correctly or not 
	if(fwrite(temparr , 54 , 1, fptr_stego_image) == 54)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}


//Here function to encodeMagicString
Status encode_magic_string(const char *magic_string , FILE *fptr_src_image, FILE *fptr_stego_image)
{
	//Here i am calling one function encode_data_to_image to encode Magic string we check this condition my magic string will be encoded or not
	if(encode_data_to_image(MAGIC_STRING, fptr_src_image, fptr_stego_image) == e_success)
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}
//Here function to encode_data_to_image 
Status encode_data_to_image(const char *data, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	//Here i create one image_buffer to store the image data
	char image_buffer[8];
	//Here i use the while loop to data
	while (*data)
	{
		//Here i declare one variable and store the given data
		char byte = *data;
		//Here i read the src_image data one by one into image-buffer and we check this condition to data is read or not correctly
		if (fread(image_buffer, 1 , 8, fptr_src_image) == 8)
		{
			//Here i call the function to encode data in lsb byte
			encode_byte_to_lsb(byte, image_buffer);
			//Here i am writiing one by one data from image_buffer into stego_image file
			fwrite(image_buffer, 1 , 8, fptr_stego_image);
			//Here we increment the data 
			data++;
		}
	}
	return e_success;
}

//Here function to encode_byte_to_lsb
Status encode_byte_to_lsb(char ch, char *image_buffer)
{
	//Here i use this for loop and i iterate this to 8 bit because we pass the data as 8 bits 
	for (int i=0; i<8; i++)
	{
		//clear a bit
		image_buffer[i] = image_buffer[i] & 0xFE;
		//get a bit
		char bit = (ch >> (7 - i)) & 0x01;
		//Replace a bit
		image_buffer[i] = image_buffer[i] | bit;
	}
	return e_success;
}
//function to encode_secret_file_extension_size
Status encode_secret_file_extension_size(Extension_value value, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    //Here i create one temparr of 32 bits to store data of srcImage
	unsigned char temparr[32];
	//Here we read 32 bit data to src_image into temparr
	fread(temparr, 1, 32, fptr_src_image);
    //Here we call encode_size_to_lsb
	encode_size_to_lsb(value, temparr);
	//After this we write data in stegoImage from temparr 
	fwrite(temparr, 1, 32, fptr_stego_image);
	return e_success;
}
//Here function to call encode_size_to_lsb
Status encode_size_to_lsb(int size, unsigned char *temparr)
{
	//Here this loop iterate 32 bits to the size
	for (int i=0; i<32; i++)
	{
		//Here we clear the lsb byte of temparr
		temparr[i]= temparr[i] & (-2);
        // Here we Set the LSB of the current byte to the ith bit of size.
		temparr[i] = ((size & (1 << i)) >> i) | temparr[i];
	}
	return e_success;
}
//Here functionn to encode_secret_file_extn
Status encode_secret_file_extn(char *extn, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	//Here we call the function data to image they convert this data into an image
	encode_data_to_image(extn, fptr_src_image, fptr_stego_image);
	return e_success;
}
//Here function to encode_secret_file_size
Status encode_secret_file_size(uint size_secret_file, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	//Here i store the size_secret_file into secret
	int secret = size_secret_file;
	//After that we call the function encode_secret_file_extension_size to encode the extension size
	encode_secret_file_extension_size(secret, fptr_src_image, fptr_stego_image);
	return e_success;
}
//Here function to call encode_secret_file_data
Status encode_secret_file_data(FILE *fptr_secret , FILE *fptr_src_image, FILE *fptr_stego_image)
{
	//Here i rewind the cursor position to 0th position
	rewind(fptr_secret);
	//Here we set cursor position at 0th position by using fseek
	fseek(fptr_secret, 0, SEEK_END);
	//Here i find the size_secret by using ftell
	int size_secret = ftell(fptr_secret);
	//Here we create an temp of size_secret to store the secret data
	char temp[size_secret];
	rewind(fptr_secret);
	//Here i read the size_secret data at a time from fptr_secret into temp
	fread(temp, 1, size_secret, fptr_secret);
	//And here we store the null character of temp last index
	temp[size_secret] = '\0';
    // Here in return we call the function encode data to image to convert this secret data into an image
	return encode_data_to_image(temp, fptr_src_image, fptr_stego_image);
}
//Here function to call copy_remaining_img_data
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
	//Here we declare char ch variable
	char ch;
	//Here we read the data from fptr_src_image into ch while >0 
	while(fread(&ch, 1, 1, fptr_src_image) > 0)
	{
		//And here we write in stego_image from ch
		fwrite(&ch, 1 , 1, fptr_stego_image);
	}
	//At last e_success will be return
	return e_success;
}


