/************************************************************************
OpenGL Viewer using "FBO" <glloader.h>
-------------------------------------------------------------------------

Version 1.2 - August 10 2004

-------------------------------------------------------------------------
Author: Chen Ying-Chieh

*************************************************************************/

#ifndef _LOBJMODEL_H_
#define _LOBJMODEL_H_

#include "glm/glm.h"
#include "octree/Octree.h"
using namespace LLib::Graphics::Model3D;
using namespace LLib::Graphics::Model3D::GLM;


namespace LLib
{
	namespace Graphics
	{
		namespace Model3D
		{
		
				// deinfes
				#ifndef NULL
				#define NULL 0
				#endif

				#ifndef MIN 
				#define MIN(A,B) (((A)<(B))?(A):(B))
				#endif
				#ifndef MAX
				#define MAX(A,B) (((A)>(B))?(A):(B))
				#endif
				#ifndef SWAP
				#define SWAP( A, B, T) ( (T) = (A), (A) = (B), (B) =(T))
				#endif

				#ifndef LOBJ_LX
				#define LOBJ_LX 0
				#endif
				#ifndef LOBJ_LY
				#define LOBJ_LY 1
				#endif
				#ifndef LOBJ_LZ
				#define LOBJ_LZ 2
				#endif

				#ifndef LOBJ_MIN
				#define LOBJ_MIN 0
				#endif
				#ifndef LOBJ_MAX
				#define LOBJ_MAX 1
				#endif

				/* =====================================================*/

				class LOBJModel
				{
					// variables
				public:
					// obj Model
					GLMmodel* objModel;
					int		numTriTangent;
					float*	triTangents;
					float*	triBinormals;
					float*	triTangentLen;	// = len(tri.texCoord[S]) / len(tri.in.tangent.direction.in.object.space)
					float*	triBinormalLen; // = len(tri.texCoord[T]) / len(tri.in.binormal.direction.in.object.space)

					// bind on vertex, but Normal is indicate by index of triangles information
					int			numVertexTangent;
					GLfloat*	vertexTangents;
					GLfloat*	vertexBinormals;

				protected:
					// glList
					GLuint glListID;

					// bounding box
					double boundery[3][2];
					// octree
					bool		isOctreeBuilded;
					LLib::Graphics::Model3D::Octree octree;


					// others
					float		refractionCoeffection;


					// functions
				public:
					LOBJModel();
					~LOBJModel();

					// obj model operation.
					void showInfo();
					void readObjModel(char * fname);
					void writeObjModel(char* filename, GLuint mode)					{ glmWriteOBJ(this->objModel, filename, mode);};
					void Unitize()													{ glmUnitize(this->objModel);findBounderyBox();};
					void Scale( GLfloat scale)										{ glmScale(this->objModel, scale);findBounderyBox();};
					void FacetNormals()												{ glmFacetNormals(this->objModel);};
					void VertexNormals(GLfloat angle)								{ glmVertexNormals(this->objModel,angle);};
					void ComputeTexOrientation();					// computer Tangent and Binormal from texCoord, the same as function named "ComputeFaceTangent"
					void ComputeFaceTangent()						// computer Tangent and Binormal from texCoord, the same as function named "ComputeTexOrientation"
																					{ this->ComputeTexOrientation(); };
					void ComputeVertexTangent();
					void ComputeFaceTangentLength();

					int isNoramlExist()												{ return ((objModel->numnormals !=0)? 1:0);};
					int isTexCoordExist()											{ return ((objModel->numtexcoords != 0)? 1:0);};

					void LinearTexture()											{ glmLinearTexture(this->objModel);};
					void SpheremapTexture()											{ glmSpheremapTexture(this->objModel);};

					void DrawGroup(GLuint mode, char * gName);
					void Draw(GLuint mode)											{ glmDraw(this->objModel, mode);};
					void DrawModelEdge(float colorR = 1, float colorG = 1, float colorB = 1);
					void DrawWithNormalColor(GLuint mode);
					void DrawVextexTangent(float scalar=10);
					void DrawVextexBinormal(float scalar=10);
					void DrawVextexNormal(float scalar=10);
					void drawEdgeUsingTexCoord();

					void DrawFaceTangent();
					void DrawFaceBinormal();
					void DrawFaceNormal();
					
					GLuint MakeList(GLuint mode)									{ return (glListID = glmList(this->objModel, mode));};
					void CallList()													{ if(glListID != -1) (glCallList( glListID));};
					void DeleteList()												{ if(glListID != -1) (glDeleteLists( glListID, 1));};

					// octree
					void buildOctree(int);
					bool intersection(vec3 &rayPos, vec3 &rayDir, vec3& returnVec3, vec3& returnNoraml, vec3& returnTexCoord);
					void showOctreeBoundingBox();

					// bounding Box
					int getBounderyZValue(float* modelView, float * Zminmax);
					int getBounderyVertices(float bounder[6][4]);
					void showBounderyBox();

					//	float[3][2] getBoundery()									{ return this->boundery;};

					// others
					float getRefractionCoeffection()								{ return this->refractionCoeffection; };
					void setRefractionCoeffection(float n)							{ this->refractionCoeffection = n;};

				private:
					void findBounderyBox();
				};
		}	// namespace Model3D
	}	// namespace Graphics
}	// namespace LLib


#endif