#include "../../header/algebra3/algebra3.h"
#include "../../header/LOBJModel/LOBJModel.h"
#include <stdio.h>
using namespace LLib::Math;


namespace LLib
{
	namespace Graphics
	{
		namespace Model3D
		{
			bool isIntersection( vec3 pos, vec3 dir, vec3 * ver[3], float	& distance, vec3	& intersectedVertex);

			//////////////////////////////////////////////////////////////////////////
			//				Define  in glm.cpp only
			GLMgroup* findGroup(GLMmodel* model, char* name)
			{
				GLMgroup* group;

				group = model->groups;
				while(group) {
//					printf("\"%s\":%d  ==> \"%s\"\n", group->name, group->numtriangles, name);
					if (!strcmp(name, group->name))
						break;
					group = group->next;
				}
				printf("\n");

				return group;
			}
			//////////////////////////////////////////////////////////////////////////


			LOBJModel::LOBJModel()
			{
				this->objModel	= NULL;
				this->isOctreeBuilded	= false;
				this->refractionCoeffection = 1.25;
				this->numTriTangent = 0;
				this->triTangents  = NULL;
				this->triBinormals = NULL;
				this->triTangentLen= NULL;
				this->triBinormalLen=NULL;

				this->numVertexTangent = 0;
				this->vertexTangents = NULL;
				this->vertexBinormals= NULL;

				this->glListID = -1;
			}
			LOBJModel::~LOBJModel()
			{
				if(this->objModel!=NULL)
					glmDelete(this->objModel);
				this->objModel	= NULL;
				free(this->triTangents);
				free(this->triBinormals);
				free(this->triTangentLen);
				free(this->triBinormalLen);

				free(this->vertexTangents);
				free(this->vertexBinormals);
			}

			void LOBJModel::readObjModel(char *fname)
			{
				// read model
				this->objModel = glmReadOBJ(fname);
				// find boundery

				findBounderyBox();
			}

			void LOBJModel::DrawGroup(GLuint mode, char * gName)
			{
				static GLuint i;
				static GLMgroup* group;
				static GLMtriangle* triangle;
				static GLMmaterial* material;


				/* do a bit of warning */
				if (mode & GLM_FLAT && !objModel->facetnorms) {
					printf("glmDraw() warning: flat render mode requested "
						"with no facet normals defined.\n");
					mode &= ~GLM_FLAT;
				}
				if (mode & GLM_SMOOTH && !objModel->normals) {
					printf("glmDraw() warning: smooth render mode requested "
						"with no normals defined.\n");
					mode &= ~GLM_SMOOTH;
				}
				if (mode & GLM_TEXTURE && !objModel->texcoords) {
					printf("glmDraw() warning: texture render mode requested "
						"with no texture coordinates defined.\n");
					mode &= ~GLM_TEXTURE;
				}
				if (mode & GLM_FLAT && mode & GLM_SMOOTH) {
					printf("glmDraw() warning: flat render mode requested "
						"and smooth render mode requested (using smooth).\n");
					mode &= ~GLM_FLAT;
				}
				if (mode & GLM_COLOR && !objModel->materials) {
					printf("glmDraw() warning: color render mode requested "
						"with no materials defined.\n");
					mode &= ~GLM_COLOR;
				}
				if (mode & GLM_MATERIAL && !objModel->materials) {
					printf("glmDraw() warning: material render mode requested "
						"with no materials defined.\n");
					mode &= ~GLM_MATERIAL;
				}
				if (mode & GLM_COLOR && mode & GLM_MATERIAL) {
					printf("glmDraw() warning: color and material render mode requested "
						"using only material mode.\n");
					mode &= ~GLM_COLOR;
				}
				if (mode & GLM_COLOR)
					glEnable(GL_COLOR_MATERIAL);
				else if (mode & GLM_MATERIAL)
					glDisable(GL_COLOR_MATERIAL);

				/* perhaps this loop should be unrolled into material, color, flat,
				smooth, etc. loops?  since most cpu's have good branch prediction
				schemes (and these branches will always go one way), probably
				wouldn't gain too much?  */


				group = findGroup(this->objModel, gName);
				{
					if (mode & GLM_MATERIAL) {
						material = &objModel->materials[group->material];
						glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
						glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
						glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
					}

					if (mode & GLM_COLOR) {
						glColor3fv(material->diffuse);
					}

					glBegin(GL_TRIANGLES);
					for (i = 0; i < group->numtriangles; i++) {
						triangle = &(objModel->triangles[(group->triangles[i])]);

						if (mode & GLM_FLAT)
							glNormal3fv(&objModel->facetnorms[3 * triangle->findex]);

						if (mode & GLM_SMOOTH)
							glNormal3fv(&objModel->normals[3 * triangle->nindices[0]]);
						if (mode & GLM_TEXTURE)
							glTexCoord2fv(&objModel->texcoords[2 * triangle->tindices[0]]);
						glVertex3fv(&objModel->vertices[3 * triangle->vindices[0]]);

						if (mode & GLM_SMOOTH)
							glNormal3fv(&objModel->normals[3 * triangle->nindices[1]]);
						if (mode & GLM_TEXTURE)
							glTexCoord2fv(&objModel->texcoords[2 * triangle->tindices[1]]);
						glVertex3fv(&objModel->vertices[3 * triangle->vindices[1]]);

						if (mode & GLM_SMOOTH)
							glNormal3fv(&objModel->normals[3 * triangle->nindices[2]]);
						if (mode & GLM_TEXTURE)
							glTexCoord2fv(&objModel->texcoords[2 * triangle->tindices[2]]);
						glVertex3fv(&objModel->vertices[3 * triangle->vindices[2]]);

					}
					glEnd();
				}
			}




			void LOBJModel::DrawModelEdge(float colorR, float colorG, float colorB)
			{
				static GLuint mode = GLM_SMOOTH;
				static GLuint i;
				static GLMgroup* group;
				static GLMtriangle* triangle;

				GLMmodel* model = this->objModel;


				glColor3f(colorR, colorG, colorB);
				group = model->groups;
				while (group)
				{
					glBegin(GL_LINES);
					for (i = 0; i < group->numtriangles; i++) {
						triangle = &(model->triangles[(group->triangles[i])]);
						glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
						glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
						glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
						glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
						glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
						glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
					}
					glEnd();

					group = group->next;
				}
			}

			void LOBJModel::DrawWithNormalColor( GLuint mode )
			{
				static GLuint i;
				static GLMgroup* group;
				static GLMtriangle* triangle;
				static GLMmaterial* material;

				GLMmodel* model = this->objModel;

				group = model->groups;
				while(group)
				{
					glBegin(GL_TRIANGLES);
					for (i = 0; i < group->numtriangles; i++) 
					{
						triangle = &this->objModel->triangles[group->triangles[i]];

						if (mode & GLM_FLAT)
						{
							glNormal3fv(&model->facetnorms[3 * triangle->findex]);
							glColor3f( model->facetnorms[3 * triangle->findex+0]* 0.5 +0.5,
								model->facetnorms[3 * triangle->findex+1]* 0.5 +0.5,
								model->facetnorms[3 * triangle->findex+2]* 0.5 +0.5);
						}

						if (mode & GLM_SMOOTH)
						{
							glNormal3fv(&model->normals[3 * triangle->nindices[0]]);
							glColor3f( model->normals[3 * triangle->nindices[0] + 0] *0.5 + 0.5,
								model->normals[3 * triangle->nindices[0] + 1] *0.5 + 0.5,
								model->normals[3 * triangle->nindices[0] + 2] *0.5 + 0.5);
						}

						if (mode & GLM_TEXTURE)
							glTexCoord2fv(&model->texcoords[2 * triangle->tindices[0]]);
						glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);

						if (mode & GLM_SMOOTH)
						{
							glNormal3fv(&model->normals[3 * triangle->nindices[1]]);
							glColor3f( model->normals[3 * triangle->nindices[1] + 0] *0.5 + 0.5,
								model->normals[3 * triangle->nindices[1] + 1] *0.5 + 0.5,
								model->normals[3 * triangle->nindices[1] + 2] *0.5 + 0.5);
						}
						if (mode & GLM_TEXTURE)
							glTexCoord2fv(&model->texcoords[2 * triangle->tindices[1]]);
						glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);

						if (mode & GLM_SMOOTH)
						{
							glNormal3fv(&model->normals[3 * triangle->nindices[2]]);
							glColor3f(	model->normals[3 * triangle->nindices[2] + 0] *0.5 + 0.5,
								model->normals[3 * triangle->nindices[2] + 1] *0.5 + 0.5,
								model->normals[3 * triangle->nindices[2] + 2] *0.5 + 0.5);
						}
						if (mode & GLM_TEXTURE)
							glTexCoord2fv(&model->texcoords[2 * triangle->tindices[2]]);
						glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);

					}
					glEnd();

					group = group->next;
				}
			}

			void LOBJModel::DrawVextexTangent(float scalar)
			{
				static GLuint i;
				static GLMgroup* group;
				static GLMtriangle* triangle;

				GLMmodel* model = this->objModel;

				float pt[3][3];

				glColor3f(1,1,1);
				group = model->groups;
				while (group)
				{
					//glBegin(GL_TRIANGLES);
					glBegin(GL_LINES);
					for (i = 0; i < group->numtriangles; i++) {
						triangle = &(model->triangles[(group->triangles[i])]);
						for (int j=0; j<3; j++)
							for (int k=0; k<3; k++)
							{
								pt[j][k] = model->vertices[3 * triangle->vindices[j]+k] + this->vertexTangents[3*triangle->vindices[j]+k]*scalar;
							}

							glColor3f(	model->normals[3 * triangle->nindices[0] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[0] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[0] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
							glVertex3fv(pt[0]);


							glColor3f(	model->normals[3 * triangle->nindices[1] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[1] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[1] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
							glVertex3fv(pt[1]);

							glColor3f(	model->normals[3 * triangle->nindices[2] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[2] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[2] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
							glVertex3fv(pt[2]);

					}
					glEnd();

					group = group->next;
				}
			}

			void LOBJModel::DrawVextexBinormal(float scalar)
			{
				static GLuint i;
				static GLMgroup* group;
				static GLMtriangle* triangle;

				GLMmodel* model = this->objModel;

				float pt[3][3];

				glColor3f(1,1,1);
				group = model->groups;
				while (group)
				{
					//glBegin(GL_TRIANGLES);
					glBegin(GL_LINES);
					for (i = 0; i < group->numtriangles; i++) {
						triangle = &(model->triangles[(group->triangles[i])]);
						for (int j=0; j<3; j++)
							for (int k=0; k<3; k++)
							{
								pt[j][k] = model->vertices[3 * triangle->vindices[j]+k] + this->vertexBinormals[3*triangle->vindices[j]+k]*scalar;
							}

							glColor3f(	model->normals[3 * triangle->nindices[0] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[0] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[0] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
							glVertex3fv(pt[0]);


							glColor3f(	model->normals[3 * triangle->nindices[1] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[1] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[1] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
							glVertex3fv(pt[1]);

							glColor3f(	model->normals[3 * triangle->nindices[2] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[2] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[2] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
							glVertex3fv(pt[2]);

					}
					glEnd();

					group = group->next;
				}
			}

			void LOBJModel::DrawVextexNormal(float scalar)
			{
				static GLuint i;
				static GLMgroup* group;
				static GLMtriangle* triangle;

				GLMmodel* model = this->objModel;

				float pt[3][3];

				glColor3f(1,1,1);
				group = model->groups;
				while (group)
				{
					//glBegin(GL_TRIANGLES);
					glBegin(GL_LINES);
					for (i = 0; i < group->numtriangles; i++) {
						triangle = &(model->triangles[(group->triangles[i])]);
						for (int j=0; j<3; j++)
							for (int k=0; k<3; k++)
							{
								pt[j][k] = model->vertices[3 * triangle->vindices[j]+k] + model->normals[3*triangle->nindices[j]+k] * scalar;
							}

							glColor3f(	model->normals[3 * triangle->nindices[0] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[0] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[0] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
							glVertex3fv(pt[0]);


							glColor3f(	model->normals[3 * triangle->nindices[1] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[1] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[1] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
							glVertex3fv(pt[1]);

							glColor3f(	model->normals[3 * triangle->nindices[2] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[2] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[2] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
							glVertex3fv(pt[2]);

					}
					glEnd();

					group = group->next;
				}
			}

			void LOBJModel::DrawFaceTangent()
			{
				static GLuint mode = GLM_SMOOTH;
				static GLuint i;
				static GLMgroup* group;
				static GLMtriangle* triangle;

				GLMmodel* model = this->objModel;

				float pt[3][3];

				glColor3f(1,1,1);
				group = model->groups;
				while (group)
				{
					//glBegin(GL_TRIANGLES);
					glBegin(GL_LINES);
					for (i = 0; i < group->numtriangles; i++) {
						triangle = &(model->triangles[(group->triangles[i])]);
						for (int j=0; j<3; j++)
							for (int k=0; k<3; k++)
							{
								pt[j][k] = model->vertices[3 * triangle->vindices[j]+k] + this->triTangents[3*(group->triangles[i])+k]*10;
							}

							//glColor3f(	model->normals[3 * triangle->nindices[0] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[0] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[0] + 2] *0.5 + 0.5);


							glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
							glVertex3fv(pt[0]);


							//glColor3f(	model->normals[3 * triangle->nindices[1] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[1] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[1] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
							glVertex3fv(pt[1]);

							//glColor3f(	model->normals[3 * triangle->nindices[2] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[2] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[2] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
							glVertex3fv(pt[2]);

					}
					glEnd();

					group = group->next;
				}

			}
			void LOBJModel::DrawFaceNormal()
			{
				static GLuint i;
				static GLMgroup* group;
				static GLMtriangle* triangle;

				GLMmodel* model = this->objModel;

				float pt[3][3];

				glColor3f(1,1,1);
				group = model->groups;
				while (group)
				{
					//glBegin(GL_TRIANGLES);
					glBegin(GL_LINES);
					for (i = 0; i < group->numtriangles; i++) {
						triangle = &(model->triangles[(group->triangles[i])]);
						for (int j=0; j<3; j++)
							for (int k=0; k<3; k++)
							{
								pt[j][k] = model->vertices[3 * triangle->vindices[j]+k] + model->facetnorms[3*triangle->findex+k];
							}

							//glColor3f(	model->normals[3 * triangle->nindices[0] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[0] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[0] + 2] *0.5 + 0.5);


							glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
							glVertex3fv(pt[0]);


							//glColor3f(	model->normals[3 * triangle->nindices[1] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[1] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[1] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
							glVertex3fv(pt[1]);

							//glColor3f(	model->normals[3 * triangle->nindices[2] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[2] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[2] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
							glVertex3fv(pt[2]);

					}
					glEnd();

					group = group->next;
				}

			}


			void LOBJModel::DrawFaceBinormal()
			{
				static GLuint mode = GLM_SMOOTH;
				static GLuint i;
				static GLMgroup* group;
				static GLMtriangle* triangle;

				GLMmodel* model = this->objModel;

				float pt[3][3];

				glColor3f(1,1,1);
				group = model->groups;
				while (group)
				{
					//glBegin(GL_TRIANGLES);
					glBegin(GL_LINES);
					for (i = 0; i < group->numtriangles; i++) {
						triangle = &(model->triangles[(group->triangles[i])]);
						for (int j=0; j<3; j++)
							for (int k=0; k<3; k++)
							{
								pt[j][k] = model->vertices[3 * triangle->vindices[j]+k] + this->triBinormals[3*(group->triangles[i])+k]*10;
							}

							//glColor3f(	model->normals[3 * triangle->nindices[0] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[0] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[0] + 2] *0.5 + 0.5);


							glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
							glVertex3fv(pt[0]);


							//glColor3f(	model->normals[3 * triangle->nindices[1] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[1] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[1] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
							glVertex3fv(pt[1]);

							//glColor3f(	model->normals[3 * triangle->nindices[2] + 0] *0.5 + 0.5, model->normals[3 * triangle->nindices[2] + 1] *0.5 + 0.5, model->normals[3 * triangle->nindices[2] + 2] *0.5 + 0.5);
							glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
							glVertex3fv(pt[2]);

					}
					glEnd();

					group = group->next;
				}

			}



			//************************************
			// Method:    ComputFaceTangentLength
			// FullName:  LLib::Graphics::Model3D::LOBJModel::ComputFaceTangentLength
			// Access:    public 
			// Returns:   void
			// Qualifier: compute the length of triangle tangent on texture space, and store it in triTangentLen[], triBinormalLen[]
			//************************************
			void LOBJModel::drawEdgeUsingTexCoord()
			{
				static GLuint i;
				static GLMgroup* group;
				static GLMtriangle* triangle;
				static GLMmaterial* material;

				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();
				glOrtho(0,1,0,1,-1,1);
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();

				{
					group = this->objModel->groups;
					while (group) {

						glColor3f(1,1,1);

						glBegin(GL_LINES);
						for (i = 0; i < group->numtriangles; i++) {
							triangle = &(this->objModel->triangles[(group->triangles[i])]);

							glVertex3f(this->objModel->texcoords[2 * triangle->tindices[0]],this->objModel->texcoords[2 * triangle->tindices[0]+1], 1);
							glVertex3f(this->objModel->texcoords[2 * triangle->tindices[1]],this->objModel->texcoords[2 * triangle->tindices[1]+1], 1);

							glVertex3f(this->objModel->texcoords[2 * triangle->tindices[1]],this->objModel->texcoords[2 * triangle->tindices[1]+1], 1);
							glVertex3f(this->objModel->texcoords[2 * triangle->tindices[2]],this->objModel->texcoords[2 * triangle->tindices[2]+1], 1);

							glVertex3f(this->objModel->texcoords[2 * triangle->tindices[2]],this->objModel->texcoords[2 * triangle->tindices[2]+1], 1);
							glVertex3f(this->objModel->texcoords[2 * triangle->tindices[0]],this->objModel->texcoords[2 * triangle->tindices[0]+1], 1);
						}
						glEnd();

						group = group->next;
					}
				}

				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);

				return ;
			}

			//************************************
			// Method:    ComputFaceTangentLength
			// FullName:  LLib::Graphics::Model3D::LOBJModel::ComputFaceTangentLength
			// Access:    public 
			// Returns:   void
			// Qualifier: compute the length of triangle tangent on texture space, and store it in triTangentLen[], triBinormalLen[]
			//************************************
			void LOBJModel::ComputeFaceTangentLength()
			{
				if (this->objModel->texcoords == NULL)
				{
					printf("LOBJModel::Can't compute the texture orientation, NO texture!\n");
					return;
					
				}
				else if (this->triTangents == NULL || this->triBinormals == NULL)
				{
					printf("LOBJModel::Can't compute the tangent length without triangle tangents.\n");
					return ;
				}
				else
				{
					printf("LOBJModel::Compute the texture orientation length for OBJ\n");
				}

				this->numTriTangent = this->objModel->numtriangles;
				if(this->triTangentLen  != NULL) free(this->triTangentLen);
				if(this->triBinormalLen != NULL) free(this->triBinormalLen);
				this->triTangentLen  = (float*) malloc(sizeof(float) * this->numTriTangent);
				this->triBinormalLen = (float*) malloc(sizeof(float) * this->numTriTangent);


				////////////////////////////////////////////////////////////////////
				//	Step.1	Calculate length of S and T for each triangle
				float maximum[2] = {0,0}, minimum[2] = {1,1};
				for (int i=0;i< (int) this->objModel->numtriangles;i++)
				{
					vec3 vS,vT;
					GLMtriangle* tri = &this->objModel->triangles[i];

					int * tIdx = (int*) &tri->tindices[0];
					int * vIdx = (int*) &tri->vindices[0];
					vec3 vTex[3];		//Texture Coordinate
					vec3 vP[3];			//Position of 3 vertices
					vec3 edge[3];

					//initialize values :: set  Positions and texCoords of vertices of triangle into vTex[] and vP[].
					for(int j=0; j<3; j++)
					{
						vP[j]	= vec3(	this->objModel->vertices[vIdx[j]*3+0],
										this->objModel->vertices[vIdx[j]*3+1],
										this->objModel->vertices[vIdx[j]*3+2]);
						vTex[j] = vec3(	this->objModel->texcoords[tIdx[j]*2+0],
										this->objModel->texcoords[tIdx[j]*2+1],
										0);
					}
					edge[0] = vP[0] - vP[1];
					edge[1] = vP[1] - vP[2];
					edge[2] = vP[2] - vP[0];
					float oTriLen[2] = {0,0}; // triangle range in object space lay on tangent and binormal direction
					vec3 tangent  = vec3(this->triTangents[i*3], this->triTangents[i*3+1], this->triTangents[i*3+2]);
					vec3 binormal = vec3(this->triBinormals[i*3], this->triBinormals[i*3+1], this->triBinormals[i*3+2]);
					for (int j=0; j<3; j++)
					{
						oTriLen[0] = MAX(oTriLen[0], abs(edge[j]*tangent));
						oTriLen[1] = MAX(oTriLen[0], abs(edge[j]*binormal));
					}

					this->triTangentLen[i]  = (MAX( MAX(vTex[0][0], vTex[1][0]), vTex[2][0]) - MIN( MIN(vTex[0][0], vTex[1][0]), vTex[2][0]))/ oTriLen[0];
					this->triBinormalLen[i] = (MAX( MAX(vTex[0][1], vTex[1][1]), vTex[2][1]) - MIN( MIN(vTex[0][1], vTex[1][1]), vTex[2][1]))/ oTriLen[1];

					if (triTangentLen[i]> maximum[0])	{	maximum[0] = triTangentLen[i];	}
					else if (triTangentLen[i]< minimum[0])	{	minimum[0] = triTangentLen[i];	}
					if (triBinormalLen[i]> maximum[1])	{	maximum[1] = triBinormalLen[i];	}
					else if (triBinormalLen[i]< minimum[1])	{	minimum[1] = triBinormalLen[i];	}
				}
				printf("Tangent Len:\t[%f, %f]\nBinormal Len:\t[%f,%f]\nRatio:\t\t[%f, %f]\n", maximum[0], minimum[0], maximum[1], minimum[1], maximum[0]/maximum[1], minimum[0]/minimum[1]);
			}


			////////////////////////////////////////////////////////////////////////
			//	Name:	ComputeTextureOrientation
			//	Func:	If texture coordinate available, compute texture orientation
			//	Date:	2005/09/15
			////////////////////////////////////////////////////////////////////////
			void LOBJModel::ComputeTexOrientation()
			{
				int i, j;
				////////////////////////////////////////////////////////////////////
				//	Step.0	Check
				if (this->objModel->facetnorms== NULL)
				{
					printf("LOBJModel::Can't compute the texture orientation, NO Face Normal!\n");
				}
				if (this->objModel->texcoords)
					printf("LOBJModel::Compute the texture orientation for OBJ\n");
				else
				{
					printf("LOBJModel::Can't compute the texture orientation, NO texture!\n");
					return;
				}

				this->numTriTangent = this->objModel->numtriangles;
				if(this->triTangents != NULL) free(this->triTangents);
				if(this->triBinormals != NULL) free(this->triBinormals);
				this->triTangents = (float*) malloc(sizeof(float) * this->numTriTangent*3);
				this->triBinormals = (float*) malloc(sizeof(float) * this->numTriTangent*3);


				////////////////////////////////////////////////////////////////////
				//	Step.1	Calculate S and T for each triangle
				for (i=0;i< (int) this->objModel->numtriangles;i++)
				{
					vec3 vS,vT;
					GLMtriangle* tri = &this->objModel->triangles[i];

					int * tIdx = (int*) &tri->tindices[0];
					int * vIdx = (int*) &tri->vindices[0];
					vec3 vTex[3];		//Texture Coordinate
					vec3 vP[3];			//Position of 3 vertices


					// set  Positions and texCoords of vertices of triangle into vTex[] and ;vP[]
					for(j=0; j<3; j++)
					{
						vP[j]= vec3(	this->objModel->vertices[vIdx[j]*3+0],
										this->objModel->vertices[vIdx[j]*3+1],
										this->objModel->vertices[vIdx[j]*3+2]);
						vTex[j] = vec3(	this->objModel->texcoords[tIdx[j]*2+0],
										this->objModel->texcoords[tIdx[j]*2+1],
										0);
					}

					const float Delta = 0.00001;
					if (fabs(vTex[0][LOBJ_LY]-vTex[1][LOBJ_LY])<Delta)		//P1-P0 paraller to S coordination
					{
						if (vTex[0][LOBJ_LX]>vTex[1][LOBJ_LX])
							vS=vP[0]-vP[1];
						else
							vS=vP[1]-vP[0];
						vS = vS.normalize();					
					}
					else
					{
						vec3 vU;	//vU.x+vU.y+vU.z==0 and let vU.z==1
						vU[LOBJ_LX]=(vTex[2][LOBJ_LY]-vTex[1][LOBJ_LY])/(vTex[1][LOBJ_LY]-vTex[0][LOBJ_LY]);
						vU[LOBJ_LY]=(vTex[0][LOBJ_LY]-vTex[2][LOBJ_LY])/(vTex[1][LOBJ_LY]-vTex[0][LOBJ_LY]);
						vU[LOBJ_LZ]=1;

						//Check the S coordination direction 
						{
							vec3 vTestS = vTex[0]*vU[LOBJ_LX] + vTex[1] * vU[LOBJ_LY ]+vTex[2]*vU[LOBJ_LZ];
							//		ASSERT(fabs(vTestS.y)<Delta);
							if (vTestS[LOBJ_LX]<0)
								vU=vU*(-1);	//Change the direction	

						}

						vS=vP[0]*vU[LOBJ_LX]+vP[1]*vU[LOBJ_LY]+vP[2]*vU[LOBJ_LZ];
						vS.normalize();
					}

					vec3 vN = vec3(	this->objModel->facetnorms[tri->findex*3+0],
									this->objModel->facetnorms[tri->findex*3+1],
									this->objModel->facetnorms[tri->findex*3+2]);
					vT= vN ^ vS;
					vT = vT.normalize();

					//Store the value to triangles
					for(j=0; j<3; j++)
					{
						this->triTangents[i*3+j] = vS[j];
						this->triBinormals[i*3+j] = vT[j];
					}
				}
			/*
				////////////////////////////////////////////////////////////////////
				//	Step.2	Transfer S,T for each Vertex
				for (i=0;i<m_nVertices;i++)
				{
					CVertex* pVrt=m_pVertices+i;

					vec3f& vS = pVrt->m_vS;
					vec3f& vT = pVrt->m_vT;

					vT = vS = vec3f(0,0,0);			// Clear old value		


					for (int j=0;j<pVrt->m_nTriangle ;j++)
					{
						int ntIdx = pVrt->m_pTriangleIndices [j];
						vS = vS+m_pTriangles[ntIdx].m_vS;
						vT = vT+m_pTriangles[ntIdx].m_vT;
					}

					vS.unify ();
					vT.unify ();
				}
			//*/
			}

			////////////////////////////////////////////////////////////////////////
			//	Name:	ComputeVertexTangent
			//	Func:	If texture coordinate available, compute texture orientation and compute Vertex Tangent
			//	Date:	2006/07/12
			////////////////////////////////////////////////////////////////////////
			void LOBJModel::ComputeVertexTangent()
			{
				int *vertexTangentStackNum = NULL;
				if ( this->triTangents == NULL || this->triBinormals==NULL || this->numTriTangent==0)
				{
					printf("triangle Tangent don\'t Exist. pleas call ComputeFaceTangent() first!\n");
					return ;
				}
				else if(this->objModel->normals==NULL || this->objModel->numnormals==0)
				{
					printf("vertex Normal don\'t exist. please call glmVertexNormals() first!\n");
					return ;
				}
				printf("normal: %d, vertex: %d\ntriTangent: %d, face: %d\n", this->objModel->numnormals, this->objModel->numvertices, this->numTriTangent, this->objModel->numtriangles);


				// bind on vertex, but Normal is indicate by index of triangles information

				this->numVertexTangent = this->objModel->numvertices;
				this->vertexTangents = (GLfloat*) malloc(sizeof(GLfloat)* 3* (this->numVertexTangent+1));
				this->vertexBinormals= (GLfloat*) malloc(sizeof(GLfloat)* 3* (this->numVertexTangent+1));
				vertexTangentStackNum= (int*)	  malloc(sizeof(int)*(this->numVertexTangent+1));

				memset(this->vertexTangents,  0, (this->numVertexTangent+1)*3*sizeof(GLfloat));
				memset(this->vertexBinormals, 0, (this->numVertexTangent+1)*3*sizeof(GLfloat));
				memset(vertexTangentStackNum, 0, (this->numVertexTangent+1)*1*sizeof(int));

				for (int i=0; i<this->objModel->numtriangles; i++)	// for each triangle
				{
					for (int j=0; j<3; j++)							// for each vertex on triangle
					{
						int vIndex = this->objModel->triangles[i].vindices[j];
						vertexTangentStackNum[vIndex] +=1;
						for (int k=0; k<3; k++)						// for X,Y,Z
						{
							this->vertexTangents [3*vIndex+k] += this->triTangents[3*i+k];
							this->vertexBinormals[3*vIndex+k] += this->triBinormals[3*i+k];
						}
					}
					
				}
				for (int i=1; i<=this->numVertexTangent; i++)
				{
					for (int k=0; k<3; k++)
					{
						this->vertexTangents[3*i+k] /= vertexTangentStackNum[i];
						this->vertexBinormals[3*i+k]/= vertexTangentStackNum[i];
					}
				}
				return ;

			}
			

			//******************************************************************************************//
			//		bounding box
			//******************************************************************************************//
			int LOBJModel::getBounderyVertices( float bounderyVertex[8][4])
			{
				for(int i=0; i<8; i++)
					bounderyVertex[i][3]	= 1;

				bounderyVertex[0][0]	= this->boundery[LOBJ_LX][LOBJ_MIN];
				bounderyVertex[0][1]		= this->boundery[LOBJ_LY][LOBJ_MIN];
				bounderyVertex[0][2]	= this->boundery[LOBJ_LZ][LOBJ_MIN];

				bounderyVertex[1][0]		= this->boundery[LOBJ_LX][LOBJ_MIN];
				bounderyVertex[1][1]		= this->boundery[LOBJ_LY][LOBJ_MIN];
				bounderyVertex[1][2]		= this->boundery[LOBJ_LZ][LOBJ_MAX];

				bounderyVertex[2][0]	= this->boundery[LOBJ_LX][LOBJ_MIN];
				bounderyVertex[2][1]		= this->boundery[LOBJ_LY][LOBJ_MAX];
				bounderyVertex[2][2]	= this->boundery[LOBJ_LZ][LOBJ_MIN];

				bounderyVertex[3][0]	= this->boundery[LOBJ_LX][LOBJ_MIN];
				bounderyVertex[3][1]		= this->boundery[LOBJ_LY][LOBJ_MAX];
				bounderyVertex[3][2]	= this->boundery[LOBJ_LZ][LOBJ_MAX];

				bounderyVertex[4][0]	= this->boundery[LOBJ_LX][LOBJ_MAX];
				bounderyVertex[4][1]		= this->boundery[LOBJ_LY][LOBJ_MIN];
				bounderyVertex[4][2]	= this->boundery[LOBJ_LZ][LOBJ_MIN];

				bounderyVertex[5][0]	= this->boundery[LOBJ_LX][LOBJ_MAX];
				bounderyVertex[5][1]		= this->boundery[LOBJ_LY][LOBJ_MIN];
				bounderyVertex[5][2]	= this->boundery[LOBJ_LZ][LOBJ_MAX];

				bounderyVertex[6][0]	= this->boundery[LOBJ_LX][LOBJ_MAX];
				bounderyVertex[6][1]		= this->boundery[LOBJ_LY][LOBJ_MAX];
				bounderyVertex[6][2]	= this->boundery[LOBJ_LZ][LOBJ_MIN];

				bounderyVertex[7][0]	= this->boundery[LOBJ_LX][LOBJ_MAX];
				bounderyVertex[7][1]		= this->boundery[LOBJ_LY][LOBJ_MAX];
				bounderyVertex[7][2]	= this->boundery[LOBJ_LZ][LOBJ_MAX];

				return 1;
			}

			int LOBJModel::getBounderyZValue(float* openGLModelView, float * Zminmax)
			{
				float zcomponet[4];
				float bounderyVertex[8][4];
				float tmp;

				zcomponet[0] = openGLModelView[2];
				zcomponet[1] = openGLModelView[6];
				zcomponet[2] = openGLModelView[10];
				zcomponet[3] = openGLModelView[14];

				getBounderyVertices( bounderyVertex);

				tmp=0;
				for(int j=0;  j<4; j++)
					tmp +=  zcomponet[j] * bounderyVertex[0][j];
				Zminmax[LOBJ_MIN] =Zminmax[LOBJ_MAX] = tmp;
				for(int i=1; i<8; i++)
				{
					tmp=0;
					for(int j=0;  j<4; j++)
						tmp +=  zcomponet[j] * bounderyVertex[i][j];
					// min store max, max store min, because the min and max is in homographics coordinate.
					Zminmax[LOBJ_MIN] = (Zminmax[LOBJ_MIN]>tmp)? Zminmax[LOBJ_MIN]: tmp;
					Zminmax[LOBJ_MAX] = (Zminmax[LOBJ_MAX]<tmp)? Zminmax[LOBJ_MAX]: tmp;
				}
				return 1;

			}

			void LOBJModel::findBounderyBox()
			{
				for(int j=0; j<3; j++)
				{
					boundery[j][LOBJ_MAX] = boundery[j][LOBJ_MIN] =  this->objModel->vertices[j];
				}
				for(int i=0; i<(int)this->objModel->numvertices; i++)
					for(int j=0; j<3; j++)
					{
						boundery[j][LOBJ_MIN] =  ( boundery[j][LOBJ_MIN] <  this->objModel->vertices[i*3+j])? (boundery[j][LOBJ_MIN]): (this->objModel->vertices[i*3+j]);
						boundery[j][LOBJ_MAX] =  ( boundery[j][LOBJ_MAX] >  this->objModel->vertices[i*3+j])? (boundery[j][LOBJ_MAX]): (this->objModel->vertices[i*3+j]);
					}
				return;
			}
			void LOBJModel::showBounderyBox()
			{
				static int order[6][4] =	// CCW order
					{
						{0,1,3,2},
						{5,4,6,7},
						{1,5,7,3},
						{0,2,6,4},
						{2,3,7,6},
						{0,4,5,1}
					};
				static float bounderV[8][4];
				this->getBounderyVertices(bounderV);

				glBegin(GL_QUADS);
					for(int i=0; i<6; i++)
						for(int j=0; j<4; j++)
						{
							glVertex3fv(bounderV[order[i][j]]);
						}
				glEnd();

			}





			//******************************************************************************************//
			// Octree
			//******************************************************************************************//
			void LOBJModel::buildOctree(int bound_num)
			{
				int i, j;
				fprintf(stdout,"\rBuilding Octree.");
				//	 doing this function ===> octree.set_data(int , vertex, triangle);
				octree._vertex.resize(this->objModel->numvertices);
				for( i=1; i<=(int)this->objModel->numvertices; i++)
					for( j=0; j<3; j++)
						octree._vertex[i-1][j] = objModel->vertices[i*3+j];
				octree._triangle.resize(this->objModel->numtriangles);
				for( i=0; i<(int)this->objModel->numtriangles; i++)
					for( j=0; j<3; j++)
						octree._triangle[i].v[j] = objModel->triangles[i].vindices[j]-1;
				octree._bound_num = bound_num;

				// build octree
				octree.build_tree();
				this->isOctreeBuilded = true;
				fprintf(stdout,"\rBuilded Octree.     \n");
			}

			bool LOBJModel::intersection(vec3 &rayPos, vec3 &rayDir, vec3& returnVec3, vec3& returnNoraml, vec3& returnTexCoord)
			{
				int i,j;
				vector <int> t_idx;
				int listSize;
				float	distance = -1, temp;
				vec3*	pTriVer[3];
				vec3*	pTriVer_tmp[3];
				vec3		triVer[3];
				vec3		triNormal[3];
				vec3		triTexCoord[3];
				vec3	intersectedVertex3;
				int intersectedTriIndex;

				if (!this->isOctreeBuilded)
				{
					printf("Do not Build Octree!!! building now!\r");
					this->buildOctree(100);
					printf("Do not Build Octree!!! build.                               \n");
				}


				octree.get_intersect_triangles(rayPos, rayDir, t_idx);
				listSize = t_idx.size();

				for ( i=0; i<listSize; i++)
				{
					for( j=0; j<3; j++)
						pTriVer_tmp[j] = &octree._vertex[octree._triangle[t_idx[i]].v[j]];

					if (isIntersection(rayPos, rayDir, pTriVer_tmp, temp, intersectedVertex3))
					{
						if( temp>0.001 &&( (temp < distance)|| distance == -1) )
						{
							returnVec3 = intersectedVertex3;
							distance = temp;
							intersectedTriIndex = t_idx[i];
							for( j=0; j<3; j++)
								pTriVer[j] = pTriVer_tmp[j];
						}
					}
				}
			#ifdef LDEBUG_SHOW_INTERSECTED_POINTS_INFO
				printf("\t\t\tdistance: %f\n", distance);
			#endif
				if (distance == -1)
				{
					return false;
				}
				else
				{
					// get normal and texCoord
					GLMtriangle tri =objModel->triangles[intersectedTriIndex];
					//		printf("\n");
					for(int i=0l; i<3; i++)
					{
						for( j=0; j<3; j++)
						{
							triVer[i][j]			=this->objModel->vertices[tri.vindices[i]*3+j];
						}
						for( j=0; j<3; j++)
							triNormal[i][j]		= this->objModel->normals[tri.nindices[i]*3+j];		
						for( j=0; j<2; j++)
							triTexCoord[i][j]	= this->objModel->texcoords[tri.tindices[i]*2+j];
					}
					//  interpolated the returnNormal, returnTexCoord and

					int sIndex[3][2] = {
						{1,2},
						{2,0},
						{0,1}
					};
					vec3 v[3];	float s[4];	float w[3];

					for(j=0; j<3; j++)
						v[j] = triVer[j] - returnVec3;
					s[3] =0;
					for(j=0; j<3; j++)
					{
						s[3] += (s[j] = (v[sIndex[j][0]] ^ v[sIndex[j][1]]).length());
					}
					for(j=0; j<3; j++)
						w[j] = s[j] / s[3];

					returnNoraml	=	(w[0]*triNormal[0] + w[1]*triNormal[1] +w[2]*triNormal[2]).normalize();
					returnTexCoord	=	w[0]*triTexCoord[0] + w[1]*triTexCoord[1] +w[2]*triTexCoord[2];

					//			triVer[i].print(stdout, "triVer[?]");
					//			pTriVer[i]->print(stdout, "ptriVer[!]");
				}
				//		rayPos.print(stdout, "rayPos");
				//		rayDir.print(stdout, "rayDir");
				//		(rayPos + distance* rayDir).print(stdout, "intersected");
				//		returnVec3.print(stdout, "p2");

				//			getchar();

				return true;
			}

			bool isIntersection(vec3 pos, vec3 dir,			// line
								vec3	* ver[3],					// triangle 3 points
								float	& distance,					// output:  t value
								vec3	& intersectedVertex	// output: intersected point
								)
			{
			#define EPS 0.000001
				vec3 e1, e2, p, q, s;
				float a, f, t, u, v;
				e1 = *ver[1] - *ver[0];
				e2 = *ver[2] - *ver[0];
				p  = dir ^ e2;
				a = e1* p;
				if ( a>-EPS && a< EPS)
					return false;
				f = 1/a;
				s = pos - *ver[0];
				u = f*( s * p);
				if (u<0.0||u>1.0)
					return false;
				q = s^e1;
				v = f*( dir*q);
				if (v<0.0||v+u>1.0)
					return false;
				t= f*(e2*q);
				intersectedVertex = pos + dir * t;
				distance = t;	// dir is the unit vector

				return true;
			};

			void LOBJModel::showOctreeBoundingBox()
			{
				vector <vec3>	leaves = octree.get_leaves_bound();
				glColor3f(0, 1, 0);
				glBegin(GL_LINES);
				for (int i=0; i<(int)leaves.size() / 2; i++)
				{
					vec3	blu, frd;		

					blu		=	leaves[2 * i];
					frd		=	leaves[2 * i + 1];
					double	dx = frd[VX] - blu[VX];
					double	dy = blu[VY] - frd[VY];
					double	dz = frd[VZ] - blu[VZ];

					glVertex3f(blu[0], blu[1], blu[2]);
					glVertex3f(blu[0] + dx, blu[1], blu[2]);

					glVertex3f(blu[0], blu[1], blu[2]);
					glVertex3f(blu[0], blu[1] - dy, blu[2]);

					glVertex3f(blu[0], blu[1], blu[2]);
					glVertex3f(blu[0], blu[1], blu[2] + dz);

					glVertex3f(blu[0] + dx, blu[1], blu[2]);
					glVertex3f(frd[0], frd[1] + dy, frd[2]);

					glVertex3f(blu[0] + dx, blu[1], blu[2]);
					glVertex3f(frd[0], frd[1], frd[2] - dz);

					glVertex3f(blu[0], blu[1] - dy, blu[2]);
					glVertex3f(frd[0], frd[1], frd[2] - dz);

					glVertex3f(blu[0], blu[1] - dy, blu[2]);
					glVertex3f(frd[0] - dx, frd[1], frd[2]);

					glVertex3f(blu[0], blu[1], blu[2] + dz);
					glVertex3f(frd[0], frd[1] + dy, frd[2]);

					glVertex3f(blu[0], blu[1], blu[2] + dz);
					glVertex3f(frd[0] - dx, frd[1], frd[2]);

					glVertex3f(frd[0], frd[1], frd[2]);
					glVertex3f(frd[0] - dx, frd[1], frd[2]);

					glVertex3f(frd[0], frd[1], frd[2]);
					glVertex3f(frd[0], frd[1] + dy, frd[2]);

					glVertex3f(frd[0], frd[1], frd[2]);
					glVertex3f(frd[0], frd[1], frd[2] - dz);
				}
				glEnd();
			};

			void LOBJModel::showInfo()
			{
				printf("vertex  : %7d\n", this->objModel->numvertices);
				printf("normal  : %7d\n", this->objModel->numnormals);
				printf("triangle: %7d\n", this->objModel->numtriangles);
				printf("face Nor: %7d\n", this->objModel->numfacetnorms);
				printf("groups:   %7d\n", this->objModel->numgroups);
				printf("material: %7d\n", this->objModel->nummaterials);
			}
		}	// namespace Model3D
	}	// namespace Graphics
}	// namespace LLib


