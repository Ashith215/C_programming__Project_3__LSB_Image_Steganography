/*
 *      Name            :       Ashith P Amin
 *
 *      Date            :       17/11/2025
 *
 *      Description     :       Steganography Project
 *
 *                              -> In this project secret data is stored in LSB(Least Significant Bit) of .bmp image file & this is digital steganography.
 *                              -> Image file used is .bmp file, since they are uncompressed, and therefore large and lossless.
 *                              -> Advantages of .bmp files are; they have simple structure and their wide acceptance in Windows programs.
 *                              -> Image file contains at first 54 bytes of bmp header, then RGB data which is image, and then EOF(End Of File).
 *                              -> In this project main encoding and decoding is done only for this RGB(Red, Green, Blue) data.
 *                              -> If we encode secret data in MSB(Most Significant Bit) of RGB data, then there is a huge change in image output.
 *                              -> So, if we encode secret data to LSB of RGB data then there wouldn't be much in change in color of image for human eye.
 *
 *                              Encoding and Decoding:
 *
 *                              -> If argc value is other than 3, 4, or 5 then prints error message.
 *                              -> Then operation type is checked encode(-e), decode(-d) else unsupported.
 *                              -> Validation of args done for encode and decode accordingly.
 *                              -> For encoding arguments should be minimum - 4, maximum - 5. And 5th argument is optional.
 *                              -> That is ./a.out -e <.bmp file> <secret file(.c/.sh/.txt)> [output .bmp file(optional)]
 *                              -> For decoding arguments should be minimum - 3, maximum - 4. And 4th argument is optional.
 *                              -> That is ./a.out -d <secret messag encoded .bmp file> [decoded secret file name without extension(optional)].
 *                              -> For encoding, first file capacity is check by comparing secret message size and image file RGB data size.
 *                              -> Then if secret file size is less then 54 - bytes bmp header is copied to output image file.
 *                              -> Then magic string(#*) is encoded in first 16 bytes of RGB data in its LSB.
 *                              -> Then first the secret file extension size is encoded and then the secret file extention is encoded.
 *                              -> Then secret file size is encoded and then the main secret file data is encoded into LSB of RGB data.
 *                              -> Then after encoding secret data, remaining image bytes are copied to output image file.
 *                              -> For decoding, first 54 - bytes of bmp header is skipped.
 *                              -> And then from next 16 bytes, magic string is decoded and then compared with original magic string(#*).
 *                              -> If both are same then decoding is continued else decoding is stopped.
 *                              -> If same then, first size of secret file extention is decoded.
 *                              -> And then using that size, secret file extension is decoded.
 *                              -> And then the output secret file name is concatinated with this decoded extention to stored the decoded message.
 *                              -> Then after this secret file size is decoded and then secret file data is decoded.
 *                              -> And then this decoded message is stored in that output file after concanation of the extention.
 */






#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
	EncodeInfo encInfo;
	DecodeInfo decInfo;

	// if => argc is 3, 4, or 5.
	if(argc >= 3 && argc <= 5)
	{
    		// checks operation type.
    		// check_operation_type function is called.
		OperationType ret = check_operation_type(argv);
		
		// if => e_encode
		if(ret == e_encode)
		{	
			// if => argc is 4 or 5.
			if(argc >= 4 && argc <= 5)
			{
				/* Reads and validates Encode args from argv */
				if(read_and_validate_encode_args(argv, &encInfo) == e_success)
				{
					// starts the encooding.
					if(do_encoding(argv, &encInfo) == e_success)
					{
						printf("INFO: ## Encoding Done Successfully ##\n");
						return 0;
					}
					else
					{
						return 0;
					}
				}
				else									// prints error message.
				{
					printf("\nERROR: ");
					for(int i=0; i<argc; i++)
					{
						printf("%s ", argv[i]);					// prints command-line arguments user entered.
					}
					printf(": INVALID ARGUMENTS\nUSAGE:\n");
					printf("Encoding : ./a.out -e <.bmp_file> <.c/.sh/.txt_file> [.bmp_output_file(optional)]\n");
					printf("Decoding : ./a.out -d <.bmp_file> [decoded_output_file_name_without_extention(optional)]\n\n");
					return 0;
				}
			}
			else										// prints error message.
			{
				printf("\nERROR: ");
				for(int i=0; i<argc; i++)
				{
					printf("%s ", argv[i]);						// prints command-line arguments user entered.
				}
				printf(": INVALID ARGUMENTS\nUSAGE:\n");
				printf("Encoding : ./a.out -e <.bmp_file> <.c/.sh/.txt_file> [.bmp_output_file(optional)]\n");
				printf("Decoding : ./a.out -d <.bmp_file> [decoded_output_file_name_without_extention(optional)]\n\n");
				return 0;
			}
		}

		// if => e_decode
		if(ret == e_decode)
		{
			// if => argc is 3 or 4.
			if(argc >= 3 && argc <= 4)
			{
				/* Reads and validates Decode args from argv */
				if(read_and_validate_decode_args(argv, &decInfo) == e_success)
				{
					// starts the decooding.
					if(do_decoding(argv, &decInfo) == e_success)
					{
						printf("INFO: ## Decoding Done Successfully ##\n");
						return 0;
					}
					else
					{
						return 0;
					}
				}
				else									// prints error message.
				{
					printf("\nERROR: ");
					for(int i=0; i<argc; i++)					 
					{
						printf("%s ", argv[i]);					// prints command-line arguments user entered.
					}
					printf(": INVALID ARGUMENTS\nUSAGE:\n");
					printf("Encoding : ./a.out -e <.bmp_file> <.c/.sh/.txt_file> [.bmp_output_file(optional)]\n");
					printf("Decoding : ./a.out -d <.bmp_file> [decoded_output_file_name_without_extention(optional)]\n\n");
					return 0;
				}
			}
			else										// prints error message.
			{
				printf("\nERROR: ");
				for(int i=0; i<argc; i++)
				{
					printf("%s ", argv[i]);						// prints command-line arguments user entered.
				}
				printf(": INVALID ARGUMENTS\nUSAGE:\n");
				printf("Encoding : ./a.out -e <.bmp_file> <.c/.sh/.txt_file> [.bmp_output_file(optional)]\n");
				printf("Decoding : ./a.out -d <.bmp_file> [decoded_output_file_name_without_extention(optional)]\n\n");
				return 0;
			}
		}

		// if => e_unsupported
		if(ret == e_unsupported)								// prints error message.
		{
			printf("\nERROR: ");			
			for(int i=0; i<argc; i++)
			{
				printf("%s ", argv[i]);							// prints command-line arguments user entered.
			}
			printf(": INVALID ARGUMENTS\nUSAGE:\n");
			printf("Encoding : ./a.out -e <.bmp_file> <.c/.sh/.txt_file> [.bmp_output_file(optional)]\n");
			printf("Decoding : ./a.out -d <.bmp_file> [decoded_output_file_name_without_extention(optional)]\n\n");
			return 0;
		}
	}
	else												//prints error message.
	{
		printf("\nERROR: ");
		for(int i=0; i<argc; i++)
		{
			printf("%s ", argv[i]);								// prints command-line arguments user entered.
		}
		printf(": INVALID ARGUMENTS\nUSAGE:\n");
		printf("Encoding : ./a.out -e <.bmp_file> <.c/.sh/.txt_file> [.bmp_output_file(optional)]\n");
		printf("Decoding : ./a.out -d <.bmp_file> [decoded_output_file_name_without_extention(optional)]\n\n");
	}
	return 0;
}
