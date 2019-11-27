/*
 * Texture image definitions from Chapter 8.
 *
 * Written by Michael Sweet
 */

/*
 * Include necessary headers.
 */

#ifndef _TEXTURE_H_
#  define _TEXTURE_H_
#include <stdio.h>
#include <stdlib.h>
#  include "../bitmapIO/bitmap.h"
using namespace LLib::IO::BtimapIO;

namespace LLib
{
	namespace Graphics
	{
		namespace Texture
		{

			/*
			 * Make this header file work with C and C++ source code...
			 */

			#  ifdef __cplusplus
			extern "C" {
			#  endif /* __cplusplus */

			/**
			*		\brief Load Bitmap file as OpenGL texture object.
			*		GLuint LoadBitmapTexture( (char *filename, GLboolean alpha, GLenum minfilter, GLenum magfilter, GLenum wrap, GLenum type);
			*		\param filename:	file name
			*		\param alpha:		Generate alpha for bitmap
			*		\param minfilter:	Minification filter
			*		\param magfilter:	Magnification filter
			*		\param wrap:		Repeat or clamp	
			*		\param type:		-1 for auto detection Texture type.
			*		\return value:
			*							0: Error
			*							Texture object.
			**/
			extern GLuint LoadBitmapTexture(char *filename, GLboolean alpha,
									  GLenum minfilter, GLenum magfilter,  GLenum wrap, 
									  GLenum	  type = -1 /* = -1, auto detection Texture type */);


			/**
			*		\brief Load bmp bits file as OpenGL texture object. 
			*		GLuint LoadBitsTexture( BITMAPINFO *info, GLubyte *bits, GLboolean alpha, GLenum minfilter, GLenum magfilter, GLenum wrap, int isGray);
			*		\param info:		Bitmap information
			*		\param bits:		Bitmap RGB pixels
			*		\param alpha:		Generate alpha for bitmap
			*		\param minfilter:	Minification filter
			*		\param magfilter:	Magnification filter
			*		\param wrap:		Repeat or clamp	
			*		\param isGray:		-1 for auto detection Texture type.
			*		\return value:
			*							0: Error
			*							Texture object.
			**/
			extern
			GLuint									/* O - Texture object or 0 on error */
				LoadBitsTexture(BITMAPINFO	 *info,	/* Bitmap information */
				GLubyte      *bits,					/* Bitmap RGB pixels */
				GLboolean alpha,					/* I - Generate alpha for bitmap */
				GLenum    minfilter,				/* I - Minification filter */
				GLenum    magfilter,				/* I - Magnification filter */
				GLenum    wrap,						/* I - Repeat or clamp */
				int isGray);

			#  ifdef __cplusplus
			}
			#  endif /* __cplusplus */


			GLuint gen2DTextureObjectFromBits(
							void* bits, GLenum dataType,
							int width, int height,  int channelNumber,
							GLenum    minfilter = GL_LINEAR, GLenum magfilter = GL_LINEAR,
							GLenum    wrap = GL_REPEAT 
							);



		} // namespace Texture
	}	// Graphics
}	// LLib

#endif /* !_TEXTURE_H_ */

/*
 * End of "$Id: texture.h,v 1.2 1999/08/31 04:32:51 mike Exp $".
 */
