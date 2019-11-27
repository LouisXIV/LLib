/************************************************************************
		OpenGL Viewer using "glut"
-------------------------------------------------------------------------

	Version 1.2 - August 10 2004

-------------------------------------------------------------------------
	Author: Chen Ying-Chieh

*************************************************************************/

#include "../../header/LViewer/LViewer2.h"
#include "../../header/LTranslateMatrix/LTranslateMatrix.h"
using namespace LLib::Math;
#include "../../header/LTimer/LTimer.h"

#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>

namespace LLib
{
	namespace Viewer
	{

		/* =====================================================*/
			LTranslateMatrix LViewer2::viewMatrix(vec3( 0, 250, 0), vec3( 0, 0, 0), vec3(1,0,0));
			LTranslateMatrix * LViewer2::controlMatrix = &LViewer2::viewMatrix;
			LTranslateMatrix * LViewer2::currentViewMatrix = &LViewer2::viewMatrix;


		// class static initialize
			bool __isFlagRegistered[32] = {false};
			int __flagN[10];
			int __flag[32];

			bool* LViewer2::isFlagRegistered = __isFlagRegistered;
			int * LViewer2::flagN = __flagN;
			int * LViewer2::flag  = __flag;


			int LViewer2::win_id = 0;
			GLUI * LViewer2::gluiContext = NULL;


			vec3 LViewer2::viewerPosition		= vec3( 0, 250, 0);		// vec3( 0, 0, -50);
			vec3 LViewer2::viewerDirection		= vec3( 0, -1, 0);	// vec3( 0, 0, 1);	// z
			vec3 LViewer2::viewerUpDirection		= vec3( 1, 0, 0);	// vec3( 0, 1, 0);	// y
			vec3 LViewer2::viewerLeftDirection;							//	= vec3( 1, 0, 0);	// x
			vec3 LViewer2::ballTracingCenter;
			vec3 LViewer2::modelCenter			= vec3( 0, 0, 0);

			double	LViewer2::seeda			= 0;
			double	LViewer2::phi			= 0;
			double	LViewer2::DirLength		= 10;
			float	LViewer2::FOV			= 60;
			int		LViewer2::width			= 300;
			int		LViewer2::height			= 300;

			double	LViewer2::nearPlane		= 50.0;
			double	LViewer2::farPlane		= 3000.0;

			int		LViewer2::currentFlagN	=0;
			double	LViewer2::preX			=0;
			double	LViewer2::preY			=0;
			double	LViewer2::prepreX		=0;
			double	LViewer2::prepreY		=0;
			int		LViewer2::mouseLKey		= KEYUP;
			int		LViewer2::mouseRKey		= KEYUP;
			int		LViewer2::mouseMKey		= KEYUP;

			float	LViewer2::moveSpeedMode	= LViewer2_HIGH;
			bool	LViewer2::modelCenterMode= true;
			bool	LViewer2::rotating				= false;
			int*	LViewer2::rotatingMouseKey= NULL;

			void (*	LViewer2::m_drawModel)		(void);
			void (* LViewer2::m_init)			(void) = NULL;
			void (* LViewer2::m_idlef)			(void) = NULL;
			int  (* LViewer2::m_keyboard)		(unsigned char key, int x, int y) = NULL;
			void (* LViewer2::m_changScreenSize)	(int w, int h)	= NULL;
			int  (* LViewer2::m_mouseFun)		(int button, int state, int x, int y) = NULL;
			int  (* LViewer2::m_motionFun)		(int x, int y) = NULL;

		// private
			bool	LViewer2::m_isFullScreen = false;
			int		LViewer2::tmpWinX = -1;	// for full screen
			int		LViewer2::tmpWinY = -1;	// for full screen
			int		LViewer2::tmpWinW = -1;	// for full screen
			int		LViewer2::tmpWinH = -1;	// for full screen
			
			

		// Function List
		void _renderScreen(void);
		void _changScreenSize(int x, int y);
		void _keyboard(unsigned char key, int x, int y);
		void _specialKey(int key, int x, int y);
		void _mouse(int button, int state, int x, int y);
		void _init_gl(void);





		// Globe Variable List




		void _changScreenSize(int w, int h)
		{
			
			GLfloat aspect;

			glViewport(0, 0, w, h);
			LViewer2::width = w;
			LViewer2::height = h;

			glMatrixMode (GL_PROJECTION);
			glLoadIdentity();
			if (h==0) h = 1;

			aspect = w/(float)h;

			gluPerspective(	LViewer2::FOV, aspect, LViewer2::getNearPlane(), LViewer2::getFarPlane());


			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			return ;
		}



		void _keyboard(unsigned char key, int x, int y)
		{

			// user defined keyboard function
			if (LViewer2::m_keyboard != NULL)
			{
				if( Viewer::FINISH == (*LViewer2::m_keyboard)(key, x, y))
					return ;
			}

			// default keyboard action
			if (key == 'q' || key == 'Q')
				exit(1);
			else if ( key == 'r' || key == 'R' )
			{
				LViewer2::controlMatrix->reset();
			}
			else if ( key =='s' || key == 'S')
			{
				if (LViewer2::moveSpeedMode == LViewer2_HIGH)
				{
					printf("Move Speed Mode Change to LOW.\n");
					LViewer2::moveSpeedMode = LViewer2_LOW;
					LViewer2::controlMatrix->setMoveSpeed( LViewer2_LOW );
				}
				else
				{
					printf("Move Speed Mode Change to HIGH.\n");
					LViewer2::moveSpeedMode = LViewer2_HIGH;
					LViewer2::controlMatrix->setMoveSpeed( LViewer2_HIGH );
				}

			}
			else if ( key == 'u' || key == 'U' )
				LViewer2::controlMatrix->move3DUpDown(1);
			else if ( key == 'd' || key == 'D' )
				LViewer2::controlMatrix->move3DUpDown(-1);

			else if( key == '+')
			{
			}
			else if (key == '-')
			{
			}

// flag [A-Za-z]
			if ( (key - 'a'<32) && (key -'a' >=0))
			{
				LViewer2::flag[key - 'a'] ^= 1;
			}
			else if ( (key - 'A'<32) && (key -'A' >=0))
			{
				LViewer2::flag[key - 'A'] ^= 1;
			}


// numbers [0-9]
			if ( (key - '0'<10) && (key -'0' >=0))
			{
				LViewer2::flagN[LViewer2::currentFlagN] = 0;
				LViewer2::currentFlagN = key - '0';
				LViewer2::flagN[key - '0'] = 1;
			}

			glutPostRedisplay();

		}



		void _specialKey(int key, int x, int y)
		{

			if(key == GLUT_KEY_UP)
				LViewer2::controlMatrix->move3DFrontBack(1);
			else if( key == GLUT_KEY_DOWN)
				LViewer2::controlMatrix->move3DFrontBack(-1);
			else if( key == GLUT_KEY_LEFT)
				LViewer2::controlMatrix->move3DLeftRight(-1);
			else if( key == GLUT_KEY_RIGHT)
				LViewer2::controlMatrix->move3DLeftRight(1);
			
			// Refresh the Window
			glutPostRedisplay();

		}

		void _motion(int x, int y)
		{
			// user defined keyboard function
			if (LViewer2::m_motionFun != NULL)
			{
				if( Viewer::FINISH == (*LViewer2::m_motionFun)( x, y))
					return ;
			}

			double difX, difY;

			if (	LViewer2::mouseLKey == KEYDOWN || 
					LViewer2::mouseRKey == KEYDOWN ||
					LViewer2::mouseMKey == KEYDOWN )			// 空間中遊走
			{	
				difX = -x + LViewer2::preX;
				difY = -y + LViewer2::preY;

				LViewer2::prepreX = LViewer2::preX;
				LViewer2::prepreY = LViewer2::preY;

				LViewer2::preX = x;
				LViewer2::preY = y;

			}

			if (LViewer2::mouseMKey == KEYDOWN)
			{
		//		printf("%lf %lf\n", difX, difY);
				LViewer2::controlMatrix->move3D(difX, difY);
			}

			else if (LViewer2::mouseLKey == KEYDOWN)
			{
				LViewer2::controlMatrix->moveView( difX, difY);
			}

			else if (LViewer2::mouseRKey == KEYDOWN)		// ball tarcing
			{
				LViewer2::controlMatrix->moveOnSphere(difX, difY);
			}


			glutPostRedisplay();

			return;
		}


		void _mouse(int button, int state, int x, int y)
		{
		// user defined keyboard function
			if (LViewer2::m_mouseFun != NULL)
			{
				if( Viewer::FINISH == (*LViewer2::m_mouseFun)(button, state, x, y))
					return ;
			}


		//	double difX, difY;

			LViewer2::rotating = false;
			if(button == 0 )				// mouse left key
			{
				if (state == KEYDOWN)			// left key down
				{
					LViewer2::rotating = false;
					LViewer2::prepreX = LViewer2::preX = x;
					LViewer2::prepreY = LViewer2::preY = y;
					LViewer2::mouseLKey = KEYDOWN;
				}
				else							// left key up
				{
					LViewer2::rotating = true;
					LViewer2::mouseLKey = KEYUP;
					LViewer2::rotatingMouseKey = &LViewer2::mouseLKey;
				}
			}

			if (button == 1)					// mouse middle
			{
				if (state == KEYDOWN)			// middle key down
				{
					LViewer2::rotating = false;
					LViewer2::prepreX = LViewer2::preX = x;
					LViewer2::prepreY = LViewer2::preY = y;
					LViewer2::mouseMKey = KEYDOWN;
				}
				else							// left key up
				{
					LViewer2::rotating = true;
					LViewer2::mouseMKey = KEYUP;
					LViewer2::rotatingMouseKey = &LViewer2::mouseMKey;
				}
			}

			if (button == 2)					// mouse right key
			{
				if (state == KEYDOWN)			// right key down
				{
					LViewer2::rotating = false;
					LViewer2::prepreX = LViewer2::preX = x;
					LViewer2::prepreY = LViewer2::preY = y;
					LViewer2::mouseRKey = KEYDOWN;
				}
				else							// right key up
				{
					LViewer2::rotating = true;
					LViewer2::mouseRKey = KEYUP;
					LViewer2::rotatingMouseKey = &LViewer2::mouseRKey;
				}
				
			}
		}

		void _renderScreen(void)
		{
		// FPS calculation:
			static unsigned int framecount = 0;
			static LLib::Timer::LTimer2 timer;
			double duration = timer.getTime(1);
			if (duration > 1.0)
			{
				char buf[64];
				sprintf(buf, "FPS: %4.2lf", framecount / duration);
				framecount = 0;
				glutSetWindowTitle(buf);
			}
			framecount++;


		// for FPS calculation begin  1/2
			//LARGE_INTEGER counterS, counterE, fre;
			//int fps = 1;
			//fps &= QueryPerformanceCounter( &counterS);
			
		// for FPS calculation end
		/*
			printf("viewer Postion: (%6.3lf, %6.3lf, %6.3lf) look at (%6.3lf, %6.3lf, %6.3lf)\n\tup (%6.3lf, %6.3lf, %6.3lf)\n", 
						LViewer2::viewerPosition[0] , LViewer2::viewerPosition[1] , LViewer2::viewerPosition[2],
						LViewer2::viewerDirection[0], LViewer2::viewerDirection[1], LViewer2::viewerDirection[2],
						LViewer2::viewerUpDirection[0], LViewer2::viewerUpDirection[1], LViewer2::viewerUpDirection[2]
				);
		//*/


			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			LViewer2::currentViewMatrix->applyView();

			
		// Model  begin
			glPushMatrix();
				(*LViewer2::m_drawModel)();
			glPopMatrix();
		// Model end
			

		// for FPS calculation end
		glutSwapBuffers();

		//// for FPS calculation begin 2/2
		//	fps &= QueryPerformanceCounter( &counterE);	// get end time
		//	fps &= QueryPerformanceFrequency( &fre);		// get sample frequence
		//	if(fps)
		//	{
		//		printf("\rfps:%lf", (fre.QuadPart / (double) (counterE.QuadPart - counterS.QuadPart)) );
		//	}
		}


		void LViewer2::init_gl(void){

//			LViewer2::viewMatrix = LTranslateMatrix();
//			LViewer2::controlMatrix	= &LViewer2::viewMatrix;
			int i;

			for (i=0; i<32; i++)
				LViewer2::flag[i] = 1;
			for (i=0; i<10; i++)
				LViewer2::flagN[i] = 0;
			LViewer2::flagN[0] = 1;
			LViewer2::currentFlagN = 0;


			printf("Use Mouse to handle direction.\nDirected Arrow and \'U\' \'D\' to move.\n");
// init glew()
#ifdef USE_GLEW
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				/* Problem: glewInit failed, something is seriously wrong. */
				fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
				exit(1);
			}
			fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif


			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glEnable(GL_DEPTH_TEST);
		//	glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);

			if (LViewer2::m_init != NULL)
			{
				LViewer2::m_init();
			}

		};

		void LViewer2::_idle()
		{
			if (	LViewer2::rotating && *LViewer2::rotatingMouseKey == KEYUP && 
					!(	fabs(-LViewer2::preX + LViewer2::prepreX) < 3 &&
						fabs(-LViewer2::preY + LViewer2::prepreY) < 3	)
				)
			{
				double x = LViewer2::preX;
				double y = LViewer2::preY;
				*LViewer2::rotatingMouseKey = KEYDOWN;
				LViewer2::preX = LViewer2::prepreX;
				LViewer2::preY = LViewer2::prepreY;
				_motion( x, y);
				*LViewer2::rotatingMouseKey = KEYUP;
			}
			if (LViewer2::m_idlef != NULL)
			{
				if( glutGetWindow() != LViewer2::win_id )
					glutSetWindow( LViewer2::win_id );
				LViewer2::m_idlef();
			}
			return;
		}

		void LViewer2::createWindow( void (*renderFunc)(void), void (*idleFunc)(void), void (*initFunc)(void),  int width /*= -1*/, int height /*= -1*/, float nearPlane /*= -1*/, float farPlane /*= -1*/ )
		{
			if(nearPlane != -1)	LLib::Viewer::LViewer2::setNearPlane(nearPlane);
			if(farPlane != -1)	LLib::Viewer::LViewer2::setFarPlane(farPlane);
			
			if(width != -1) LLib::Viewer::LViewer2::setWidth(width);
			if(height!= -1) LLib::Viewer::LViewer2::setHeight(height);

			LLib::Viewer::LViewer2::setDrawModelFunc(renderFunc);
			LLib::Viewer::LViewer2::setIdleFunc(idleFunc);
			LLib::Viewer::LViewer2::setInitFunc(initFunc);

			LLib::Viewer::LViewer2::showWindowByGlut();

			return ;
		}

		void LViewer2::showWindowByGlut()
		{

			if (!m_changScreenSize)
			{
				 m_changScreenSize = _changScreenSize;
			}

			glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
			glutInitWindowSize(LViewer2::width,LViewer2::height);

			LViewer2::win_id = glutCreateWindow("space viewer");

			if (LViewer2::gluiContext)
			{
				LViewer2::gluiContext = GLUI_Master.create_glui( "glui", 0, LViewer2::width+2, 0);  //create a GLUI object

				GLUI_Master.set_glutIdleFunc( _idle );
				GLUI_Master.set_glutSpecialFunc( _specialKey );
				GLUI_Master.set_glutKeyboardFunc(_keyboard);
				GLUI_Master.set_glutMouseFunc(_mouse);
				GLUI_Master.set_glutDisplayFunc(_renderScreen);
				GLUI_Master.set_glutReshapeFunc(m_changScreenSize);
			}
			else
			{
				glutIdleFunc(_idle);
				glutSpecialFunc(_specialKey);
				glutKeyboardFunc(_keyboard);
				glutMouseFunc(_mouse);

				glutDisplayFunc(_renderScreen);
				glutReshapeFunc(m_changScreenSize);
			}
			glutMotionFunc(_motion);
				
			init_gl();

			glutMainLoop();
		}

		LViewer2::LViewer2()	{};
		LViewer2::~LViewer2()	{};
	}	// Viewer
} // LLib
