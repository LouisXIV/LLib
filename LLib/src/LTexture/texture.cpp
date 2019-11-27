/*
 * Texture image routines from Chapter 8.
 *
 * Written by Michael Sweet
 */

/*
 * Include necessary headers.
 */

#include "../../header/LTexture/texture.h"

namespace LLib
{
	namespace Graphics
	{
		namespace Texture
		{

			/*
			 * 'TextureLoad()' - Load a bitmap file and define it as a 1D or 2D texture.
			 *
			 * Returns 0 on success or -1 on error.
			 */

			GLuint									/* O - Texture object or 0 on error */
			LoadBitmapTexture(	char      *filename, /* I - Bitmap file to load */
								GLboolean alpha,     /* I - Generate alpha for bitmap */
								GLenum    minfilter, /* I - Minification filter */
								GLenum    magfilter, /* I - Magnification filter */
								GLenum    wrap,      /* I - Repeat or clamp */
								GLenum	  type		 /* = -1, auto detection Texture type */
							)
				{
				int         i;               /* Looping var */
				BITMAPINFO	*info;           /* Bitmap information */
				GLubyte	*bits;           /* Bitmap RGB pixels */
				GLubyte     *ptr;            /* Pointer into bit buffer */
				GLubyte     temp;            /* Swapping variable */
				GLuint      texture;         /* Texture object */

				int			channelNumber;

				/* Try loading the bitmap file... */
				bits = LoadDIBitmap(filename, &info);
				if (bits == (GLubyte *)0)
					return (0);

				if (info->bmiHeader.biBitCount == 32)
					channelNumber = 4;
				else
					channelNumber = 3;

			#ifdef WIN32 /* This already done by non-WIN32 LoadDIBitmap */
				/*
				 * Convert the bitmap data from BGR to RGB. Since texture images
				 * must be a power of two, and since we can figure that we won't
				 * have a texture image as small as 2x2 pixels, we ignore any
				 * alignment concerns...
				 */

				for (i = info->bmiHeader.biWidth * info->bmiHeader.biHeight, ptr = bits;
					 i > 0;
				 i --, ptr += channelNumber)
					{
				/* Swap red and blue */
				temp   = ptr[0];
				ptr[0] = ptr[2];
				ptr[2] = temp;
				}
			#endif /* WIN32 */

				/* Figure out the type of texture... */
				if(type == -1)	// auto dection texture type
				{
					 if (info->bmiHeader.biHeight == 1)
						type = GL_TEXTURE_1D;
					else
						type = GL_TEXTURE_2D;
				}

				/* Create and bind a texture object */
				glGenTextures(1, &texture);
				glBindTexture(type, texture);

				/* Set texture parameters */
				glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magfilter);
				glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minfilter);
				glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
				glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);
				glTexEnvi(type, GL_TEXTURE_ENV_MODE, GL_MODULATE);

				/*
				 * Set texture image; if the minification filter uses mip-mapping
				 * then use gluBuild2D/1DMipmaps() to load the texture...
				 */

				if (minfilter == GL_LINEAR || minfilter == GL_NEAREST)
				{
					if (channelNumber == 4)
						glTexImage2D(type, 0, 4, info->bmiHeader.biWidth,
							info->bmiHeader.biHeight, 0, GL_RGBA,
							GL_UNSIGNED_BYTE, bits);
					else
						glTexImage2D(type, 0, 3, info->bmiHeader.biWidth,
									info->bmiHeader.biHeight, 0, GL_RGB,
									GL_UNSIGNED_BYTE, bits);
				}
				else if (type == GL_TEXTURE_1D)
					gluBuild1DMipmaps(type, 3, info->bmiHeader.biWidth,
									  GL_RGB, GL_UNSIGNED_BYTE, bits);
				else
				{
					if (channelNumber == 4)
						gluBuild2DMipmaps(type, 4, info->bmiHeader.biWidth,
										  info->bmiHeader.biHeight, GL_RGBA,
										  GL_UNSIGNED_BYTE, bits);
					else
						gluBuild2DMipmaps(type, 3, info->bmiHeader.biWidth,
											info->bmiHeader.biHeight, GL_RGB,
											GL_UNSIGNED_BYTE, bits);
				}

				/* Free the bitmap and return... */
				free(info);
				free(bits);

				return (texture);
				}

			GLuint								/* O - Texture object or 0 on error */
			LoadBitsTexture(BITMAPINFO	 *info,	/* Bitmap information */
						GLubyte      *bits,		/* Bitmap RGB pixels */
						GLboolean alpha,		/* I - Generate alpha for bitmap */
						GLenum    minfilter,	/* I - Minification filter */
						GLenum    magfilter,	/* I - Magnification filter */
						GLenum    wrap,			/* I - Repeat or clamp */
						int isGray)
				{
				int         i;               /* Looping var */
				GLubyte     *ptr;            /* Pointer into bit buffer */
				GLubyte     temp;            /* Swapping variable */
				GLenum      type;            /* Texture type */
				GLuint      texture;         /* Texture object */

				/* Try loading the bitmap file... */
				if (bits == (GLubyte *)0)
					return (0);

			#ifdef WIN32 /* This already done by non-WIN32 LoadDIBitmap */
				/*
				 * Convert the bitmap data from BGR to RGB. Since texture images
				 * must be a power of two, and since we can figure that we won't
				 * have a texture image as small as 2x2 pixels, we ignore any
				 * alignment concerns...
				 */

				for (i = info->bmiHeader.biWidth * info->bmiHeader.biHeight, ptr = bits;
					 i > 0 && (!isGray);
				 i --, ptr += 1)
					{
				/* Swap red and blue */
				temp   = ptr[0];
				ptr[0] = ptr[2];
				ptr[2] = temp;
				}
			#endif /* WIN32 */

				/* Figure out the type of texture... */
				if (info->bmiHeader.biHeight == 1)
					type = GL_TEXTURE_1D;
				else
					type = GL_TEXTURE_2D;

				/* Create and bind a texture object */
				glGenTextures(1, &texture);
				glBindTexture(type, texture);

				/* Set texture parameters */
				glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magfilter);
				glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minfilter);
				glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
				glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);
				glTexEnvi(type, GL_TEXTURE_ENV_MODE, GL_MODULATE);

				/*
				 * Set texture image; if the minification filter uses mip-mapping
				 * then use gluBuild2D/1DMipmaps() to load the texture...
				 */

				if (minfilter == GL_LINEAR || minfilter == GL_NEAREST)
					glTexImage2D(type, 0, 3, info->bmiHeader.biWidth,
								 info->bmiHeader.biHeight, 0, GL_LUMINANCE,
								 GL_UNSIGNED_BYTE, bits);
				else if (type == GL_TEXTURE_1D)
					gluBuild1DMipmaps(type, 3, info->bmiHeader.biWidth,
									  GL_LUMINANCE, GL_UNSIGNED_BYTE, bits);
				else
					gluBuild2DMipmaps(type, 3, info->bmiHeader.biWidth,
									  info->bmiHeader.biHeight, GL_LUMINANCE,
									  GL_UNSIGNED_BYTE, bits);

				/* Free the bitmap and return... */


				return (texture);
			};


			GLuint // return 0 if any error, or return the texture object.
				gen2DTextureObjectFromBits(		void *bits, GLenum dataType,
												int width, int height, int channelNumber,
												GLenum    minfilter,	/* = GL_LINEAR */
												GLenum    magfilter,	/* = GL_LINEAR */
												GLenum    wrap			/* = GL_REPEAT */)
			{
				int         i;               /* Looping var */
				GLenum      type;            /* Texture type */
				GLuint      texture;         /* Texture object */

				/* Try loading the bitmap file... */
				if (bits == (void *)0)
					return (0);


				/* Figure out the type of texture... */
				if (height == 1)
					type = GL_TEXTURE_1D;
				else
					type = GL_TEXTURE_2D;

				/* Create and bind a texture object */
				glGenTextures(1, &texture);
				glBindTexture(type, texture);

				/* Set texture parameters */
				glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magfilter);
				glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minfilter);
				glTexParameteri(type, GL_TEXTURE_WRAP_S, wrap);
				glTexParameteri(type, GL_TEXTURE_WRAP_T, wrap);
				glTexEnvi(type, GL_TEXTURE_ENV_MODE, GL_MODULATE);

				/*
				* Set texture image; if the minification filter uses mip-mapping
				* then use gluBuild2D/1DMipmaps() to load the texture...
				*/

				if (minfilter == GL_LINEAR || minfilter == GL_NEAREST)
				{
					if (channelNumber == 4)
						glTexImage2D(type, 0, 4, width, height, 0, GL_RGBA,	dataType, bits);
					else
						glTexImage2D(type, 0, 3, width, height, 0, GL_RGB, dataType, bits);
				}
				else if (type == GL_TEXTURE_1D)
					gluBuild1DMipmaps(type, 3, width, GL_RGB, dataType, bits);
				else
				{
					if (channelNumber == 4)
						gluBuild2DMipmaps(type, 4, width, height, GL_RGBA, dataType, bits);
					else
						gluBuild2DMipmaps(type, 3, width, height, GL_RGB, dataType, bits);
				}
				return (texture);
			}

		} // namespace Texture
	}	// Graphics
}	// LLib
