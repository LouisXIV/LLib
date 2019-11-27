#include "../../header/algebra3/algebra3.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef __WXMSW__
	#include <Windows.h>
	#include <Commdlg.h>
#else
	#include <wx\\wx.h>
#endif

#include "../../header/bitmapIO/bitmap.h"
#include "../../header/bitmapIO/IO.h"
using namespace LLib::Math;

using namespace LLib::IO::BtimapIO;

#ifndef SWAP
#define SWAP( A, B, T) ( (T) = (A), (A) = (B), (B) =(T))
#endif


namespace LLib
{
	namespace IO
	{
//		void getCurrentDirectory(char*buffer, int bufferSize)
//		{
//			DWORD GetCurrentDirectory(bufferSize,	buffer);
//		}

		int isFileExisted(const char* fname)
		{
			FILE * fp = fopen( fname, "r");
			if (fp==NULL)
			{
				return 0;
			}
			fclose(fp);
			return 1;
		}

		int getFileName(char * buffer, int bufferSize, char * fileFilter, char * title)
		{
			char currentWorkingDirectory[512];

			GetCurrentDirectory( 512,	currentWorkingDirectory);

			OPENFILENAME ofn;
//			char fileFilterStr[512];

//			sprintf(fileFilterStr, "%s", fileFilter);

			LPOPENFILENAME lpofn = (LPOPENFILENAME) &ofn;
			if(!buffer)
			{
				printf("IO::getFileName() ==> Buffer is NULL!!\n");
				return 1;
			}

			// open file
			{
				memset(lpofn, 0, sizeof(OPENFILENAME));
				ofn.hwndOwner	= NULL;
				ofn.hInstance		= NULL;
				ofn.lpstrCustomFilter	= NULL;
				ofn.nMaxCustFilter	= NULL;
				ofn.lpstrInitialDir		= NULL;
				ofn.nFileOffset	= NULL;
				ofn.lStructSize	= sizeof (OPENFILENAME);

				ofn.lpstrFilter	= fileFilter;
				ofn.nFilterIndex		= 1;
				ofn.lpstrFile		= new CHAR[512];		// as output  and MUST BE NULL
				ofn.nMaxFile		= 512;
				for (int i=0; i<ofn.nMaxFile; i++)
					ofn.lpstrFile[i] = 0;
				ofn.lpstrFileTitle		= new CHAR[256];
				ofn.nMaxFileTitle		= 256;
				for (int i=0; i<ofn.nMaxFileTitle; i++)
					ofn.lpstrFileTitle[i] = 0;
				ofn.lpstrTitle			= title;
				//			ofn.Flags					= OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
			}
			if( GetOpenFileName( lpofn ) )
			{
				//MessageBox(NULL, "true", NULL, MB_OK);
					GetFileTitle( lpofn->lpstrFile,lpofn->lpstrFileTitle, 256 );
				#ifdef _DEBUG
					printf("\nFileName: %s\n", lpofn->lpstrFileTitle);
					printf("FullFileName: %s\n", lpofn->lpstrFile);
				#endif // _DEBUG
					if(strlen(lpofn->lpstrFile)< bufferSize)
						sprintf(buffer, "%s", lpofn->lpstrFile);
					else
					{
						printf("IO::getFileName() ==> Buffer is too small!!\n");
						return 2;
					}
			}
			else
			{
				#ifdef _DEBUG
					DWORD err = CommDlgExtendedError();
					MessageBox(NULL, "false", NULL, MB_OK);
				#endif //_DEBUG
					printf("IO::getFileName() ==> OpenFile Fail!!\n");
					return 1;
			}

			SetCurrentDirectory( currentWorkingDirectory);
			
			return 0;
		}



		void outputCamera(const char * filename, vec3 p, vec3 d, vec3 u, double angel)
		{
			FILE * fp = fopen(filename, "w");

			if (NULL == fp)
			{
				printf("file open error!\n");
				return ;
			}
			else
			{
				fprintf(fp, "%lf %lf %lf\n", p[0], p[1], p[2]);
				fprintf(fp, "%lf %lf %lf\n", d[0], d[1], d[2]);
				fprintf(fp, "%lf %lf %lf\n", u[0], u[1], u[2]);
				fprintf(fp, "%lf", angel);
			}
			fflush(fp);
			fclose(fp);
		};


		namespace BtimapIO
		{
			//************************************
			// Method:    outputScreenAsBitArray
			// FullName:  LLib::IO::BtimapIO::outputScreenAsBitArray
			// Access:    public 
			// Returns:   void
			// Qualifier: output screen as bits array with file name, fname.")
			// Parameter: char * filename
			//************************************

			void outputScreenAsBitArray(const char * fname)
			{
				int channelNumber = 3;
				GLubyte * buffer	;// = (GLubyte*) malloc(sizeof(GLubyte) * width * height * 3);
				BITMAPINFO * info;
				glReadBuffer(GL_BACK); // Sets the Reading Source to Offscreen(Back) Buffer
				glFinish() ;
				//	glReadPixels( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);

				buffer = ReadDIBitmap(&info, channelNumber);

				FILE * fp = fopen(fname, "bw");
				if(fp!=NULL)
				{
					fwrite(buffer, 1,info->bmiHeader.biBitCount, fp);
					fclose(fp);
				}

			}

			//************************************
			// Method:    outputBMP
			// FullName:  LLib::IO::BtimapIO::outputBMP
			// Access:    public 
			// Returns:   void
			// Qualifier: output screen as bitmap with file name, fname. (this is an old function with wrong name. new function is "outputScreenAsBitmap()")
			// Parameter: char * filename
			//************************************
			void outputBMP(const char * filename)
			{
				outputScreenAsBitmap(filename);
			};


			//************************************
			// Method:    outputScreenAsBitmap
			// FullName:  LLib::IO::BtimapIO::outputScreenAsBitmap
			// Access:    public 
			// Returns:   void
			// Qualifier: output screen as bitmap with fname.
			// Parameter: char * filename
			//************************************
			void outputScreenAsBitmap(const char * filename, int channelNumber /* = 3 */)
			{
				GLubyte * buffer	;// = (GLubyte*) malloc(sizeof(GLubyte) * width * height * 3);
				BITMAPINFO * info;
				glReadBuffer(GL_BACK); // Sets the Reading Source to Offscreen(Back) Buffer
				glFinish() ;
			//	glReadPixels( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);

				buffer = ReadDIBitmap(&info, channelNumber);
				SaveDIBitmap(filename, info, buffer);
				free(buffer);
				free(info);

				return;
			};
			void outputTextureAsBitmap( const char * filename, GLuint texID, int width, int height, int channelNumber /*= 3*/ )
			{
				GLubyte * bits;
				BITMAPINFO * info;

				SetBitmapHeader(&info, width, height, channelNumber);
				if((bits = (GLubyte*)calloc(info->bmiHeader.biSizeImage,1)) == NULL)
				{
					free(info);
					return ;
				}
				glBindTexture(GL_TEXTURE_2D, texID);
				if (channelNumber==4)
				{
					glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);
				}
				else
				{
					glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, bits);
				}

				if(bits != NULL)		// SWAP:  BGR to RGB
				{
					int i,j, index;
					GLubyte tmp;
					for(i=0; i< info->bmiHeader.biWidth; i++)
						for(j=0; j< info->bmiHeader.biHeight; j++)
						{
							index = channelNumber*(j*info->bmiHeader.biWidth + i);
							SWAP(bits[index+0], bits[index+2], tmp);
						}
				}

				SaveDIBitmap(filename, info, bits);
				free(bits);
				free(info);
			}
		}	// namespace BitmapIO

		namespace ImageObjects
		{
			LBitmapObject::LBitmapObject()
			{
				this->bits = NULL;
				this->info = NULL;
			}
			LBitmapObject::~LBitmapObject()
			{
				if(this->bits!=NULL)	free(this->bits);
				if(this->info!=NULL)	free(this->info);
			}

			GLubyte * LBitmapObject::GetPixel(int x, int y)
			{
				int index = channelNumber*(y* this->info->bmiHeader.biWidth + x);
				return &bits[index];
			}
			const GLubyte * LBitmapObject::GetPixel(int x, int y) const
			{
				int index = channelNumber*(y* this->info->bmiHeader.biWidth + x);
				return (const GLubyte *) &bits[index];
			}

			GLubyte * LBitmapObject::SetPixel(int x, int y, GLubyte * colors)
			{
				int index = channelNumber*(y* this->info->bmiHeader.biWidth + x);
				for (int i=0;i<channelNumber; ++i)
				{
					bits[index+i] = colors[i];
				}
				return &bits[index];
			}


			bool LBitmapObject::LoadBMP(int width, int height, int nChannels)
			{
				if(this->bits!=NULL)
					free(this->bits);
				if(this->info!=NULL)
					free(this->info);

				channelNumber = nChannels;

				SetBitmapHeader(&this->info, width, height, nChannels);
				if(this->info == NULL)
				{
					return false;
				}
				if((this->bits = (GLubyte*)calloc(this->info->bmiHeader.biSizeImage,1)) == NULL)
				{
					free(this->info);
					return false;
				}

				return true;
			}

			int LBitmapObject::LoadBMP( const char* fname)
			{
				if(this->bits!=NULL)
					free(this->bits);
				if(this->info!=NULL)
					free(this->info);
				this->bits = NULL;
				this->info = NULL;
				this->bits = LoadDIBitmap(fname, &this->info);


				if (info==NULL)
				{
					return -1;
				}

				if (info->bmiHeader.biBitCount == 32)
					channelNumber = 4;
				else
					channelNumber = 3;

				if(this->bits != NULL)		// SWAP:  BGR to RGB
				{
					int i,j, index;
					GLubyte tmp;
					for(i=0; i< this->info->bmiHeader.biWidth; i++)
						for(j=0; j< this->info->bmiHeader.biHeight; j++)
						{
							index = channelNumber*(j*this->info->bmiHeader.biWidth + i);
							SWAP(this->bits[index+0], this->bits[index+2], tmp);
						}
						return 1;
				}
				return 0;

			};

			void LBitmapObject::SaveBMP(const char * fname)
			{
				if(this->bits != NULL)		
				{
					// SWAP:  RGB to BGR
					int i,j, index;
					GLubyte tmp;
					for(i=0; i< this->info->bmiHeader.biWidth; i++)
						for(j=0; j< this->info->bmiHeader.biHeight; j++)
						{
							index = channelNumber*(j*this->info->bmiHeader.biWidth + i);
							SWAP(this->bits[index+0], this->bits[index+2], tmp);
						}

						// save to file
						SaveDIBitmap(fname, this->info, this->bits);

						// SWAP:  BGR to RGB
						for(i=0; i< this->info->bmiHeader.biWidth; i++)
							for(j=0; j< this->info->bmiHeader.biHeight; j++)
							{
								index = channelNumber*(j*this->info->bmiHeader.biWidth + i);
								SWAP(this->bits[index+0], this->bits[index+2], tmp);
							}

				}

				return ;
			}

			double LBitmapObject::PSNR( const LBitmapObject &im1, const LBitmapObject & im2 )
			{
				int width = im1.getWidth();
				int height= im1.getHeight();
				if(width != im2.getWidth() || height!=im2.getHeight())
				{
					fprintf(stdout, "Image size does not matched!! image1(%d,%d) != image2(%d,%d)\n", 
								im1.getWidth(), im1.getHeight(), 
								im2.getWidth(), im2.getHeight());
					return -1;
				}

				double mse = 0;

				for (int y=0; y<height; y++)
					for (int x=0; x<width; x++)
					{
						const GLubyte * c0 = im1.GetPixel(x,y);
						const GLubyte * c1 = im2.GetPixel(x,y);

						for (int c=0; c<3; ++c)
						{
							double dif = (c0[c] - c1[c]);
							dif = (dif * dif) / (height*width*3);
							mse += dif ;
						}
					}

				double PSNR = 10 * log10(255.0*255.0 / mse);

				return PSNR;
			}
		} // namespace ImageObjects
	}	// namespace IO
}	// namespace LLib