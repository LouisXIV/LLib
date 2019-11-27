#include "../../header/LRenderTextureObject/LRenderTextureObject.h"


namespace LLib
{
	namespace Graphics
	{
		namespace Texture
		{

				void reportError(int idx)
				{
					GLenum err;

					if( 0!=  (err = glGetError()))
					{
						printf("OpenGL error - %d!!(%d)\n", idx, err);
					}
				};

				LRenderTextureObject::LRenderTextureObject( int w, int h,
									GLint colorInternal,
									GLenum colorFormat,
									GLenum colorType,
									GLint texWarpS,
									GLint texWarpT,
									GLint texMagFilter,
									GLint texMinFilter
									)
				{
					this->keepTexAfterDelete[0] = false;
					this->keepTexAfterDelete[1] = false;
					this->width = w;
					this->height = h;

					glGenTextures( 2, this->tex);
					for (int i=0; i<2; i++)
					{
			
						glBindTexture(GL_TEXTURE_2D, this->tex[i]);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMagFilter);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMinFilter);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texWarpS);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texWarpT);
						//glTexEnvi(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_ENV_MODE, GL_MODULATE);
						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
						if (i == L_RTO_TEX_DEPTH)	// depth
						{
							glTexImage2D(	GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
								w, h, 0,
								GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			
						}
						else
						{
							glTexImage2D(	GL_TEXTURE_2D, 0, colorInternal,
								w, h, 0,
								colorFormat , colorType, NULL);
			
						}
						reportError(100+i);
					}

					this->fbo = new FramebufferObject();
					this->fbo->Bind();
						//glDrawBuffer( GL_NONE);
						glReadBuffer( GL_NONE);

					//	Attachment Color texture
						this->fbo->AttachTexture(GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->tex[L_RTO_TEX_COLOR0]);
//						this->fbo->AttachTexture(GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D,  this->tex[L_RTO_TEX_COLOR1]);
						this->fbo->IsValid();
					//	Attachment Depth texture
						this->fbo->AttachTexture( GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, this->tex[L_RTO_TEX_DEPTH]);
						this->fbo->IsValid();
					
			
					FramebufferObject::Disable();
			

				};


				LRenderTextureObject::~LRenderTextureObject()
				{
					if(!this->keepTexAfterDelete[0] && this->tex[0])	glDeleteTextures(1, &this->tex[0]);
					if(!this->keepTexAfterDelete[1] && this->tex[1])	glDeleteTextures(1, &this->tex[1]);
					if(this->fbo)		delete this->fbo;
				};

		} // namespace Texture
	}	// Graphics
}	// LLib