#pragma once

#include "../../header/algebra3/algebra3.h"
#include <stdio.h>


using namespace LLib::Math;

namespace LLib
{
	namespace IO
	{
		namespace ImageObjects
		{
			class LFloatArrayObject
			{
			protected:
				int m_width;
				int m_height;
				int m_nChannel;
				vector<float> m_fbits;

			public:
				LFloatArrayObject()
				{
					clear();
				}
				LFloatArrayObject(const int width, const int height, const int nChannel, const float * fbits)
				{
					m_width		= width;
					m_height	= height;
					m_nChannel	=nChannel;
					m_fbits.resize(width*height*nChannel);
					memcpy( &(m_fbits[0]), fbits, width*height*nChannel*sizeof(float));
				};
				~LFloatArrayObject()
				{
					m_fbits.clear();
				}


				bool load(const char* filename)
				{
					FILE* fp = fopen(filename, "rb");
					bool ret = false;
					if((fp!=NULL))
					{
						ret = read(fp);
						ret &= (0==fclose(fp));
					}
					return ret;
				}

				bool save(const char* filename) const
				{
					FILE* fp = fopen(filename, "wb");
					bool ret = false;
					if((fp!=NULL))
					{
						ret = write(fp);
						ret &= (0==fclose(fp));
					}
					return ret;
				}


				//	const float * getPixel(int x, in y);
				//	return value:¡@NULL for error, other the pointer to the pixel value
				bool setPixel(const int x, const int y, const float * pixel)
				{
					if(!isValidCoord(x,y))				{	return false; }
					for (int c=0; c<m_nChannel; ++c)	{	m_fbits[(y*m_width + x)*m_nChannel + c] = pixel[c];	}	return true;
				}
				const float * getPixel(const int x, const int y)	const	{	return ( isValidCoord(x,y))? (& m_fbits[(y*m_width+x)*m_nChannel]) : NULL;};
				const int getWidth()		const							{return m_width;};
				const int getHeight()		const							{return m_height;};
				const int getChannelNum()	const							{return m_nChannel;};
				bool compare(const LFloatArrayObject &fmap)	const			{ size_t size = m_width*m_height*m_nChannel;	return ( m_width== fmap.m_width && m_height== fmap.m_height && m_nChannel	== fmap.m_nChannel && (0 == memcmp(&m_fbits[0], &fmap.m_fbits[0], size)))? true:false;}

			private:
				inline bool isValidCoord(int x, int y)		const			{	return (x<0 || x >= m_width || y<0 || y>=m_height)? (false):(true);};

				void clear()	{
					m_width		= 0;
					m_height	= 0;
					m_nChannel	= 0;
					//if(m_fbits)	free(m_fbits);
					//m_fbits		= NULL;
					m_fbits.clear();
				};

				bool read(FILE * fp)
				{
					if( 1 != fread( &m_width,  sizeof(int), 1, fp) ||
						1 != fread( &m_height, sizeof(int), 1, fp) ||
						1 != fread( &m_nChannel, sizeof(int), 1, fp))
					{
						return false;
					}
					long size = m_width*m_height*m_nChannel;

					m_fbits.resize(size);
					if( size != fread(&m_fbits[0], sizeof(float), size, fp))
					{
						this->clear();
						return false;
					};
					return true;		
				}

				bool write(FILE * fp) const
				{
					long size = m_width*m_height*m_nChannel;
					if( 1 != fwrite( &m_width,  sizeof(int), 1, fp)	||
						1 != fwrite( &m_height, sizeof(int), 1, fp)	||
						1 != fwrite( &m_nChannel, sizeof(int), 1, fp)	||
						size != fwrite(&m_fbits[0],  sizeof(float), size, fp))
					{
						return false;
					}
					return true;		
				}

			};

		}
	}
}