/*
 * Windows BMP file functions for OpenGL.
 *
 * Written by Michael Sweet.
 */

#include <stdlib.h>
#include <errno.h>
#include "../../header/bitmapIO/bitmap.h"
#include <stdio.h>

namespace LLib
{
	namespace IO
	{
		namespace BtimapIO
		{
					#ifdef WIN32
					/*
					* 'LoadDIBitmap()' - Load a DIB/BMP file from disk.
					*
					* Returns a pointer to the bitmap if successful, NULL otherwise...
					*/

					GLubyte *                          /* O - Bitmap data */
					LoadDIBitmap(const char *filename, /* I - File to load */
								BITMAPINFO **info)    /* O - Bitmap information */
						{
						FILE             *fp;          /* Open file pointer */
						GLubyte          *bits;        /* Bitmap pixel bits */
						int              bitsize;      /* Size of bitmap */
						int              infosize;     /* Size of header information */
						BITMAPFILEHEADER header;       /* File header */

						/* Try opening the file; use "rb" mode to read this *binary* file. */
						if ((fp = fopen(filename, "rb")) == NULL)
							return (NULL);

						/* Read the file header and any following bitmap information... */
						if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1)
							{
							/* Couldn't read the file header - return NULL... */
						fclose(fp);
							return (NULL);
							}

						if (header.bfType != 'MB')	/* Check for BM reversed... */
							{
							/* Not a bitmap file - return NULL... */
							fclose(fp);
							return (NULL);
							}

						infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
						if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL)
							{
							/* Couldn't allocate memory for bitmap info - return NULL... */
							fclose(fp);
							return (NULL);
							}

						if ((int) fread(*info, 1, infosize, fp) <  infosize)
							{
							/* Couldn't read the bitmap header - return NULL... */
							free(*info);
							fclose(fp);
							return (NULL);
							}

						/* Now that we have all the header info read in, allocate memory for *
						* the bitmap and read *it* in...                                    */
						if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
							bitsize = ((*info)->bmiHeader.biWidth *
									(*info)->bmiHeader.biBitCount + 7) / 8 *
  								abs((*info)->bmiHeader.biHeight);

						if ((bits = (GLubyte*) malloc(bitsize)) == NULL)
							{
							/* Couldn't allocate memory - return NULL! */
							free(*info);
							fclose(fp);
							return (NULL);
							}

						if ((int)fread(bits, 1, bitsize, fp) < bitsize)
							{
							/* Couldn't read bitmap - free memory and return NULL! */
							free(*info);
							free(bits);
							fclose(fp);
							return (NULL);
							}

						/* OK, everything went fine - return the allocated bitmap... */
						fclose(fp);
						return (bits);
						}


					/*
					* 'SaveDIBitmap()' - Save a DIB/BMP file to disk.
					*
					* Returns 0 on success or -1 on failure...
					*/

					int                                /* O - 0 = success, -1 = failure */
					SaveDIBitmap(const char *filename, /* I - File to load */
								BITMAPINFO *info,     /* I - Bitmap information */
							GLubyte    *bits)     /* I - Bitmap data */
						{
						FILE             *fp;          /* Open file pointer */
						int              size,         /* Size of file */
										infosize,     /* Size of bitmap info */
										bitsize;      /* Size of bitmap pixels */
						BITMAPFILEHEADER header;       /* File header */


						/* Try opening the file; use "wb" mode to write this *binary* file. */
						if ((fp = fopen(filename, "wb")) == NULL)
							return (-1);

						/* Figure out the bitmap size */
						if (info->bmiHeader.biSizeImage == 0)
						bitsize = (info->bmiHeader.biWidth *
        						info->bmiHeader.biBitCount + 7) / 8 *
							abs(info->bmiHeader.biHeight);
						else
						bitsize = info->bmiHeader.biSizeImage;

						/* Figure out the header size */
						infosize = sizeof(BITMAPINFOHEADER);
						switch (info->bmiHeader.biCompression)
						{
						case BI_BITFIELDS :
								infosize += 12; /* Add 3 RGB doubleword masks */
								if (info->bmiHeader.biClrUsed == 0)
							break;
						case BI_RGB :
								if (info->bmiHeader.biBitCount > 8 &&
        						info->bmiHeader.biClrUsed == 0)
							break;
						case BI_RLE8 :
						case BI_RLE4 :
								if (info->bmiHeader.biClrUsed == 0)
								infosize += (1 << info->bmiHeader.biBitCount) * 4;
							else
								infosize += info->bmiHeader.biClrUsed * 4;
							break;
						}

						size = sizeof(BITMAPFILEHEADER) + infosize + bitsize;

						/* Write the file header, bitmap information, and bitmap pixel data... */
						header.bfType      = 'MB'; /* Non-portable... sigh */
						header.bfSize      = size;
						header.bfReserved1 = 0;
						header.bfReserved2 = 0;
						header.bfOffBits   = sizeof(BITMAPFILEHEADER) + infosize;

						if (fwrite(&header, 1, sizeof(BITMAPFILEHEADER), fp) < sizeof(BITMAPFILEHEADER))
							{
							/* Couldn't write the file header - return... */
							fclose(fp);
							return (-1);
							}

						if ((int)fwrite(info, 1, infosize, fp) < infosize)
							{
							/* Couldn't write the bitmap header - return... */
							fclose(fp);
							return (-1);
							}

						if ((int)fwrite(bits, 1, bitsize, fp) < bitsize)
							{
							/* Couldn't write the bitmap - return... */
							fclose(fp);
							return (-1);
							}

						/* OK, everything went fine - return... */
						fclose(fp);
						return (0);
						}

					GLubyte *
					ReadDIBitmap(BITMAPINFO **info, int channelNumbers /* = 3*/)
					{
						long	bitsize,
								width;
						GLint viewport[4];
						GLubyte *bits;

						glGetIntegerv(GL_VIEWPORT,viewport);
						if((*info = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER))) == NULL)
						{
							return (NULL);
						}
						width = viewport[2]*channelNumbers;
						width = (width+3) & ~3;
						bitsize = width * viewport[3];
						if((bits = (GLubyte*)calloc(bitsize,1)) == NULL)
						{
							free(*info);
							return (NULL);
						}
						glFinish();
						glPixelStorei(GL_PACK_ALIGNMENT,4);
						glPixelStorei(GL_PACK_ROW_LENGTH,0);
						glPixelStorei(GL_PACK_SKIP_ROWS,0);
						glPixelStorei(GL_PACK_SKIP_PIXELS,0);

						if (channelNumbers==4)
						{
							glReadPixels(0,0,viewport[2],viewport[3],GL_BGRA_EXT,GL_UNSIGNED_BYTE,bits);
						}
						else if( channelNumbers == 3)
						{
							glReadPixels(0,0,viewport[2],viewport[3],GL_BGR_EXT,GL_UNSIGNED_BYTE,bits);
						}
						else
						{
							free(info);
							free(bits);
							return NULL;
						}

						(*info)->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
						(*info)->bmiHeader.biWidth = viewport[2];
						(*info)->bmiHeader.biHeight = viewport[3];
						(*info)->bmiHeader.biPlanes = 1;
						(*info)->bmiHeader.biBitCount = 8 * channelNumbers;
						(*info)->bmiHeader.biCompression = BI_RGB;
						(*info)->bmiHeader.biSizeImage = bitsize;
						(*info)->bmiHeader.biXPelsPerMeter = 2952;
						(*info)->bmiHeader.biYPelsPerMeter = 2952;
						(*info)->bmiHeader.biClrUsed = 0;
						(*info)->bmiHeader.biClrImportant = 0;

						return (bits);
					}
					void SetBitmapHeader( BITMAPINFO ** info, int width, int height, int channelNumbers /*= 3*/ )
					{
						long w = width*channelNumbers;
						w = (w+3) & ~3;
						long bitsize = w * height;

						if((*info = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER))) == NULL)
						{
							return ;
						}
						(*info)->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
						(*info)->bmiHeader.biWidth = width;
						(*info)->bmiHeader.biHeight = height;
						(*info)->bmiHeader.biPlanes = 1;
						(*info)->bmiHeader.biBitCount = 8 * channelNumbers;
						(*info)->bmiHeader.biCompression = BI_RGB;
						(*info)->bmiHeader.biSizeImage = bitsize;
						(*info)->bmiHeader.biXPelsPerMeter = 2952;
						(*info)->bmiHeader.biYPelsPerMeter = 2952;
						(*info)->bmiHeader.biClrUsed = 0;
						(*info)->bmiHeader.biClrImportant = 0;
						return ;
					}


					#else /* !WIN32 */
					/*
					* Functions for reading and writing 16- and 32-bit little-endian integers.
					*/

					static unsigned short read_word(FILE *fp);
					static unsigned int   read_dword(FILE *fp);
					static int            read_long(FILE *fp);

					static int            write_word(FILE *fp, unsigned short w);
					static int            write_dword(FILE *fp, unsigned int dw);
					static int            write_long(FILE *fp, int l);


					/*
					* 'LoadDIBitmap()' - Load a DIB/BMP file from disk.
					*
					* Returns a pointer to the bitmap if successful, NULL otherwise...
					*/

					GLubyte *                          /* O - Bitmap data */
					LoadDIBitmap(const char *filename, /* I - File to load */
								BITMAPINFO **info)    /* O - Bitmap information */
						{
						FILE             *fp;          /* Open file pointer */
						GLubyte          *bits;        /* Bitmap pixel bits */
						GLubyte          *ptr;         /* Pointer into bitmap */
						GLubyte          temp;         /* Temporary variable to swap red and blue */
						int              x, y;         /* X and Y position in image */
						int              length;       /* Line length */
						int              bitsize;      /* Size of bitmap */
						int              infosize;     /* Size of header information */
						BITMAPFILEHEADER header;       /* File header */


						/* Try opening the file; use "rb" mode to read this *binary* file. */
						if ((fp = fopen(filename, "rb")) == NULL)
							return (NULL);

						/* Read the file header and any following bitmap information... */
						header.bfType      = read_word(fp);
						header.bfSize      = read_dword(fp);
						header.bfReserved1 = read_word(fp);
						header.bfReserved2 = read_word(fp);
						header.bfOffBits   = read_dword(fp);

						if (header.bfType != BF_TYPE) /* Check for BM reversed... */
							{
							/* Not a bitmap file - return NULL... */
							fclose(fp);
							return (NULL);
							}

						infosize = header.bfOffBits - 18;
						if ((*info = (BITMAPINFO *)malloc(sizeof(BITMAPINFO))) == NULL)
							{
							/* Couldn't allocate memory for bitmap info - return NULL... */
							fclose(fp);
							return (NULL);
							}

						(*info)->bmiHeader.biSize          = read_dword(fp);
						(*info)->bmiHeader.biWidth         = read_long(fp);
						(*info)->bmiHeader.biHeight        = read_long(fp);
						(*info)->bmiHeader.biPlanes        = read_word(fp);
						(*info)->bmiHeader.biBitCount      = read_word(fp);
						(*info)->bmiHeader.biCompression   = read_dword(fp);
						(*info)->bmiHeader.biSizeImage     = read_dword(fp);
						(*info)->bmiHeader.biXPelsPerMeter = read_long(fp);
						(*info)->bmiHeader.biYPelsPerMeter = read_long(fp);
						(*info)->bmiHeader.biClrUsed       = read_dword(fp);
						(*info)->bmiHeader.biClrImportant  = read_dword(fp);

						if (infosize > 40)
						if (fread((*info)->bmiColors, infosize - 40, 1, fp) < 1)
								{
								/* Couldn't read the bitmap header - return NULL... */
								free(*info);
								fclose(fp);
								return (NULL);
								}

						/* Now that we have all the header info read in, allocate memory for *
						* the bitmap and read *it* in...                                    */
						if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
							bitsize = ((*info)->bmiHeader.biWidth *
									(*info)->bmiHeader.biBitCount + 7) / 8 *
  								abs((*info)->bmiHeader.biHeight);

						if ((bits = malloc(bitsize)) == NULL)
							{
							/* Couldn't allocate memory - return NULL! */
							free(*info);
							fclose(fp);
							return (NULL);
							}

						if (fread(bits, 1, bitsize, fp) < bitsize)
							{
							/* Couldn't read bitmap - free memory and return NULL! */
							free(*info);
							free(bits);
							fclose(fp);
							return (NULL);
							}

						/* Swap red and blue */
						length = ((*info)->bmiHeader.biWidth * 3 + 3) & ~3;
						for (y = 0; y < (*info)->bmiHeader.biHeight; y ++)
							for (ptr = bits + y * length, x = (*info)->bmiHeader.biWidth;
								x > 0;
							x --, ptr += 3)
							{
							temp   = ptr[0];
							ptr[0] = ptr[2];
							ptr[2] = temp;
							}

						/* OK, everything went fine - return the allocated bitmap... */
						fclose(fp);
						printf("read ok!\n");
						return (bits);
						}


					/*
					* 'SaveDIBitmap()' - Save a DIB/BMP file to disk.
					*
					* Returns 0 on success or -1 on failure...
					*/

					int                                /* O - 0 = success, -1 = failure */
					SaveDIBitmap(const char *filename, /* I - File to load */
								BITMAPINFO *info,     /* I - Bitmap information */
							GLubyte    *bits)     /* I - Bitmap data */
						{
						FILE *fp;                      /* Open file pointer */
						int  size,                     /* Size of file */
							infosize,                 /* Size of bitmap info */
							bitsize;                  /* Size of bitmap pixels */


						/* Try opening the file; use "wb" mode to write this *binary* file. */
						if ((fp = fopen(filename, "wb")) == NULL)
							return (-1);

						/* Figure out the bitmap size */
						if (info->bmiHeader.biSizeImage == 0)
						bitsize = (info->bmiHeader.biWidth *
        						info->bmiHeader.biBitCount + 7) / 8 *
							abs(info->bmiHeader.biHeight);
						else
						bitsize = info->bmiHeader.biSizeImage;

						/* Figure out the header size */
						infosize = sizeof(BITMAPINFOHEADER);
						switch (info->bmiHeader.biCompression)
						{
						case BI_BITFIELDS :
								infosize += 12; /* Add 3 RGB doubleword masks */
								if (info->bmiHeader.biClrUsed == 0)
							break;
						case BI_RGB :
								if (info->bmiHeader.biBitCount > 8 &&
        						info->bmiHeader.biClrUsed == 0)
							break;
						case BI_RLE8 :
						case BI_RLE4 :
								if (info->bmiHeader.biClrUsed == 0)
								infosize += (1 << info->bmiHeader.biBitCount) * 4;
							else
								infosize += info->bmiHeader.biClrUsed * 4;
							break;
						}

						size = sizeof(BITMAPFILEHEADER) + infosize + bitsize;

						/* Write the file header, bitmap information, and bitmap pixel data... */
						write_word(fp, BF_TYPE);        /* bfType */
						write_dword(fp, size);          /* bfSize */
						write_word(fp, 0);              /* bfReserved1 */
						write_word(fp, 0);              /* bfReserved2 */
						write_dword(fp, 18 + infosize); /* bfOffBits */

						write_dword(fp, info->bmiHeader.biSize);
						write_long(fp, info->bmiHeader.biWidth);
						write_long(fp, info->bmiHeader.biHeight);
						write_word(fp, info->bmiHeader.biPlanes);
						write_word(fp, info->bmiHeader.biBitCount);
						write_dword(fp, info->bmiHeader.biCompression);
						write_dword(fp, info->bmiHeader.biSizeImage);
						write_long(fp, info->bmiHeader.biXPelsPerMeter);
						write_long(fp, info->bmiHeader.biYPelsPerMeter);
						write_dword(fp, info->bmiHeader.biClrUsed);
						write_dword(fp, info->bmiHeader.biClrImportant);

						if (infosize > 40)
						if (fwrite(info->bmiColors, infosize - 40, 1, fp) < 1)
								{
								/* Couldn't write the bitmap header - return... */
								fclose(fp);
								return (-1);
								}

						if (fwrite(bits, 1, bitsize, fp) < bitsize)
							{
							/* Couldn't write the bitmap - return... */
							fclose(fp);
							return (-1);
							}

						/* OK, everything went fine - return... */
						fclose(fp);
						return (0);
						}


					/*
					* 'read_word()' - Read a 16-bit unsigned integer.
					*/

					static unsigned short     /* O - 16-bit unsigned integer */
					read_word(FILE *fp)       /* I - File to read from */
						{
						unsigned char b0, b1; /* Bytes from file */

						b0 = getc(fp);
						b1 = getc(fp);

						return ((b1 << 8) | b0);
						}


					/*
					* 'read_dword()' - Read a 32-bit unsigned integer.
					*/

					static unsigned int               /* O - 32-bit unsigned integer */
					read_dword(FILE *fp)              /* I - File to read from */
						{
						unsigned char b0, b1, b2, b3; /* Bytes from file */

						b0 = getc(fp);
						b1 = getc(fp);
						b2 = getc(fp);
						b3 = getc(fp);

						return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
						}


					/*
					* 'read_long()' - Read a 32-bit signed integer.
					*/

					static int                        /* O - 32-bit signed integer */
					read_long(FILE *fp)               /* I - File to read from */
						{
						unsigned char b0, b1, b2, b3; /* Bytes from file */

						b0 = getc(fp);
						b1 = getc(fp);
						b2 = getc(fp);
						b3 = getc(fp);

						return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
						}


					/*
					* 'write_word()' - Write a 16-bit unsigned integer.
					*/

					static int                     /* O - 0 on success, -1 on error */
					write_word(FILE           *fp, /* I - File to write to */
							unsigned short w)   /* I - Integer to write */
						{
						putc(w, fp);
						return (putc(w >> 8, fp));
						}


					/*
					* 'write_dword()' - Write a 32-bit unsigned integer.
					*/

					static int                    /* O - 0 on success, -1 on error */
					write_dword(FILE         *fp, /* I - File to write to */
								unsigned int dw)  /* I - Integer to write */
						{
						putc(dw, fp);
						putc(dw >> 8, fp);
						putc(dw >> 16, fp);
						return (putc(dw >> 24, fp));
						}


					/*
					* 'write_long()' - Write a 32-bit signed integer.
					*/

					static int           /* O - 0 on success, -1 on error */
					write_long(FILE *fp, /* I - File to write to */
							int  l)   /* I - Integer to write */
						{
						putc(l, fp);
						putc(l >> 8, fp);
						putc(l >> 16, fp);
						return (putc(l >> 24, fp));
						}

					#endif /* WIN32 */
		}	// namespace BitmapIO
	}	// namespace IO
}	// namespace LLib
