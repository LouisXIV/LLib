#ifndef _LRENDERTEXTUREOBJECT_H_
#define _LRENDERTEXTUREOBJECT_H_
#include <stdio.h>
#include "../fboClass/framebufferObject.h"
using namespace LLib::NV;

namespace LLib
{
	namespace Graphics
	{
		namespace Texture
		{
			/**
			 *		\brief object used to render to texture by Frame Buffer Object API
			 *
			 *
			 **/
				class LRenderTextureObject
					{
					public:
						/**
						*		constructor
						*		
						*		\param w						Texture Width
						*		\param h						Texture Height
						*		\param colorInternal The Internal Format of Color Texture 
						*		\param colorFormat		...
						*		\param colorType		...
						*		\param texWarpS		...
						*		\param texWarpS		...
						*		\param texMagFilter	....
						*		\param texMinFilter	...
						**/
						LRenderTextureObject( int w, int h,
										GLint colorInternal = GL_RGBA,
										GLenum colorFormat = GL_RGBA,
										GLenum colorType = GL_FLOAT,
										GLint texWarpS = GL_CLAMP,
										GLint texWarpT = GL_CLAMP,
										GLint texMagFilter = GL_LINEAR,
										GLint texMimFilter = GL_LINEAR
										);

						~LRenderTextureObject();

						/**
						*	Begin(), 
						**/
						inline void Begin()	
						{	this->fbo->Bind();
							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
							glViewport(0,0,this->width, this->height);
							return; 
						};

						/**
						*	End():
						*	\param screenWidth
						*	\param screenHeight
						**/
						inline void End(int screenWidth, int screenHeight) 
						{
							FramebufferObject::Disable();
							glViewport(0,0, screenWidth, screenHeight);
							return ;
						};

						GLuint getColorTex() { return tex[L_RTO_TEX_COLOR0]; };
						GLuint getDepthTex() { return tex[L_RTO_TEX_DEPTH]; };
						void keepColorTexAfterDelete() { this->keepTexAfterDelete[L_RTO_TEX_COLOR0] = true; return;};
						void keepDepthTexAfterDelete() { this->keepTexAfterDelete[L_RTO_TEX_DEPTH] = true; return;};
						int getWidth()	{ return this->width;};
						int getHeight()	{ return this->height;};

					private:
						enum
						{
							L_RTO_TEX_COLOR0,
							L_RTO_TEX_DEPTH
						};
						int width;
						int height;
						GLuint tex[2];		// tex[DEPTH, COLOR]
						FramebufferObject *fbo;
						bool keepTexAfterDelete[2];
					};


		} // namespace Texture
	}	// Graphics
}	// LLib

#endif	// _LRENDERTEXTUREOBJECT_H_