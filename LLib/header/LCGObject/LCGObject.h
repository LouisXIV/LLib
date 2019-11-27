/************************************************************************
		OpenGL CG using "cg\cg.h"
-------------------------------------------------------------------------

	Version 0.1 - August 22 2005

-------------------------------------------------------------------------
	Author: Chen Ying-Chieh

*************************************************************************/

#include <cg\cg.h>
#include <Cg\cgGL.h>

#ifndef _LCGOBJECT_H_
#define _LCGOBJECT_H_

namespace LLib
{
	namespace Graphics
	{
		namespace CGUtility
		{
						class LCGObject
						{
							public:


						// ¤º³¡ function points
							private:
							int VPNum;
							int FPNum;
							char ** vpNames;
							char ** vpEntryFunc;
							char ** fpNames;
							char *** vpParams;
							char *** fpParams;
							char ** fpEntryFunc;
							CGcontext	myCgContext;
							CGprofile	vertexProfile, fragmentProfile;
							CGprogram*	myCgVertexProgram;
							CGprogram*	myCgFragmentProgram;
						//	CGerror		*myCgError;

							public:
								LCGObject(int VPNum = 1, int FPNum = 1, CGprofile fpProfile = (CGprofile) NULL, CGprofile vpProfile = (CGprofile)NULL);
								~LCGObject();
						// load programs, (set and load)
								void	SetVertexProgram (int index, char* fname, char* enterFunction = "main", const char ** params = NULL);	//set and load program
								void	SetFragmentProgram(int inedex, char* fname, char* enterFunction = "main", const char ** params = NULL);

								void ReloadAllShaders();
						// enable/disable programs
								void enableVP(int index);	// bind and enable program
								void enableFP(int index);
								void disableVP();
								void disableFP();

						// set parameters
								void	SetVPParameter1 (int VPIndex, char * pname, float para);
								void	SetVPParameter1 (int VPIndex, char * pname, double para);
								void	SetVPParameter2(int VPIndex, char * pname, float* para);
								void	SetVPParameter2(int VPIndex, char * pname, double* para);
								void	SetVPParameter3(int VPIndex, char * pname, float* para);
								void	SetVPParameter3(int VPIndex, char * pname, double* para);
								void	SetVPParameter4(int VPIndex, char * pname, float* para);
								void	SetVPParameter4(int VPIndex, char * pname, double* para);
								
								void	SetVPMatrixR(int VPIndex, char * pname, double * matrix);
								void	SetVPMatrixC(int VPIndex, char * pname, double * matrix);
								void	SetVPMatrixR(int VPIndex, char * pname, float * matrix);
								void	SetVPMatrixC(int VPIndex, char * pname, float * matrix);
								void	SetVPStateMatrix(int VPIndex, char * pname, CGGLenum statMatrixType, CGGLenum transform);
										//	statMatrixType:
										//		CG_GL_MODELVIEW_MATRIX	for the current model-view matrix
										//		CG_GL_PROJECTION_MATRIX for the current projection matrix
										//		CG_GL_TEXTURE_MATRIX	for the current texture matrix
										//		CG_GL_MODELVIEW_PROJECTION_MATRIX	for the concatenated modelview and projection matrices
										//	tansform:
										//		CG_GL_MATRIX_IDENTITY	for applying no transformation at all
										//		CG_GL_MATRIX_TRANSPOSE	for transposing the matrix
										//		CG_GL_MATRIX_INVERSE	for inverting the matrix
										//		CG_GL_MATRIX_INVERSE_TRANSPOSE for inverting and transposing the matrix

								void	SetFPParameter1 (int FPIndex, char * pname, float para);
								void	SetFPParameter1 (int FPIndex, char * pname, double para);
								void	SetFPParameter2(int FPIndex, char * pname, float* para);
								void	SetFPParameter2(int FPIndex, char * pname, double* para);
								void	SetFPParameter3(int FPIndex, char * pname, float* para);
								void	SetFPParameter3(int FPIndex, char * pname, double* para);
								void	SetFPParameter4(int FPIndex, char * pname, float* para);
								void	SetFPParameter4(int FPIndex, char * pname, double* para);
								
								void	SetFPMatrixR(int FPIndex, char * pname, double * matrix);
								void	SetFPMatrixC(int FPIndex, char * pname, double * matrix);
								void	SetFPMatrixR(int FPIndex, char * pname, float * matrix);
								void	SetFPMatrixC(int FPIndex, char * pname, float * matrix);
								void	SetFPStateMatrix(int VPIndex, char * pname, CGGLenum statMatrixType, CGGLenum transform);
										//	statMatrixType:
										//		CG_GL_MODELVIEW_MATRIX	for the current model-view matrix
										//		CG_GL_PROJECTION_MATRIX for the current projection matrix
										//		CG_GL_TEXTURE_MATRIX	for the current texture matrix
										//		CG_GL_MODELVIEW_PROJECTION_MATRIX	for the concatenated modelview and projection matrices
										//	tansform:
										//		CG_GL_MATRIX_IDENTITY	for applying no transformation at all
										//		CG_GL_MATRIX_TRANSPOSE	for transposing the matrix
										//		CG_GL_MATRIX_INVERSE	for inverting the matrix
										//		CG_GL_MATRIX_INVERSE_TRANSPOSE for inverting and transposing the matrix


							private:
						//		void	LoadVertexProgram(int VPindex);
						//		void	LoadFragmentProgram(int FPindex);

						};
		}	// namespace CGUtility
	}	// namespace Graphics
}	// namespace LLib
#endif
