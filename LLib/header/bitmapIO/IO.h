#ifndef _IO_H_
#define _IO_H_

#include "../../header/algebra3/algebra3.h"
#include "../../header/bitmapIO/bitmap.h"
#include <stdio.h>
using namespace LLib::Math;

namespace LLib
{
	namespace IO
	{
		/**
			*		\brief Check if a file existed. 
			*		int isFileExisted(const char * fname);
			*		\param fname: file name
			*		\return value:
			*			0: Not Existed
			*			1: existed.
			**/
			int isFileExisted(const char * buffer);

		/**
		  *		\brief Get a File Name String from File Dialog
		  *		int getFileName(char * buffer, int bufferSize);
		  *		\param buffer: if chosen a file, it will return the file's full Path and Name to this buffer
		  *		\param bufferSize: the size of this buffer for output
		  *		\param fileFilter: file filters. ex: "All file(*.*)\0*.*;\0JPEG File(*.jpg)\0*.jpg"
		  *		\param title: the pop box title. ex: "Open File:"
		  *		\return value:
		  *			0: Normal
		  *			1: don't choose any file or choose file fail.
		  *			2: the buffer size is too small.
		  **/
		int getFileName(char * buffer, int bufferSize, char * fileFilter = "All file(*.*)\0*.*;\0JPEG File(*.jpg)\0*.jpg", char *title = "Open File:");

		void outputCamera(const char * filename, vec3 p, vec3 d, vec3 u, double angel);

		namespace BtimapIO
		{
			void outputTextureAsBitmap(const char * filename, GLuint texID, int width, int height, int channelNumber = 3);
			void outputScreenAsBitmap(const char * filename, int channelNumber = 3 );
			void outputScreenAsBitArray(const char * fname);
			void outputBMP(const char * filename);
		}	// namespace BitmapIO

		namespace ImageObjects
		{
			class LBitmapObject
			{
			public:
				// variables
				GLubyte * bits;
				BITMAPINFO *info;
				int channelNumber;

				// functions
				LBitmapObject();
				~LBitmapObject();

				bool LoadBMP(int width, int height, int nChannels);
				int  LoadBMP( const char* fname);
				void SaveBMP( const char * fname);

				GLubyte *GetPixel(int x, int y);
				const GLubyte *GetPixel(int x, int y) const;
				GLubyte *SetPixel(int x, int y, GLubyte *);
				
				

				int getWidth()		const { return (info)? info->bmiHeader.biWidth : -1;};
				int getHeight()		const { return (info)? info->bmiHeader.biHeight: -1;};
				int getChannelNum()	const { return channelNumber;};

			public:
				static double PSNR(const LBitmapObject &im1, const LBitmapObject & im2);
			};	// class LBitmapObject
		}	// namespace ImageObjects
	}	// namespace IO
}	// namespace LLib
#endif