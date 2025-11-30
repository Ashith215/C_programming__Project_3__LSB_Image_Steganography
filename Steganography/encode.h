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





#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;		// => stores the src_Image_fname
    FILE *fptr_src_image;		// => File pointer for src_image
    char *extn_image_file;		// => stores image_file extension
    uint image_capacity;		// => Stores the src_img_filesize

    /* Secret File Info */
    char *secret_fname;			// => Stores the Secret_fname	
    FILE *fptr_secret;			// => File pointer for secret_file
    char *extn_secret_file;		// => Stores the secret_file extention
    uint secret_file_size;		// => stores the secret_file filesize.

    /* Stego Image Info */
    char *stego_image_fname;		// => Stores the Output_img_fname
    FILE *fptr_stego_image;		// => File pointer for stego_image

} EncodeInfo;


/* Encoding function prototypes */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(char *argv[], EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extention size */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo);

/* Encode secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer, int bytes);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest, EncodeInfo *encInfo);

#endif
