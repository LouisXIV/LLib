#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../header/LCGObject/LCGObject.h"

namespace LLib
{
	namespace Graphics
	{
		namespace CGUtility
		{
				static CGcontext currentCGContext = NULL;

				static void handleCgError()
				{
					CGerror error = cgGetError();
					fprintf(stderr, "Cg error: %s\n", cgGetErrorString(error));
					if (error == CG_COMPILER_ERROR) 
					{
						fprintf(stderr, "Cg error: %s\n",cgGetLastListing(currentCGContext));
					}
					exit(1);
				}


				LCGObject::LCGObject(int VPNum, int FPNum, CGprofile fpProfile, CGprofile vpProfile)
				{
					int isVPdone = 0;
					int isFPdone = 0;

					this->VPNum = VPNum;
					this->FPNum = FPNum;
					this->myCgVertexProgram		= new CGprogram[VPNum];
					this->myCgFragmentProgram	= new CGprogram[FPNum];

					this->vpNames						= (char**)malloc(VPNum * sizeof(char**));
					for(int i=0; i<VPNum; i++)
						this->vpNames[i] = (char*)malloc(256 * sizeof(char*));
					this->vpEntryFunc						= (char**)malloc(VPNum * sizeof(char**));
					for(int i=0; i<VPNum; i++)
						this->vpEntryFunc[i] = (char*)malloc(32 * sizeof(char*));
					this->vpParams						= (char***)malloc(VPNum * sizeof(char***));
					for(int i=0; i<VPNum; i++)
					{
						this->vpParams[i] = NULL;
					}


					this->fpNames						= (char**)malloc(FPNum * sizeof(char**));
					for(int i=0; i<FPNum; i++)
						this->fpNames[i] = (char*)malloc(256 * sizeof(char*));
					this->fpEntryFunc					= (char**)malloc(FPNum * sizeof(char**));
					for(int i=0; i<FPNum; i++)
						this->fpEntryFunc[i] = (char*)malloc(32 * sizeof(char*));
					this->fpParams						= (char***)malloc(FPNum * sizeof(char***));
					for(int i=0; i<FPNum; i++)
					{
						this->fpParams[i] = NULL;
					}

					if(vpProfile!=NULL)
					{
						isVPdone = cgGLIsProfileSupported(vpProfile);
						fprintf(stderr, "VP: user chosen profiles is supported on this system.\n");
					}

					if (vpProfile==NULL && cgGLIsProfileSupported(CG_PROFILE_VP20))
					{
						vertexProfile = CG_PROFILE_VP20;
						fprintf(stderr, "VP20 vertex profiles is supported on this system.\n");
					}
					else if (vpProfile==NULL && cgGLIsProfileSupported(CG_PROFILE_ARBVP1))
					{
						vertexProfile = CG_PROFILE_ARBVP1;
						fprintf(stderr, "ARBVP1 vertex profiles is supported on this system.\n");
					}
					else if(vpProfile==NULL)
					{
						fprintf(stderr, "Neither ARBVP1 nor VP20 vertex profiles supported on this system.\n");
						exit(1);
					}
				//	vertexProfile = CG_PROFILE_VS_1_1;
					// fragment(pixel) shader

					if(fpProfile!=NULL)
					{
						isFPdone = cgGLIsProfileSupported(fpProfile);
						fprintf(stderr, "FP: user chosen profiles is supported on this system.\n");
					}


					if (fpProfile==NULL && cgGLIsProfileSupported(CG_PROFILE_FP40))
					{
						fragmentProfile = CG_PROFILE_FP40;
						fprintf(stdout, "FP40 fragment profiles is supported on this system.\n");
					}
					else if (fpProfile==NULL && cgGLIsProfileSupported(CG_PROFILE_FP30))
					{
						fragmentProfile = CG_PROFILE_FP30;
						fprintf(stdout, "FP30 fragment profiles is supported on this system.\n");
					}
					else if (fpProfile==NULL && cgGLIsProfileSupported(CG_PROFILE_ARBFP1))
					{
						fragmentProfile = CG_PROFILE_ARBFP1;
						fprintf(stdout, "ARBFP1 fragment profiles is used on this system.\n");
					}
					else if (fpProfile==NULL)
					{
						fprintf(stderr, "Neither FP30, FP40 nor ARBFP1 fragment profiles supported on this system.\n");
						exit(1);
					}

					cgSetErrorCallback(handleCgError);
//*	
					this->myCgContext = cgCreateContext();
//*/
				};


				LCGObject::~LCGObject()
				{
					int i;
					for(i=0; i<VPNum; i++)
					{
						free(this->vpNames[i]);
						free(this->vpEntryFunc[i]);
						if(this->vpParams[i]!=NULL)
						{
							char ** params = this->vpParams[i];
							int j=0;
							while (params[j])
							{
								if(params[j])
								{
									free(params[j]);
								}
							}
							free(params);
						}
					}
					free(this->vpNames);
					free(this->vpEntryFunc);
					free(this->vpParams);

					for(i=0; i<FPNum; i++)
					{
						free(this->fpNames[i]);
						free(this->fpEntryFunc[i]);
						if(this->fpParams[i]!=NULL)
						{
							char ** params = this->fpParams[i];
							int j=0;
							while (params[j])
							{
								if(params[j])
								{
									free(params[j]);
								}
							}
							free(params);
						}
					}
					free(this->fpNames);
					free(this->fpEntryFunc);
					free(this->fpParams);

					delete myCgVertexProgram;
					delete myCgFragmentProgram;
				}


				void copyParams(char *** target, const char ** sources)
				{
					if (sources == NULL)
					{
						(*target) = NULL;
					}
					else
					{
						int count=0;
						while (sources[count])
						{
							count++;
						}
						*target = (char**) malloc(sizeof(char*)*(count+1)); 
						int i = 0;
						while (sources[i])
						{
							(*target)[i] = (char *) malloc( 128 * sizeof(char));
							strcpy((*target)[i], sources[i]);
							i++;
						}
						(*target)[i] = NULL;
					}
				}




				void LCGObject::SetVertexProgram( int index, char* fname, char* enterFunction /*= "main"*/, const char ** params /*= NULL*/ )
				{
					LLib::Graphics::CGUtility::currentCGContext = this->myCgContext;
					strcpy( this->vpNames[index], fname);
					strcpy( this->vpEntryFunc[index], enterFunction);
					copyParams(&(this->vpParams[index]), params);
					
					myCgVertexProgram[index] = cgCreateProgramFromFile( myCgContext, CG_SOURCE, fname,  vertexProfile, enterFunction, params);
					cgGLLoadProgram(myCgVertexProgram[index]);
				}


				void LCGObject::SetFragmentProgram( int index, char* fname, char* enterFunction /*= "main"*/, const char ** params /*= NULL*/ )
				{
					LLib::Graphics::CGUtility::currentCGContext = this->myCgContext;
					strcpy( this->fpNames[index], fname);
					strcpy( this->fpEntryFunc[index], enterFunction);
					copyParams(&(this->fpParams[index]), params);

					myCgFragmentProgram[index] = cgCreateProgramFromFile( myCgContext, CG_SOURCE, fname,  fragmentProfile, enterFunction, params);
					cgGLLoadProgram(myCgFragmentProgram[index]);
				}


				void LCGObject::ReloadAllShaders()
				{
					int i;
					LLib::Graphics::CGUtility::currentCGContext = this->myCgContext;
					cgGLDisableProfile(vertexProfile);
					cgGLDisableProfile(fragmentProfile);
					for( i=0; i<VPNum;i++)
					{
						printf("reload vp file: %s with entryFuction: %s %s\n", vpNames[i], vpEntryFunc[i], vpParams[i]);
						cgDestroyProgram(myCgVertexProgram[i]);
						myCgVertexProgram[i] = cgCreateProgramFromFile( myCgContext, CG_SOURCE, vpNames[i],  vertexProfile, vpEntryFunc[i], (const char **) vpParams[i]);
						cgGLLoadProgram(myCgVertexProgram[i]);
					}
					for( i=0; i<FPNum;i++)
					{
						printf("reload fp file: %s with entryFuction: %s %s\n", fpNames[i], fpEntryFunc[i], fpParams[i]);
						cgDestroyProgram(myCgFragmentProgram[i]);
						myCgFragmentProgram[i] = cgCreateProgramFromFile( myCgContext, CG_SOURCE, fpNames[i],  fragmentProfile, fpEntryFunc[i], (const char **) fpParams[i]);
						cgGLLoadProgram(myCgFragmentProgram[i]);
					}
				}

				void LCGObject::enableVP(int index)
				{
					cgGLBindProgram(myCgVertexProgram[index]);
					cgGLEnableProfile(vertexProfile);
					return ;
				}
				void LCGObject::disableVP()
				{
					cgGLDisableProfile(vertexProfile);
					return ;
				}

				void LCGObject::enableFP(int index)
				{
					cgGLBindProgram(myCgFragmentProgram[index]);
					cgGLEnableProfile(fragmentProfile);
					return ;
				}
				void LCGObject::disableFP()
				{
					cgGLDisableProfile(fragmentProfile);
					return ;
				}







				void LCGObject::SetVPParameter1(int VPIndex, char * pname, float para)
				{
					cgGLSetParameter1f(cgGetNamedParameter(myCgVertexProgram[VPIndex], pname), para);
				}
				void LCGObject::SetVPParameter1(int VPIndex, char * pname, double para)
				{
					cgGLSetParameter1d(cgGetNamedParameter(myCgVertexProgram[VPIndex], pname), para);
				}
				void LCGObject::SetVPParameter2(int VPIndex, char * pname, float *para)
				{
					cgGLSetParameter2fv(cgGetNamedParameter(myCgVertexProgram[VPIndex], pname), para);
				}
				void LCGObject::SetVPParameter2(int VPIndex, char * pname, double *para)
				{
					cgGLSetParameter2dv(cgGetNamedParameter(myCgVertexProgram[VPIndex], pname), para);
				}
				void LCGObject::SetVPParameter3(int VPIndex, char * pname, float *para)
				{
					cgGLSetParameter3fv(cgGetNamedParameter(myCgVertexProgram[VPIndex], pname), para);
				}
				void LCGObject::SetVPParameter3(int VPIndex, char * pname, double *para)
				{
					cgGLSetParameter3dv(cgGetNamedParameter(myCgVertexProgram[VPIndex], pname), para);
				}
				void LCGObject::SetVPParameter4(int VPIndex, char * pname, float *para)
				{
					cgGLSetParameter4fv(cgGetNamedParameter(myCgVertexProgram[VPIndex], pname), para);
				}
				void LCGObject::SetVPParameter4(int VPIndex, char * pname, double *para)
				{
					cgGLSetParameter4dv(cgGetNamedParameter(myCgVertexProgram[VPIndex], pname), para);
				}
				void LCGObject::SetVPMatrixC(int VPIndex, char * pname, double * matrix)
				{
					cgGLSetMatrixParameterdc(	cgGetNamedParameter(myCgVertexProgram[VPIndex], pname),
						matrix);
				}
				void LCGObject::SetVPMatrixR(int VPIndex, char * pname, double * matrix)
				{
					cgGLSetMatrixParameterdr(	cgGetNamedParameter(myCgVertexProgram[VPIndex], pname),
						matrix);
				}
				void LCGObject::SetVPMatrixC(int VPIndex, char * pname, float * matrix)
				{
					cgGLSetMatrixParameterfc(	cgGetNamedParameter(myCgVertexProgram[VPIndex], pname),
						matrix);
				}
				void LCGObject::SetVPMatrixR(int VPIndex, char * pname, float * matrix)
				{
					cgGLSetMatrixParameterfr(	cgGetNamedParameter(myCgVertexProgram[VPIndex], pname),
						matrix);
				}

				void LCGObject::SetVPStateMatrix(int VPIndex, char * pname, CGGLenum statMatrixType, CGGLenum transform)
				{
					cgGLSetStateMatrixParameter(	cgGetNamedParameter(myCgVertexProgram[VPIndex], pname),
						statMatrixType,
						transform);
				}
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
				void LCGObject::SetFPParameter1(int FPIndex, char * pname, float para)
				{
					cgGLSetParameter1f(cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname), para);
				}
				void LCGObject::SetFPParameter1(int FPIndex, char * pname, double para)
				{
					cgGLSetParameter1d(cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname), para);
				}
				void LCGObject::SetFPParameter2(int FPIndex, char * pname, float *para)
				{
					cgGLSetParameter2fv(cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname), para);
				}
				void LCGObject::SetFPParameter2(int FPIndex, char * pname, double *para)
				{
					cgGLSetParameter2dv(cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname), para);
				}
				void LCGObject::SetFPParameter3(int FPIndex, char * pname, float *para)
				{
					cgGLSetParameter3fv(cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname), para);
				}
				void LCGObject::SetFPParameter3(int FPIndex, char * pname, double *para)
				{
					cgGLSetParameter3dv(cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname), para);
				}
				void LCGObject::SetFPParameter4(int FPIndex, char * pname, float *para)
				{
					cgGLSetParameter4fv(cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname), para);
				}
				void LCGObject::SetFPParameter4(int FPIndex, char * pname, double *para)
				{
					cgGLSetParameter4dv(cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname), para);
				}
				void LCGObject::SetFPMatrixC(int FPIndex, char * pname, double * matrix)
				{
					cgGLSetMatrixParameterdc(	cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname),
						matrix);
				}
				void LCGObject::SetFPMatrixR(int FPIndex, char * pname, double * matrix)
				{
					cgGLSetMatrixParameterdr(	cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname),
						matrix);
				}
				void LCGObject::SetFPMatrixC(int FPIndex, char * pname, float * matrix)
				{
					cgGLSetMatrixParameterfc(	cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname),
						matrix);
				}
				void LCGObject::SetFPMatrixR(int FPIndex, char * pname, float * matrix)
				{
					cgGLSetMatrixParameterfr(	cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname),
						matrix);
				}

				void LCGObject::SetFPStateMatrix(int FPIndex, char * pname, CGGLenum statMatrixType, CGGLenum transform)
				{
					cgGLSetStateMatrixParameter(	cgGetNamedParameter(myCgFragmentProgram[FPIndex], pname),
						statMatrixType,
						transform);
				}
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
		}	// namespace CGUtility
	}	// namespace Graphics
}	// namespace LLib
