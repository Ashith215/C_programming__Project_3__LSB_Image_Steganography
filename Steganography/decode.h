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




#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * decoding secret file from Image File 
 */

typedef struct _DecodeInfo
{
    /* Image Info */
    char *image_fname;            	// => Stores the image_fname
    FILE *fptr_image;             	// => File pointer for image file

    /* Secret File Info */
    char *secret_fname;             	// => Stores the Secret_fname
    FILE *fptr_secret;           	// => File pointer for decoded_secret_file
    uint secret_file_extn_size;		// => Stores secret_file_extension_size
    char *secret_file_extn;         	// => Stores the secret_file extention
    uint secret_file_size;              // => stores the secret_file filesize.

} DecodeInfo;


/* Decoding function prototypes */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(char *argv[], DecodeInfo *decInfo);

/* Get File pointer for image file */
Status open_img_file(DecodeInfo *decInfo);

/* Skip bmp image header */
Status skip_bmp_header(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extention size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo, char *argv[]);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(int size, DecodeInfo *decInfo);

/* Decode char bytes from LSB of image buffer */
char decode_char_bytes_from_lsb(char *image_buffer);

/* Decode int bytes from LSB of image buffer */
int decode_int_bytes_from_lsb(char *image_buffer);

#endif
