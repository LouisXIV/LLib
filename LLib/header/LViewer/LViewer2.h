/************************************************************************
		OpenGL Viewer using "glut"
-------------------------------------------------------------------------

	Version 1.2 - August 10 2004

-------------------------------------------------------------------------
	Author: Chen Ying-Chieh

*************************************************************************/

#ifndef _LVIEWER2_H_
#define _LVIEWER2_H_


#include <stdlib.h>

#include <gl/glew.h>
#ifndef USE_GLEW
	#define USE_GLEW
#endif

#include <gl/glut.h>
#include <gl/glui.h>

#include "../algebra3/algebra3.h"
#include "../LGlobal/LGlobal.h"
#include "../LTranslateMatrix/LTranslateMatrix.h"
using namespace LLib::Math;

namespace LLib
{

	namespace Viewer
	{
		/* =====================================================*/
		class LViewer2
		{
			enum {
				KEYDOWN = 0,
				KEYUP = 1,
				FINISH = 1,
				UNFINISH = 0
			};

			public:
				int win_id;
				GLUI * gluiContext;

				LTranslateMatrix *controlMatrix;
				LTranslateMatrix *currentViewMatrix;
				LTranslateMatrix viewMatrix;

				bool* isFlagRegistered;
				int * flag;
				int * flagN;
			
				vec3 viewerPosition;
				vec3 viewerDirection;			// z
				vec3 viewerUpDirection;		// y
				vec3 viewerLeftDirection;	// x
				vec3 ballTracingCenter;
				vec3 modelCenter;
				double seeda;
				double phi;
				double DirLength;

				float FOV;
				int width;
				int height;

				double nearPlane;
				double farPlane;


		// ず场 status 把计
				int	  currentFlagN;
				double preX;
				double preY;
				double prepreX;
				double prepreY;

				int mouseLKey;
				int mouseRKey;
				int mouseMKey;

				float moveSpeedMode;
				bool modelCenterMode;
				bool rotating;
				int *rotatingMouseKey;
		// ず场 function points
		
				void (*m_drawModel)		(void);
				int  (*m_keyboard)		(unsigned char key, int x, int y);
				int  (*m_mouseFun)		(int button, int state, int x, int y);
				int  (*m_motionFun)		(int x, int y);

		private:	
				void (*m_init)			(void);
				void (*m_idlef)			(void);
				void (*m_changScreenSize)	(int x, int y);
				
		/*
				void (*renderScreen)		(void);
				void (*specialKey)		(int key, int x, int y);
				
				
		*/
				bool m_isFullScreen;
				int tmpWinX;	// for full screen
				int tmpWinY;	// for full screen
				int tmpWinW;
				int tmpWinH;


				

		private:	
				LViewer2();
				~LViewer2();
		public:
				void createWindow(void (*renderFunc)(void),	void (*idleFunc)(void),	void (*initFunc)(void), int width=-1, int height=-1, float nearPlane = -1, float farPlane = -1);
				void showWindowByGlut();
				void init_gl(void);
				void _idle(void);

				void enableGLUI()			{ LViewer2::gluiContext = (GLUI*) 1; return; };

				void setCurrentControlMatrix( LTranslateMatrix * matrix = &LViewer2::viewMatrix)	{ LViewer2::controlMatrix = matrix; };
				void setCurrentViewMatrix( LTranslateMatrix * matrix = &LViewer2::viewMatrix)	{ LViewer2::currentViewMatrix = matrix; };

				void setModelCenter			( vec3 value )	{LViewer2::modelCenterMode = true; LViewer2::controlMatrix->setSphereCenter3v( LViewer2::modelCenter = value);};
				void setModelCenter			( vec4 value )	{LViewer2::modelCenterMode = true; LViewer2::controlMatrix->setSphereCenter3v(LViewer2::modelCenter = vec3 (value));};
				void setDrawModelFunc		( void (*drawModelP)		(void))									{ m_drawModel = drawModelP;};
				void setInitFunc				( void (*init)				(void))									{ m_init = init;};
				void setIdleFunc				( void (*idle)				(void))									{ m_idlef = idle;};
				void setKeyboardFunc			( int  (*keyboard)			(unsigned char key, int x, int y))		{ m_keyboard = keyboard;};
				void setMouseFunc			( int  (*mouseFun)			(int button, int state, int x, int y))	{ m_mouseFun = mouseFun;}; 
				void setMotionFunc			( int  (*motionP)			(int x, int y))							{ m_motionFun = motionP;};
				void setChangScreenSizeFunc	( void (*changScreenSize)	(int w, int h))							{ m_changScreenSize = changScreenSize; };

				void setFOV					( float fov)	{LViewer2::FOV = fov;};
				float getFOV					( )				{return LViewer2::FOV;};

				void	setWidth				( int width)				{LViewer2::width = width;};
				void setHeight				( int height)				{LViewer2::height = height;};
				void setWindowSize			( int width, int height)	{LViewer2::width = width; LViewer2::height = height;}
				int	getWidth				()				{ return LViewer2::width;};
				int	getHeight				()				{ return LViewer2::height;};
				void updateScreenSize		()				{ LViewer2::width = glutGet((GLenum)GLUT_WINDOW_WIDTH); LViewer2::height = glutGet((GLenum)GLUT_WINDOW_HEIGHT);};

				void		setNearPlane			( double n)				{LViewer2::nearPlane = n;};
				void		setFarPlane				( double f)				{LViewer2::farPlane = f;};
				void		setNearFarPlane			( double n, double f)	{LViewer2::nearPlane = n; LViewer2::farPlane = f;};
				double	getNearPlane			()			{return LViewer2::nearPlane;};
				double	getFarPlane				()			{return LViewer2::farPlane;};

		/*
				void setRenderScerrnFunc		( void (*renderScreenP)		(void))									{ renderScreen = renderScreenP;};
				void setchangeScreenSizeFunc	( void (*changeScreenSizeP)	(int x, int y))							{ changeScreenSize = changeScreenSizeP;};
				void setKeyboardFunc			( void (*keyboardP)			(unsigned char key, int x, int y))		{ keyboard = keyboardP;};
				void setSpecialKeyFunc		( void (*specialKeyP)		(int key, int x, int y))				{ specialKey = specialKeyP;};
				
		*/
		// Real actions functions
				void		setWinPos(int x, int y)						{	glutPositionWindow(x, y);};
				int		getWinPosX()								{	return glutGet((GLenum)GLUT_WINDOW_X);};
				int		getWinPosY()								{	return glutGet((GLenum)GLUT_WINDOW_Y);};
				void		getWinPos(int *winX, int *winY)				{	*winX = getWinPosY(); *winY = getWinPosY();};

				bool		isFullScreen()								{	return m_isFullScreen;};
				void		reshapeWindowSize(int width, int height)	{	m_isFullScreen = false; setWindowSize(width, height); glutReshapeWindow(width, height); };
				void		switchFullScreenAndWindow()	
				{
					if(!m_isFullScreen)	
					{
						tmpWinX = getWinPosX(); tmpWinY = getWinPosY();
						tmpWinW = getWidth();	tmpWinH = getHeight();
						glutFullScreen();
						m_isFullScreen = true;
					} else {	// else already full screen.
						reshapeWindowSize(tmpWinW, tmpWinH);
						setWinPos(	tmpWinX , tmpWinY );
						m_isFullScreen = false;
					}
				}; 
					


			private:
		};
	}
}
#endif