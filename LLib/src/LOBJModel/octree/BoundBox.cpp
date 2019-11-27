#include "../../../header/LOBJModel/octree/BoundBox.h"

namespace LLib
{
	namespace Graphics
	{
		namespace Model3D
		{

			/////////////////////////////////
			BoundBox::BoundBox()
			/////////////////////////////////
			{
				_num_in_box = 0;
				t_idx = NULL;

				for (int i=0; i<8; i++)
					child[i] = NULL;
				_is_internal_node = false;
			}

			/////////////////////////////////
			BoundBox::~BoundBox()
			/////////////////////////////////
			{
				if (t_idx != NULL) free(t_idx);

				for (int i=0; i<8; i++)
					if (child[i] != NULL) delete child[i];
			}

			/////////////////////////////////
			void BoundBox::set_child
					(int position, vec3 & blu, vec3 & frd, int num_in_box, int *t_idx)
			/////////////////////////////////
			{
				BoundBox	*bb = new BoundBox();

				bb->set_inside_triangles(num_in_box, t_idx);
				bb->set_bound(blu, frd);
				this->child[position] = bb;
			}

			/////////////////////////////////
			void BoundBox::set_bound(vec3 & blu, vec3 & frd)
			/////////////////////////////////
			{
				_blu = blu;
				_frd = frd;
			}

			/////////////////////////////////
			void BoundBox::set_inside_triangles(int num_in_box, int *t_idx)
			/////////////////////////////////
			{
				this->t_idx			=	(int *) malloc (sizeof(int) * num_in_box);
				this->_num_in_box	=	num_in_box;

				memcpy(this->t_idx, t_idx, sizeof(int) * num_in_box);
			}

			/////////////////////////////////
			void BoundBox::set_internal()
			/////////////////////////////////
			{
				_is_internal_node = true;
				free (t_idx);
			}

			/////////////////////////////////
			void BoundBox::get_bound(vec3 & blu, vec3 & frd)
			/////////////////////////////////
			{
				blu = _blu;
				frd = _frd;
			}

			/////////////////////////////////
			double BoundBox::is_intersect(vec3 & o, vec3 & d)
			/////////////////////////////////
			{
				int i;
				vec3	a[3];						//	half-length vectors
				vec3	ac = (_frd + _blu) / 2;		//	center of the box
				vec3	an[3];						//	normalized vectors
				vec3	h;							//	positive half-lengths from the center to a box face
				vec3	p;							//	vector from o to ac
				double	tmin = -999999,	tmax = 999999,	eps = 0.00000001;

				
				p		=	ac - o;
				d		=	d.normalize();
				a[VX].set( (_frd[VX] - _blu[VX]) / 2, 0, 0 );
				a[VY].set( 0, (_blu[VY] - _frd[VY]) / 2, 0 );
				a[VZ].set( 0, 0, (_frd[VZ] - _blu[VZ]) / 2 );

				for (i=0; i<3; i++)		an[i]	=	a[i];
				for (i=0; i<3; i++)		an[i]	=	an[i].normalize();
				for (i=0; i<3; i++)		h[i]	=	a[i].length();

				for (i=0; i<3; i++)
				{
					double	e,	f, t1, t2, tmp;

					e	=	an[i]	*	p;
					f	=	an[i]	*	d;
					if ( f > eps || f < -eps )	//	not	perpendicular to normal
					{
						t1	=	(e + h[i]) / f;
						t2	=	(e - h[i]) / f;
						if (t1 > t2) SWAP(t1, t2, tmp);

						tmin	=	(t1 > tmin) ? (t1) : (tmin);
						tmax	=	(t2 < tmax) ? (t2) : (tmax);
						if (tmin > tmax || tmax < 0) return -1;
					}
					else if ( -e > h[i] || e > h[i] ) return -1;		// orthogonal to normal and outside
																					// of the box
				}

				return (tmin > 0) ? (tmin) : (tmax);
			}
		}	// namespace Model3D
	}	// namespace Graphics
}	// namespace LLib
