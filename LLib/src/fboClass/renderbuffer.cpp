/*
Copyright (c) 2005, 
	Aaron Lefohn	(lefohn@cs.ucdavis.edu)
	Adam Moerschell (atmoerschell@ucdavis.edu)
All rights reserved.

This software is licensed under the BSD open-source license. See
http://www.opensource.org/licenses/bsd-license.php for more detail.

*************************************************************
Redistribution and use in source and binary forms, with or 
without modification, are permitted provided that the following 
conditions are met:

Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer. 

Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution. 

Neither the name of the University of Californa, Davis nor the names of 
the contributors may be used to endorse or promote products derived 
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/


#include "../../header/fboClass/renderbuffer.h"
#include <iostream>
using namespace std;



namespace LLib
{
	namespace NV
	{

Renderbuffer::Renderbuffer()
	: m_bufId(_CreateBufferId())
{}

Renderbuffer::Renderbuffer(GLenum internalFormat, int width, int height)
	: m_bufId(_CreateBufferId())
{
	Set(internalFormat, width, height);
}

Renderbuffer::~Renderbuffer()
{
	glDeleteRenderbuffersEXT(1, &m_bufId);
}

void Renderbuffer::Bind() 
{
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_bufId);
}

void Renderbuffer::Unbind() 
{
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}

void Renderbuffer::Set(GLenum internalFormat, int width, int height)
{
	int maxSize = Renderbuffer::GetMaxSize();
	if (width > maxSize || height > maxSize ) {
		cerr << "Renderbuffer::Renderbuffer() ERROR:\n\t"
			 << "Size too big (" << width << ", " << height << ")\n";
		return;
	}

	// Guarded bind
	GLint savedId = 0;
	glGetIntegerv( GL_RENDERBUFFER_BINDING_EXT, &savedId );
	if (savedId != m_bufId) {
		Bind();
	}

	// Allocate memory for renderBuffer
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, internalFormat, width, height );
	
	// Guarded unbind
	if (savedId != m_bufId) {
		Unbind();
	}
}

GLuint Renderbuffer::GetId() const 
{
	return m_bufId;
}

GLint Renderbuffer::GetMaxSize()
{
	GLint maxAttach = 0;
	glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE_EXT, &maxAttach );
	return maxAttach;
}

GLuint Renderbuffer::_CreateBufferId() 
{
	GLuint id = 0;
	glGenRenderbuffersEXT(1, &id);
	return id;
}

	}		// NV
}			// LLib