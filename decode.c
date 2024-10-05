#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>
//Here i type defined the unsigned int to uint
typedef unsigned int uint;
//Here i declare the extension size as  Globally because we have more places to use it
uint extension_size;
//Here i declare the char array 
char extension[5];
//Here declare some varaible
uint secret_size;
char secret[100];


/* Function for open the files*/
Status files_open(DecodeInfo *decInfo)
{
	//Here i open the source image as stego image in read mode to decode
	decInfo->fptr_source_image = fopen(decInfo->source_image_fname, "r");
	//This condition for checking Error
	if(decInfo->fptr_source_image == NULL)
	{
		//Here error will be printed in stderr
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->source_image_fname);
		return e_failure;
	}
	return e_success;
}

// function for decoding
// Decoding started
Status do_decoding(DecodeInfo *decInfo)
{	
	//Here i call the filesopen function to open files at a time i check my files are correctly open or not
	if(files_open(decInfo) == e_success)
	{
		printf("INFO : files are opened successfully\n");
	}
	else
	{
		printf("INFO : files are not opened successfully\n");
	}
	//Here i call decoding the magic string and we check error
	if(decode_magic_string(decInfo->fptr_source_image) == e_success)
	{
		printf("INFO : MAGIC_STRING are decoded Successfully\n");
	}
	else
	{
		printf("INFO : MAGIC_STRING are failed to decode\n");
	}
	//Here i call  decode secret file extension size and we check error
	if(decode_Secret_file_Extension_size(&extension_size,decInfo->fptr_source_image) == e_success)
	{
		printf("INFO : Secret_file_Extension_size are decoded successfully %d\n",extension_size);
	}
	else
	{
		printf("INFO : Secret_file_Extension_size are failed to decoded\n");
	}
	//Here i call the decode secret file extension and we check error
	if (decode_secret_file_extension(decInfo,extension,extension_size, decInfo->fptr_source_image) == e_success)
	{
		printf("INFO: Extension successfully decoded: %s\n", extension);
	}
	else
	{
		printf("INFO : Failed to decode extension\n");
	}
	//Here i call the decode secret file size and we check error
	if (decode_secret_file_size(&secret_size, decInfo->fptr_source_image) == e_success)
	{
		printf("INFO : Secret_file_size are decoded successfully %d\n",secret_size);
	}
	else
	{
		printf("INFO : Secret_file_size are failed to decoded");
	}
	//Here i  call decode secret file data and we check error
	if (decode_secret_file_data(secret,&secret_size, decInfo->fptr_source_image) == e_success)
	{
		printf("INFO : secret file data are decoded successfully %s\n",secret);
		//Here after decoding secret file data we need to write decode secret in output.txt
		fwrite(secret, secret_size, 1, decInfo->fptr_output);
        //After writing i close the file
        fclose(decInfo->fptr_output);
	}
	else
	{
		printf("INFO : secret file data are failed to decoded");
	}
	//Here i close the source file
	fclose(decInfo->fptr_source_image);

	return e_success;

}

//Here function for decode magic string
Status decode_magic_string(FILE *fptr_source_image)
{
	//Here i set my cursor position on 54 because 54 bits are header of bmp after header we decode magic string 
	fseek(fptr_source_image,54,SEEK_SET);
	//Here i create onne character array where we store the data
	char imagedata[16];
	//Here we read the 16 data at a time from source image into imagedata and we check this is correctly read or not in if condition 
	if(fread(imagedata, 1, 16, fptr_source_image) != 16)
	{
		printf("Failed to reading data\n");
		return e_failure;
	}
	//Here we create one character array of size 3 because we know i have 2 byte magic string and one is NULL character
	char decodedstring[3];
	//Here i use for loop to decode 8 bits at a time and we increment i as i+8 for this reason
	for (int i = 0; i < 16; i += 8)
	{
		//Here in every iteration we call decode lsb to byte function and after decoding we store decodedstring[i/8] here i use this for indexing position in 1st iter 1st magic string is on 0th pos
		if (decode_lsb_to_byte(&imagedata[i], &decodedstring[i / 8]) != e_success)
		{
			return e_failure;
		}
	}
	//And here at last index we store NULL character
	decodedstring[2] = '\0';
	//Here i just print my decoded magic string 
	printf("DEBUG: Decoded string: %s\n", decodedstring); // Add debug print
    
	//Here i compare decoded magic string and original encoded magic string if both are equal that time my decoding is correct by using strcmp
	if (strcmp(decodedstring, MAGIC_STRING) == 0)
	{
		printf("INFO: MAGIC_STRING decoded correctly: %s\n", decodedstring);
		return e_success;
	}
	//Here in else part we just print the error
	else
	{
		printf("ERROR: MAGIC_STRING decoding failed. Decoded string: %s\n", decodedstring);
		return e_failure;
	}
}

//Here function for decode lsb to bye
Status decode_lsb_to_byte(char *imagedata, char *decodechar)
{
	//here we innitialize ch value as zero we use this ch value because everytime after get a bit we need to shift this to lsb 
	char ch=0;
	//Here we use for loop for extracting the bits
	for(int i=0; i<8; i++)
	{
		//Here we extract he lsb bit by using get a bit
		char bit = (imagedata[i] & 0x01);  
		//After getting we replace this by using this logic
		ch = ch | (bit << (7-i));   
	}
	//Here i just print decoded things
	printf("Decoded Character: %c\n", ch);  
	*decodechar = ch;
	return e_success;
}

//Here function for decode secret file extension size
Status decode_Secret_file_Extension_size(uint *extension_size, FILE *fptr_source_image)
{
	//Here we create one temp arr of 32 bits and we store the sextension size in temparr
	unsigned char temparr[32];
	//Here i read 32 bits at a time from sourceimage into temparr and also we check error condition 
	if(fread(temparr, 1, 32, fptr_source_image) != 32)
	{
		printf("INFO : Not readed secret file extension\n");
		return e_failure;
	}
	//Here i call the decode extension size from lsb after decoding we store this on extension size
	*extension_size = decode_Extension_size_from_lsb(temparr);
	//Here i just prinnt the decode things
	printf("DEBUG: Decoded extension size: %u\n", *extension_size);
	return e_success;
}
//Here function for decode extension size from lsb
uint decode_Extension_size_from_lsb(unsigned char *temparr)
{
	//Here i initialize size as 0
	uint size=0;
	//Here i use this for loop to extract the bit
	for(int i=0; i<32; i++)
	{
		// Extract the bit and add it to the size
		size = size | ((temparr[i] & 1) << i);
	}
	return size;
}

//Here function for decode secret file extension
Status decode_secret_file_extension(DecodeInfo *decInfo, char *extension, uint extension_size, FILE *fptr_source_image)
{
	//Here i just set my cursor position where my extension is encoded
	fseek(fptr_source_image,102, SEEK_SET);
	//Here i create one character array of 32 size
	unsigned char Extn[32];
	//Here in if condition i read the data from fptr source image into Extn and check error
	if(fread(Extn, 1, 32, fptr_source_image) != 32)
	{
		printf("INFO : Reading the byte fail\n");
		return e_failure;
	}
	//Here this loop is used for every iteration 8-8 bis extracting 
	for (int i = 0; i < 32; i += 8)
	{
		//Here i call decode lsb to byte to decode from lsb and here i store index wise in extension[i/8]
		if(decode_lsb_to_byte(&Extn[i], &extension[i/8]) != e_success)
		{
			return e_failure;
		}
	}
	//Here at last size i store the NULL character
	extension[5] = '\0';
	//Here i  just print the decoded things
	printf("DEBUG: Decoded string: %s\n", extension);

//Here we use strcpy because strcpy syntax is char *strcpy(char *dest, const char *src) my second argument is always non modifiable for this reason i used this to convert my output as Non modifiable
	strcpy(decInfo->output, "out");
	//After this i concatenated my extension out.txt
	strcat(decInfo->output,extension);
	//Here i open my out.txt to write our decode password in out.txt in write mode
	decInfo->fptr_output = fopen(decInfo->output, "w");
	//Here we check error
	if(decInfo->fptr_output == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output);
		return e_failure;
	}

	return e_success;
}

//Here function for decode secret file size
Status decode_secret_file_size(uint *secret_size, FILE *fptr_source_image) 
{
	//Here we create one char tmp array
	unsigned char tmp[32];
	//Here i read the 32 bits at a time from source image into tmp and also check error 
	if(fread(tmp, 1,32, fptr_source_image) != 32)
	{
		printf("INFO : Not readed secret file size\n");
		return e_failure;
	}
	//Here i call decode extension size from lsb after decoding secret file size we store this on secret size
	*secret_size = decode_Extension_size_from_lsb(tmp);
	//Here i print the decoded things
	printf("DEBUG: Decoded file size: %u\n", *secret_size);
	return e_success;
}

//Here function for decode secret file data
Status decode_secret_file_data(char *secret ,uint *secret_size, FILE *fptr_source_image)
{
	//Here i create one char array to store our secret data size of secretsize*8 because we read 8 bits every time
	unsigned char secdata[*secret_size*8];
	//Here i read secretsize*8 data at a time from source image into secdata and also we check error 
    if (fread(secdata, 1, *secret_size*8, fptr_source_image) != (*secret_size*8))
	{
		printf("Reading the data failed\n");
		return e_failure;
	}
	//Here i use this loop for decoding lsb 8-8 bits at a time
	for(int i=0; i<*secret_size*8; i=i+8)
	{
		//Here i call decode lsb to byte and after decoding we store this on index by index thats why i use this secret[i/8]
		if(decode_lsb_to_byte(&secdata[i], &secret[i/8]) != e_success)
		{
			return e_failure;
		} 
	}
	//Here we store NULL character at last 
		secret[*secret_size] ='\0';
	return e_success;	
}
