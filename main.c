#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>
/*
Name : Pritam kumar
Date : 23/05/2024
Description : LSB image steganography
sample input :
sample output :
*/
//Here i create extn_size as a global we use more time extn_size 
int extn_size;
int main()
{
	//Here we read input by user to Encode or decode
	int opt;
    printf("1. Encode\n2. Decode\n");
	printf("select one option : ");
	scanf("%d",&opt);
	//Here i use switch statement to do encoding and decoding
	switch (opt)
	{
		case 1 : 
			//Encode start
			//Here i declared the structure variable to access the structure member
			EncodeInfo encInfo;
			//Here i store the beautiful.bmp and same as output.bmp and secret.txt 
			encInfo.src_image_fname = "beautiful.bmp";
			encInfo.stego_image_fname = "output.bmp";
			encInfo.secret_fname = "secret.txt";
            //Here we call the function do encoding
			do_encoding(&encInfo);

			break;
		case 2 :
			//Decode start
			//Here also i declared the structure variable to access the structure member
		    DecodeInfo decInfo;
			//Here in decoding my source is the output of encoding part thats why i store the output.bmp in source_image
			decInfo.source_image_fname = "output.bmp";
            //Here we call the function do decoding
			do_decoding(&decInfo);	

			break;
		default :
			//This statement for if user enter wrong input that time they  give one error message like invalid input
			printf("Invalid input\n");
			break;
	}
}
