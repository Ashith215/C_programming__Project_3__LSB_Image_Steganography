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
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/*
 * Gets File pointers for image file
 * Return Value: e_success or e_failure, on file errors
 */
Status open_img_file(DecodeInfo *decInfo)
{
	printf("INFO: Opening required image file\n");
	
    	// Image file
    	decInfo->fptr_image = fopen(decInfo->image_fname, "rb");
    	// Do Error handling
    	if(decInfo->fptr_image == NULL)
    	{
        	perror("fopen");
        	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->image_fname);
        	return e_failure;
    	}
	printf("INFO: Opened %s\n", decInfo->image_fname);
    	
	// No failure return e_success
    	return e_success;
}



/* Reads and validates Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	// if => 3rd command-line argument doesn't contain extention, then print error and return e_failure.
	if(strstr(argv[2], ".") == NULL)
	{
		printf("ERROR: Entered %s is not .bmp file.\n", argv[2]);
		return e_failure;
	}

	// if => argv[2] is .bmp file then if condition is true else print error and return e_failure.
	if(strcmp(strstr(argv[2], "."), ".bmp") == 0)
	{
		decInfo->image_fname = argv[2];
	
		// if => argv[3] is not entered by user, then if condition is true.	
		if(argv[3] == NULL)
		{
			// "decoded_secret" is taken as default decoded secret file name.
			decInfo->secret_fname = "decoded_secret";
			return e_success;
		}
		else	// if => argv[3] is entered by user, then else part is true.
		{
			// if => 4th command-line argument doesn't contain extention, then if condition is true.
			if(strstr(argv[3], ".") == NULL)
			{
				// user entered 4th command-line argument is taken as decoded secret file name and then extention is added after decoding.
				decInfo->secret_fname = argv[3];
				return e_success;
			}
			else	//if => 4th command-line argument contain any extension then print error and return e_failure.
			{
				printf("ERROR: Entered %s should not contain extention. Since extension is encoded in image file.\n", argv[3]);
				return e_failure;
			}
		}	
	}
	printf("ERROR: Entered %s is not .bmp file.\n", argv[2]);
	return e_failure;
}

/* Performs the decoding */
Status do_decoding(char *argv[], DecodeInfo *decInfo)
{
	printf("INFO: ## Decoding Procedure Started ##\n");
	// open_img_file() function is called and if => e_success.
	if(open_img_file(decInfo) == e_success)
	{
		// skip_bmp_header() function is called and if => e_success.
		if(skip_bmp_header(decInfo) == e_success)
		{
			// decode_magic_string() function is called and if => e_success.
			if(decode_magic_string(decInfo) == e_success)
			{
				// decode_secret_file_extn_size() function is called and if => e_success.
				if(decode_secret_file_extn_size(decInfo) == e_success)
				{
					// decode_secret_file_extn() function is called and if => e_success.
					if(decode_secret_file_extn(decInfo->secret_file_extn_size, decInfo, argv) == e_success)
					{
						// decode_secret_file_size() function is called and if => e_success.
						if(decode_secret_file_size(decInfo) == e_success)
						{
							// decode_secret_file_data() function is called and if => e_success.
							if(decode_secret_file_data(decInfo->secret_file_size, decInfo) == e_success)
							{
								fclose(decInfo->fptr_image);	
								fclose(decInfo->fptr_secret);	
								return e_success;
							}
							else
							{
								fclose(decInfo->fptr_image);	
								fclose(decInfo->fptr_secret);	
								remove(decInfo->secret_fname);
								return e_failure;
							}
						}
						else
						{
							fclose(decInfo->fptr_image);	
							fclose(decInfo->fptr_secret);	
							remove(decInfo->secret_fname);
							return e_failure;
						}
					}
					else
					{
						fclose(decInfo->fptr_image);	
						return e_failure;
					}
				}
				else
				{
					fclose(decInfo->fptr_image);	
					return e_failure;
				}
			}
			else
			{
				fclose(decInfo->fptr_image);	
				return e_failure;
			}
		}
		else
		{
			fclose(decInfo->fptr_image);	
			return e_failure;
		}
	}
	return e_failure;
}




/* Skips bmp image header */
Status skip_bmp_header(DecodeInfo *decInfo)
{
	// skips 54 bytes of bmp header in fptr_image file pointer.
	int fs = fseek(decInfo->fptr_image, 54, SEEK_CUR);
	// if 54 bytes are not skiped, then print error and return e_failure.
	if(fs != 0)
	{
		printf("ERROR: No 54 - bytes header in %s file.\n", decInfo->image_fname);
		return e_failure;
	}

	return e_success;
}




/* Decodes Magic String */
Status decode_magic_string(DecodeInfo *decInfo)
{
	printf("INFO: Decoding Magic String Signature\n");
	
	char magic_string[3];
	for(int i=0; i<2; i++)
	{
		char image_buffer[9];
		int r = fread(image_buffer, (sizeof(image_buffer) - 1), 1, decInfo->fptr_image);
		image_buffer[8] = '\0';

		// if fread doesn't read 8 bytes, then r will be 0 else r will be 1.
		if(r == 0)
		{
			printf("ERROR: Unable to read %s file to decode magic string.\n", decInfo->image_fname);
			return e_failure;
		}
		
		// decoded character data from image_buffer is stored in magic_string of ith index.
		magic_string[i] = decode_char_bytes_from_lsb(image_buffer);		// decode_char_bytes_from_lsb() function is called.
	}
	magic_string[2] = '\0';

	// if => decoded magic string is not equal to original magic string then print error and return e_failure.
	if(strcmp(magic_string, MAGIC_STRING) != 0)
	{
		printf("ERROR: Decoded magic string doesn't match original magic string(#*).\n");
		return e_failure;
	}
	
	printf("INFO: Done\n");
	return e_success;
}




/* Decodes secret file extention size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	char image_buffer[33];
	int r = fread(image_buffer, (sizeof(image_buffer) - 1), 1, decInfo->fptr_image);
	image_buffer[32] = '\0';

	// if fread doesn't read 32 bytes, then r will be 0 else r will be 1.
	if(r == 0)
	{
		printf("ERROR: Unable to read %s file to decode secret file extension size.\n", decInfo->image_fname);
		return e_failure;
	}

	// decoded int data from image_buffer is stored in extn_size and then copied to secret_file_extn_size pointer.
	int extn_size = decode_int_bytes_from_lsb(image_buffer);	// decode_int_byte_from_lsb() function is called.
	decInfo->secret_file_extn_size = extn_size;

	return e_success;
}




/* Decodes secret file extenstion */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo, char *argv[])
{
	printf("INFO: Decoding Output File Extension\n");

	char secret_file_extn[size + 1];
	for(int i=0; i<size; i++)
	{
		char image_buffer[9];
		int r = fread(image_buffer, (sizeof(image_buffer) - 1), 1, decInfo->fptr_image);
		image_buffer[8] = '\0';

		// if fread doesn't read 8 bytes, then r will be 0 else r will be 1.
		if(r == 0)
		{
			printf("ERROR: Unable to read %s file to decode secret file extention.\n", decInfo->image_fname);
			return e_failure;
		}

		// decoded character data from image_buffer is stored in secret_file_extn of ith index.
		secret_file_extn[i] = decode_char_bytes_from_lsb(image_buffer);			// decode_char_bytes_from_lsb() function is called.
	}
	secret_file_extn[size] = '\0';

	// decoded secret file extension base address is stored to secret_file_extn pointer.
	decInfo->secret_file_extn = secret_file_extn;
	printf("INFO: Done\n");
	
	char str[100];
	strcpy(str, decInfo->secret_fname);			// file name is copied to string str.
	strcat(str, decInfo->secret_file_extn);			// file name and extension of decoded secret file is concatinated. 
	decInfo->secret_fname = str;

	if(argv[3] == NULL)
	{
		printf("INFO: Output File not mentioned. Creating %s as default\n", decInfo->secret_fname);
	}
	else
	{
		printf("INFO: Creating %s as decoded output file.\n", decInfo->secret_fname);
	}

	// decoded secret file is oped in write mode.
	decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    	// Do Error handling
    	if(decInfo->fptr_secret == NULL)
    	{
    	    	perror("fopen");
    	    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
		remove(decInfo->secret_fname);
        	return e_failure;
    	}
	printf("INFO: Opened %s\n", decInfo->secret_fname);
	printf("INFO: Done. Opened all required files\n");
	
    	// No failure return e_success
	return e_success;
}




/* Decodes secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
	printf("INFO: Decoding File Size\n");
	
	char image_buffer[33];
	int r = fread(image_buffer, (sizeof(image_buffer) - 1), 1, decInfo->fptr_image);
	image_buffer[32] = '\0';

	// if fread doesn't read 32 bytes, then r will be 0 else r will be 1.
	if(r == 0)
	{
		printf("ERROR: Unable to read %s file to decode secret file size.\n", decInfo->image_fname);
		return e_failure;
	}

	// decoded int data from image_buffer is stored in secret_file_size pointer.
	decInfo->secret_file_size = decode_int_bytes_from_lsb(image_buffer); 		// decode_int_bytes_from_lsb() function is called.
	printf("INFO: Done\n");

	return e_success;
}





/* Decodes secret file data */
Status decode_secret_file_data(int size, DecodeInfo *decInfo)
{
	printf("INFO: Decoding File Data\n");
	
	for(int i=0; i<size; i++)
	{
		char ch;
		char image_buffer[9];
		int r = fread(image_buffer, (sizeof(image_buffer) - 1), 1, decInfo->fptr_image);
		image_buffer[8] = '\0';
		
		// if fread doesn't read 8 bytes, then r will be 0 else r will be 1.
		if(r == 0)
		{
			printf("ERROR: Unable to read %s file to decode secret file data.\n", decInfo->image_fname);
			return e_failure;
		}	

		// decoded character data from image_buffer is stored in ch.
		ch = decode_char_bytes_from_lsb(image_buffer); 			// decode_char_bytes_from_lsb() function is called.
		
		// writes character ch to fptr_secret file pointer size times.
		fwrite(&ch, sizeof(char), 1, decInfo->fptr_secret);
	}
	printf("INFO: Done\n");
	
	return e_success;
}




/* Decodes char bytes from LSB of image data array */
char decode_char_bytes_from_lsb(char *image_buffer)
{
	char ch = 0, bit;
	int j = 0;

	for(int i=7; i>=0; i--)				// to decode character data.
	{
		bit = image_buffer[j] & 1;		// lsb bit of image_buffer is obtained and stored in bit.

		ch = ch | (bit << i);			// obtained bit is replaced to ch starting from msb to lsb one by one through loop. 
		
		j++;	
	}
	return ch;
}




/* Decodes int bytes from LSB of image data array */
int decode_int_bytes_from_lsb(char *image_buffer)
{
	int in = 0;
	int j = 0, bit;

	for(int i=31; i>=0; i--)			// to decode secret file extention size and secret file size.
	{
		bit = image_buffer[j] & 1;		// lsb bit of image_buffer is obtained and stored in bit.

		in = in | (bit << i);			// obtained bit is replaced to in strating from msb to lsb one by one through loop.
		
		j++;	
	}
	return in;
}


