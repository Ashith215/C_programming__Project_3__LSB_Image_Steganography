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
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

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
	printf("INFO: Opening required files\n");

    	// Src Image file
    	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    	// Do Error handling
    	if (encInfo->fptr_src_image == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    		return e_failure;
    	}
	printf("INFO: Opened %s\n", encInfo->src_image_fname);

    	// Secret file
    	encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    	// Do Error handling
    	if (encInfo->fptr_secret == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
		fclose(encInfo->fptr_src_image);
    		return e_failure;
    	}
	printf("INFO: Opened %s\n", encInfo->secret_fname);

    	// Stego Image file
    	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    	// Do Error handling
    	if (encInfo->fptr_stego_image == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
		fclose(encInfo->fptr_src_image);
		fclose(encInfo->fptr_secret);
		remove(encInfo->stego_image_fname);
    		return e_failure;
    	}
	printf("INFO: Opened %s\n", encInfo->stego_image_fname);

    	// No failure return e_success
    	return e_success;
}


/* Checks for operation type (for both encode and decode) */
OperationType check_operation_type(char *argv[])
{
	// If 2nd command-line argument "-e" then return e_encode.
	printf("-------------------------------------------------------------------------\n");
	if(strcmp(argv[1], "-e") == 0)
	{
		printf("Operation Type = encode\n");
		printf("-------------------------------------------------------------------------\n");
		return e_encode;
	}

	// If 2nd command-line argument "-d" then return e_decode.
	if(strcmp(argv[1], "-d") == 0)
	{
		printf("Operation Type = decode\n");
		printf("-------------------------------------------------------------------------\n");
		return e_decode;
	}

	// If no either of e_encode or e_decode is returned then return e_unsupported.
	printf("Operation Type = unsupported\n");
	printf("-------------------------------------------------------------------------\n");
	return e_unsupported;
}


/* Reads and validates Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	// if => 2nd command-line argument doesn't contain extention, then print error and return e_failure.
	if(strstr(argv[2], ".") == NULL)
	{
		printf("ERROR: %s is not a .bmp file.\n", argv[2]);
		return e_failure;
	}

	// if => argv[2] is .bmp file then if condition is true else print error and return e_failure.
	if(strcmp(strstr(argv[2], "."), ".bmp") == 0)
	{
		int r1, r2, r3;	
		//copy base address of ".bmp" to extn_image_file pointer.
		encInfo->extn_image_file = ".bmp";
		//copy base address of argv[2] to src_image_fname pointer.
		encInfo->src_image_fname = argv[2];
		
		// if => 3rd command-line argument doesn't contain extention, then print error and return e_failure.
		if(strstr(argv[3], ".") == NULL)
		{
			printf("ERROR: Secret message file should be .txt/.sh/.c file only.\n");	
			return e_failure;
		}

		// if => argv[3] is .txt/.sh/.c file then if condition is true else print error and return e_failure.
		if((r1 = (strcmp(strstr(argv[3], "."), ".txt") == 0)) || (r2 = (strcmp(strstr(argv[3], "."), ".sh") == 0)) || (r3 = (strcmp(strstr(argv[3], "."), ".c") == 0)))
		{
			// if => r1 is 1, then stores .txt base address in extn_secret_file pointer.
			// if => r2 is 1, then stores .sh base address in extn_secret_file pointer.
			// if => r3 is 1, then stores .c base address in extn_secret_file pointer.
			if(r1 == 1)											
			{
				encInfo->extn_secret_file = ".txt";
			}
			else if(r2 == 1)
			{
				encInfo->extn_secret_file = ".sh";
			}
			else
			{
				encInfo->extn_secret_file = ".c";
			}
			// Stores argv[3] base address in secret_fname pointer.
			encInfo->secret_fname = argv[3];

			// if => argv[4] is entered by user, then if condition is true.
			if(!(argv[4] == NULL))
			{
				// if => 4rd command-line argument doesn't contain extention, then print error and return e_failure.
				if(strstr(argv[4], ".") == NULL)
				{
					printf("ERROR: Destination file %s is not a .bmp file.\n", argv[4]);
					return e_failure;
				}

				// if => argv[4] is .bmp file then if condition is true else print error and return e_failure.
				if(strcmp(strstr(argv[4], "."), ".bmp") == 0)
				{
					// Stores argv[4] base address in stego_image_fname pointer.
					encInfo->stego_image_fname = argv[4];
					return e_success;
				}
				else
				{
					printf("ERROR: Destination file %s is not a .bmp file.\n", argv[4]);
					return e_failure;
				}
			}
			else	// if => argv[4] is not entered by user, then store "stego.bmp" base address in stego_image_fname and return e_success.
			{
				encInfo->stego_image_fname = "stego.bmp";
				return e_success;
			}
		}
		else
		{
			printf("ERROR: Secret message file should be .txt/.sh/.c file only.\n");	
			return e_failure;
		}
	}
	printf("ERROR: %s is not a .bmp file.\n", argv[2]);
	return e_failure;
}


/* Performs the encoding */
Status do_encoding(char *argv[], EncodeInfo *encInfo)
{
	//open_files() function is called and if => e_success.
	if(open_files(encInfo) == e_success)
	{
		printf("INFO: Done\n");
		printf("INFO: ## Encoding Procedure Started ##\n");
		// check_capacity() function is called and if => e_success.
		if(check_capacity(encInfo) == e_success)
		{
			if(argv[4] == NULL)
			{
				printf("INFO: Output File not mentioned. Creating %s as default\n", encInfo->stego_image_fname);
			}
			else
			{
				printf("INFO: Creating %s as encoded output image file.\n", encInfo->stego_image_fname);
			}

			// copy_bmp_header() function is called and if => e_success.
			if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
			{
				// encode_magic_string() function is called and if => e_success.
				if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
				{
					// secret file extention size is stored in extn_secret_file_len.
					int extn_secret_file_len = strlen(encInfo->extn_secret_file);
					// encode_secret_file_extn_size() function is called and if => e_success.
					if(encode_secret_file_extn_size(extn_secret_file_len, encInfo) == e_success)
					{
						// encode_secret_file_extn() function is called and if => e_success.
						if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
						{
							// encode_secret_file_size() function called and if => e_success.
							if(encode_secret_file_size(encInfo->secret_file_size, encInfo) == e_success)
							{
								// encode_secret_file_data() function is called and if => e_success.
								if(encode_secret_file_data(encInfo) == e_success)
								{
									// copy_remaining_img_data() function is called and if => e_success.
									if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo) == e_success)
									{
										fclose(encInfo->fptr_src_image);
										fclose(encInfo->fptr_secret);
										fclose(encInfo->fptr_stego_image);
										return e_success;
									}
									else
									{
										printf("ERROR: Encoding of remaining image data failed.\n");
										fclose(encInfo->fptr_src_image);
										fclose(encInfo->fptr_secret);
										fclose(encInfo->fptr_stego_image);
										remove(encInfo->stego_image_fname);
										return e_failure;
									}
								}
								else
								{
									printf("ERROR: Encoding of secret file data failed.\n");
									fclose(encInfo->fptr_src_image);
									fclose(encInfo->fptr_secret);
									fclose(encInfo->fptr_stego_image);
									remove(encInfo->stego_image_fname);
									return e_failure;
								}
							}
							else
							{
								printf("ERROR: Encoding of secret file size failed.\n");
								fclose(encInfo->fptr_src_image);
								fclose(encInfo->fptr_secret);
								fclose(encInfo->fptr_stego_image);
								remove(encInfo->stego_image_fname);
								return e_failure;
							}
						}
						else
						{
							printf("ERROR: Encoding of secret file extension failed.\n");
							fclose(encInfo->fptr_src_image);
							fclose(encInfo->fptr_secret);
							fclose(encInfo->fptr_stego_image);
							remove(encInfo->stego_image_fname);
							return e_failure;
						}	
					}
					else
					{
						printf("ERROR: Encoding of secret file extension size failed.\n");
						fclose(encInfo->fptr_src_image);
						fclose(encInfo->fptr_secret);
						fclose(encInfo->fptr_stego_image);
						remove(encInfo->stego_image_fname);
						return e_failure;
					}
				}
				else
				{
					printf("ERROR: Encoding of Magic String(#*) failed.\n");
					fclose(encInfo->fptr_src_image);
					fclose(encInfo->fptr_secret);
					fclose(encInfo->fptr_stego_image);
					remove(encInfo->stego_image_fname);
					return e_failure;
				}
			}
			else
			{
				printf("ERROR: Encoding of 54 - byte header failed.\n");
				fclose(encInfo->fptr_src_image);
				fclose(encInfo->fptr_secret);
				fclose(encInfo->fptr_stego_image);
				remove(encInfo->stego_image_fname);
				return e_failure;
			}
		}
		else
		{
			fclose(encInfo->fptr_src_image);
			fclose(encInfo->fptr_secret);
			fclose(encInfo->fptr_stego_image);
			remove(encInfo->stego_image_fname);
			return e_failure;
		}
	}
	return e_failure;
}




/* checks capacity of image file RGB data with secret message to encode */
Status check_capacity(EncodeInfo *encInfo)
{
	// Image size plus 54 bytes bmp header size is stored in Image_capacity and then stores it to image_capacity pointer.
	int Image_capacity = (get_image_size_for_bmp(encInfo->fptr_src_image) + 54); 		//get_image_size_for_bmp() function is called.
	encInfo->image_capacity = Image_capacity;

	// Magic String length is stored.
	int Magic_string_len = strlen(MAGIC_STRING);

	// secret file extension length is stored.
	int Secret_file_extn_len = strlen(encInfo->extn_secret_file);		

	printf("INFO: Checking for %s size\n", encInfo->secret_fname);
	// secret file size is stored and then copied to secret_file_size pointer.
	int secret_fsize = get_file_size(encInfo->fptr_secret);					//get_file_size() function is called.
	encInfo->secret_file_size = secret_fsize - 1;

	//if secret file is empty then print empty.
	if(encInfo->secret_file_size == 1 || encInfo->secret_file_size == 0)
	{
		printf("ERROR: %s file is empty\n", encInfo->secret_fname);			
		return e_failure;
	}
	printf("INFO: Done. Not Empty\n");

	// 54 bmp header plus (magic_string,4 - secret_file_extention_size,secret_file_extention_length,4 - secret_file_extention_size,secret_file_size)*8.
	int Encoding_things = 54 + ((Magic_string_len + sizeof(int) + Secret_file_extn_len + 4 + encInfo->secret_file_size) * 8);

	printf("INFO: Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);
	//if encoding data id less then image header plus RGB data and end of file, then if condition is true.
	if(Encoding_things < Image_capacity)
	{
		printf("INFO: Done. Found OK\n");
		return e_success;
	}
	printf("ERROR: \"%s\" doesn't have the capacity to encode \"%s\"\n", encInfo->src_image_fname, encInfo->secret_fname);
	return e_failure;
}




/* Gets file size and the returns size as unsigned int */
uint get_file_size(FILE *fptr)
{
	// file pointer position is moved to end of file.
	fseek(fptr, SEEK_CUR, SEEK_END);

	// current position of fptr is stored in ret.
	unsigned int ret = ftell(fptr);
	
	// return ret value.
	return ret;
}





/* Copies bmp image header to destination image file */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	// rewind source file pointer to 0th position.
	rewind(fptr_src_image);
	
	printf("INFO: Copying Image Header\n");
	
	char buffer[55];
	int r = fread(buffer, (sizeof(buffer) - 1), 1, fptr_src_image);
	buffer[54] = '\0';
	
	// if fread doesn't read 54 bytes, then r will be 0 else r will be 1.
	if(r == 0)
	{
		printf("ERROR: 54 - bytes header not present.\n");
		return e_failure;
	}
	
	// writes 54 bytes of data buffer to destination image file pointer.
	fwrite(buffer, (sizeof(buffer) - 1), 1, fptr_dest_image);
	
	printf("INFO: Done\n");
	return e_success;
}




/* Stores Magic String (#*) */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	printf("INFO: Encoding Magic String Signature\n");

	// magic string length is stored.
	int magic_string_len = strlen(magic_string);				
	char *magic_str = (char *)magic_string;
	
	// encode_data_to_image() function is called and if => e_success.
	if(encode_data_to_image(magic_str, magic_string_len, encInfo) == e_success)
	{
		printf("INFO: Done\n");
		return e_success;
	}
	return e_failure;
}



/* Encode function, which does the real encoding except for secret file size and extention size */
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
	for(int i=0; i<size; i++)
	{
		char buffer[9];
		int r = fread(buffer, (sizeof(buffer) - 1), 1, encInfo->fptr_src_image);     	
		buffer[8] = '\0';

		// if fread doesn't read 8 bytes, then r will be 0 else r will be 1.
		if(r == 0)							
		{
			printf("ERROR: 8-bytes of characters from %s image file is not read for encoding data.\n", encInfo->src_image_fname);
			return e_failure;
		}
		
		// encode_byte_to_lsb() function is called.
		encode_byte_to_lsb(data[i], buffer, 8);

		// writes 8 bytes of data buffer to fptr_stego_image file pointer.
		fwrite(buffer, (sizeof(buffer) - 1), 1, encInfo->fptr_stego_image);          	
	}
	return e_success;
}




/* Encodes secret file extention size */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
	char buffer[33];
	int r = fread(buffer, (sizeof(buffer) - 1), 1, encInfo->fptr_src_image);        
	buffer[32] = '\0';

	// if fread doesn't read 32 bytes, then r will be 0 else r will be 1.
	if(r == 0)								
	{
		printf("ERROR: 32-bytes of characters from %s image file is not read for encoding secret file extention size.\n", encInfo->src_image_fname);
		return e_failure;
	}
	
	// encode_byte_to_lsb() function is called.
	encode_byte_to_lsb(size, buffer, 32);

	// writes 32 bytes of data buffer to fptr_stego_image file pointer.
	fwrite(buffer, (sizeof(buffer) - 1), 1, encInfo->fptr_stego_image);	
	
	return e_success;
}



/* Encodes a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer, int bytes)			
{
	int l = bytes;						
	int j = 0, bit;

	// if => l is 32.
	if(l == 32)
	{
		int size = data;
	
		for(int i=31; i>=0; i--)
		{
			bit = ((unsigned)size & (1 << i)) >> i;			// gets each bits of int data one by one.
			if(bit == 1)
			{
				image_buffer[j] = image_buffer[j] | 1;		// if bit is 1, then 1 is replaced to lsb of image_buffer.
			}
			else
			{
				image_buffer[j] = image_buffer[j] & (~(1));	// if bit is 0, then 0 is replaced to lsb of image_buffer.
			}
			j++;
		}
	}
	else if(l == 8)		// if => l is 8.
	{
		for(int i=7; i>=0; i--)
		{
			bit = ((unsigned)data & (1 << i)) >> i;			// gets each bits of char data one by one.
			if(bit == 1)
			{
				image_buffer[j] = image_buffer[j] | 1;		// if bit is 1, then 1 is replaced to lsb of image_buffer.
			}
			else
			{
				image_buffer[j] = image_buffer[j] & (~(1));	// if bit is 0, then 0 is replaced to lsb of image_buffer.
			}
			j++;
		}
	}
	else			// if => l is not 8 or 32.
	{
		return e_failure;
	}

	return e_success;
}





/* Encodes secret file extenstion */
Status encode_secret_file_extn(const char *ext, EncodeInfo *encInfo)
{
	printf("INFO: Encoding %s File Extension\n", encInfo->secret_fname);
	
	// length of ext pointing data is stored.
	int ext_len = strlen(ext);
	char *ext_char = (char *)ext;

	// encode_data_to_image() function is called and if => e_success.
	if(encode_data_to_image(ext_char, ext_len, encInfo) == e_success)
	{
		printf("INFO: Done\n");
		return e_success;
	}
	return e_failure;
}





/* Encodes secret file size */
Status encode_secret_file_size(int size, EncodeInfo *encInfo)
{
	printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
	
	char buffer[33];
	int r = fread(buffer, (sizeof(buffer) - 1), 1, encInfo->fptr_src_image);
	buffer[32] = '\0';
	
	// if fread doesn't read 32 bytes, then r will be 0 else r will be 1.
	if(r == 0)
	{
		printf("ERROR: 32-bytes of characters from %s image file is not read for encoding secret file size.\n", encInfo->src_image_fname);
		return e_failure;
	}

	// encode_byte_to_lsb() function is called.
	encode_byte_to_lsb(size, buffer, 32);

	// writes 32 bytes of data buffer to fptr_stego_image file pointer.
	fwrite(buffer, (sizeof(buffer) - 1), 1, encInfo->fptr_stego_image);
	
	printf("INFO: Done\n");
	return e_success;
}




/* Encodes secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	// rewind fptr_secret file pointer to 0th position.
	rewind(encInfo->fptr_secret);
	
	printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);

	// secret file data is read.
	char secret_file_data[(encInfo->secret_file_size) + 1];
	int r = fread(secret_file_data, (sizeof(secret_file_data) - 1), 1, encInfo->fptr_secret);
	secret_file_data[encInfo->secret_file_size] = '\0';

	// if fread doesn't read secret_file_size number of bytes, then r will be 0 else r will be 1.
	if(r == 0)
	{
		printf("ERROR: %s file data is not read.\n", encInfo->secret_fname);
		return e_failure;
	}

	// encode_data_to_image() function is called and if => e_success.
	if(encode_data_to_image(secret_file_data, encInfo->secret_file_size, encInfo) == e_success)
	{
		printf("INFO: Done\n");
		return e_success;
	}
	return e_failure;
}




/* Copies remaining image bytes from source image file to destination image file after encoding secret message */
Status copy_remaining_img_data(FILE* fptr_src, FILE* fptr_dest, EncodeInfo *encInfo)
{
	printf("INFO: Copying Left Over Data\n");

	// current position of fptr_src file pointer is stored in cur_pos.
	int cur_pos = ftell(fptr_src);

	// i value loops from cur_pos value to image_capacity value.
	for(int i=cur_pos; i<=(encInfo->image_capacity); i++)
	{
		char ch;
		int r = fread(&ch, sizeof(char), 1, fptr_src);

		// if fread doesn't read 1 byte each loop, then r will be 0 else r will be 1.
		if(r == 0)
		{
			printf("ERROR: Character from %s image file is not read for copying remaining data to %s encoded file.\n", encInfo->src_image_fname, encInfo->secret_fname);
			return e_failure;
		}

		// writes 1 byte of data from ch to fptr_dest file pointer for each loop.
		fwrite(&ch, sizeof(char), 1, fptr_dest);
	}
	printf("INFO: Done\n");
	return e_success;
}


