#include "../../../header/LOBJModel/octree/Octree.h"

namespace LLib
{
	namespace Graphics
	{
		namespace Model3D
		{

			/////////////////////////////////
			Octree::Octree()
			/////////////////////////////////
			{
				root	=	NULL;
				_total_bound_box	=	0;
				_total_leaves		=	0;
				_max_depth			=	0;
			}

			/////////////////////////////////
			Octree::Octree(int bound_num, vector <vec3> & vertex, vector <tri_node> & triangle)
			/////////////////////////////////
			{
				Octree();
				set_data(bound_num, vertex, triangle);
			}

			/////////////////////////////////
			Octree::~Octree()
			/////////////////////////////////
			{
				if (root != NULL)
					free_octree(root);

				_vertex.~vector();
				_triangle.~vector();
			}

			/////////////////////////////////
			void Octree::dump_info()
			/////////////////////////////////
			{
				printf("Number of bounding boxes		=	%d\n", _total_bound_box);
				printf("Total leaves				=	%d\n", _total_leaves);
				printf("The maximum depth			=	%d\n", _max_depth);
			}

			/////////////////////////////////
			void Octree::build_tree()
			/////////////////////////////////
			{
				init_root();
				sub_divide(root, 0);
			}

			/////////////////////////////////
			void Octree::set_data(int bound_num, vector <vec3> & vertex, vector <tri_node> & triangle)
			/////////////////////////////////
			{
				_bound_num	=	bound_num;
				_vertex		=	vertex;
				_triangle	=	triangle;
			}

			/////////////////////////////////
			void Octree::free_octree(BoundBox * p)
			/////////////////////////////////
			{
				if (p == NULL) return;
				if (p->is_internal() == false)	// leaf node
				{
					delete p;
					return;
				}

				for (int i=0; i<8; i++)
					free_octree(p->get_child(i));
			}

			/////////////////////////////////
			vector <vec3> Octree::get_leaves_bound()
			/////////////////////////////////
			{
				vector <vec3>	container;

				container.clear();
				traverse(root, container);

				return container;
			}

			/////////////////////////////////
			void Octree::get_intersect_triangles(vec3 & pos, vec3 & shoot_ray, vector <int> & t_idx)
			/////////////////////////////////
			{
				BoundBox	*p	=	root;

				t_idx.clear();
				find_intersect_triangles(pos, shoot_ray, t_idx, p);
			}

			/////////////////////////////////
			void Octree::find_intersect_triangles(vec3 & pos, vec3 & shoot_ray, vector <int> & t_idx, BoundBox *p)
			/////////////////////////////////
			{
				if (p == NULL) return;	// prevent root is NULL
				// return if there is no intersection at the bounding box
				if ( p->is_intersect(pos, shoot_ray) < 0 ) return;

				if ( !p->is_internal() )	// leaf node
				{
					char	*flag	=	(char *) malloc (sizeof(char) * _triangle.size());	// used for checking
																							// duplicated triangles
					memset(flag, 0, sizeof(char) * _triangle.size());

					int	* in_box = p->get_triangles();
					for (int i=0; i<p->get_num_in_box(); i++)
					{
						if (flag[in_box[i]]) continue;

						t_idx.push_back(in_box[i]);
						flag[in_box[i]] = 1;
					}
					free(flag);
					return;
				}

				// internal node, search continually
				for (int i=0; i<8; i++)
					find_intersect_triangles(pos, shoot_ray, t_idx, p->get_child(i));
			}

			/////////////////////////////////
			void Octree::init_root()
			/////////////////////////////////
			{
				int	*	t_idx;
				int i, j;
				double	xmin, xmax, ymin, ymax, zmin, zmax, eps = 0.000001;


				if (_triangle.size() == 0) return;	// no need to initial root
				root = new BoundBox();
				t_idx = (int *) malloc (sizeof(int) * _triangle.size());

				// init t_idx
				for ( i=0; i< (int) _triangle.size(); i++) t_idx[i] = i;

				// find bound
				int	index	=	_triangle[0].v[0];
				xmin = xmax =	_vertex[index][VX];
				ymin = ymax =	_vertex[index][VY];
				zmin = zmax =	_vertex[index][VZ];
				for (i=0; i< (int) _triangle.size(); i++)
					for (j=0; j<3; j++)
					{
						index	=	_triangle[i].v[j];
						xmin = (xmin > _vertex[index][VX]) ? (_vertex[index][VX]) : (xmin);
						xmax = (xmax < _vertex[index][VX]) ? (_vertex[index][VX]) : (xmax);
						ymin = (ymin > _vertex[index][VY]) ? (_vertex[index][VY]) : (ymin);
						ymax = (ymax < _vertex[index][VY]) ? (_vertex[index][VY]) : (ymax);
						zmin = (zmin > _vertex[index][VZ]) ? (_vertex[index][VZ]) : (zmin);
						zmax = (zmax < _vertex[index][VZ]) ? (_vertex[index][VZ]) : (zmax);
					}
				xmax = (xmax - xmin <= eps) ? (xmax + 1) : (xmax);
				ymax = (ymax - ymin <= eps) ? (ymax + 1) : (ymax);
				zmax = (zmax - zmin <= eps) ? (zmax + 1) : (zmax);
				root->set_inside_triangles(_triangle.size(), t_idx);
				root->set_bound(vec3(xmin, ymax, zmin), vec3(xmax, ymin, zmax));

				free(t_idx);
				_total_bound_box++;
			}

			/////////////////////////////////
			void Octree::sub_divide(BoundBox *p, int current_depth)
			/////////////////////////////////
			{
				int		num_in_box;
				int	*	t_idx;
				int		num_tri;
				int	*	new_t_idx;
				vec3	p_blu, p_frd;

				// prevent root is NULL (no data is given)
				if (p == NULL) return;

				// update _max_depth info.
				_max_depth  =	(_max_depth < current_depth) ? (current_depth) : (_max_depth);
				num_in_box	=	p->get_num_in_box();
				t_idx		=	p->get_triangles();

				if (num_in_box <= _bound_num) 	// no need to further divide
				{
					_total_leaves++;	// this is a leaf node
					return;
				}
				
				p->get_bound(p_blu, p_frd);	// get the current box bound
				for (int i=0; i<8; i++)
				{
					vec3	blu, frd;

					num_tri = 0;
					find_bound(i, p_blu, p_frd, blu, frd);

					new_t_idx	=	(int *) malloc(sizeof(int) * num_in_box);
					find_inside_triangles(blu, frd, num_in_box, t_idx, num_tri, new_t_idx);		

					// there is no need to allocate memory for zero number of triangle box			
					if (num_tri != 0)
					{
						p->set_child(i, blu, frd, num_tri, new_t_idx);
						free(new_t_idx);	// free immediately

						_total_bound_box++;
						// there is no need to proceed if the child's content is the same as parent's
						if (num_tri != num_in_box)
							sub_divide(p->get_child(i), current_depth + 1);
						else
							_total_leaves++;
					}
					else
						free(new_t_idx);	// free immediately
				}
				
				p->set_internal();
			}

			/////////////////////////////////
			void Octree::find_bound(int position, vec3 & p_blu, vec3 & p_frd, vec3 & blu, vec3 & frd)
			/////////////////////////////////
			{
				vec3	vx( (p_frd[VX] - p_blu[VX]) / 2, 0, 0 );
				vec3	vy( 0, (p_blu[VY] - p_frd[VY]) / 2, 0 );
				vec3	vz( 0, 0, (p_frd[VZ] - p_blu[VZ]) / 2 );

				switch (position)
				{
				case F_LUP:
					blu		=	p_blu + vz;
					frd		=	p_frd + vy - vx;
					break;
				case F_LDOWN:
					blu		=	p_blu + vz - vy;
					frd		=	p_frd - vx;
					break;
				case F_RUP:
					blu		=	p_blu + vx + vz;
					frd		=	p_frd + vy;
					break;
				case F_RDOWN:
					blu		=	p_blu + vx + vz - vy;
					frd		=	p_frd;
					break;
				case B_LUP:
					blu		=	p_blu;
					frd		=	p_frd - vx + vy - vz;
					break;
				case B_LDOWN:
					blu		=	p_blu - vy;
					frd		=	p_frd - vx - vz;
					break;
				case B_RUP:
					blu		=	p_blu + vx;
					frd		=	p_frd - vz + vy;
					break;
				case B_RDOWN:
					blu		=	p_blu + vx - vy;
					frd		=	p_frd - vz; 
				}
			}

			/////////////////////////////////
			void Octree::find_inside_triangles(vec3 & blu, vec3 & frd, int total_p_t, int *p_t_idx,
											int & total_t, int *t_idx)
			/////////////////////////////////
			{
				int i,j;
				BoundBox	bb;		// for checking whether the triangle is inside

				bb.set_bound( blu, frd );

				for (i=0; i<total_p_t; i++)	// for each triangle
				{
					bool	is_inside = false;
					// test whether any of the vertices are inside the box
					for (j=0; j<3 && !is_inside; j++)
					{
						int	index = _triangle[p_t_idx[i]].v[j];

						if (_vertex[index][VX] < blu[VX] || _vertex[index][VX] > frd[VX]) continue;
						if (_vertex[index][VY] > blu[VY] || _vertex[index][VY] < frd[VY]) continue;
						if (_vertex[index][VZ] < blu[VZ] || _vertex[index][VZ] > frd[VZ]) continue;
						is_inside = true;
					}
					
					// test whether any of the edge are inside the box
					for (j=0; j<3 && !is_inside; j++)
					{
						vec3	pos	=	_vertex[ _triangle[ p_t_idx[i] ].v[j] ];
						vec3	dir =	_vertex[ _triangle[ p_t_idx[i] ].v[ (j+1) % 3 ] ] - pos;
						double	len	=	dir.length();
						double	t	=	bb.is_intersect( pos, dir );

						if (t > 0 && len > t) is_inside = true;
					}

					// test whether the interior of the triangle intersects the box
					if (!is_inside)
					{
						double	t;
						int		v0	=	_triangle[ p_t_idx[i] ].v[0];
						int		v1	=	_triangle[ p_t_idx[i] ].v[1];
						int		v2	=	_triangle[ p_t_idx[i] ].v[2];

						// check first diagonal
						t	=	is_intersect_triangle(blu, frd - blu, _vertex[v0], _vertex[v1], _vertex[v2]);
						if ( t > 0 && t < (frd - blu).length() )	is_inside = true;
						// check second diagonal
						vec3	dia1 ( frd[VX], blu[VY], frd[VZ] );
						vec3	dia2 ( blu[VX], frd[VY], blu[VZ] );
						t	=	is_intersect_triangle(dia1, dia2 - dia1, _vertex[v0], _vertex[v1], _vertex[v2]);
						if ( t > 0 && t < (dia2 - dia1).length() )	is_inside = true;
					}

					if (is_inside)
						t_idx[total_t++] = p_t_idx[i];
				}
			}

			/////////////////////////////////
			void Octree::traverse(BoundBox *p, vector <vec3> & container)
			/////////////////////////////////
			{
				if (p == NULL) return;
				if (!p->is_internal())
				{
					vec3	blu, frd;

					p->get_bound(blu, frd);
					container.push_back(blu);
					container.push_back(frd);
					return;
				}

				for (int i=0; i<8; i++)
					traverse(p->get_child(i), container);
			}

			/////////////////////////////////
			double Octree::is_intersect_triangle
					(vec3 & o, vec3 & d, vec3 & v0, vec3 & v1, vec3 & v2)
			/////////////////////////////////
			{
				d.normalize();
				double	eps	=	0.0000001;
				vec3	e1	=	v1	-	v0;
				vec3	e2	=	v2	-	v0;
				vec3	p	=	d	^	e2;
				double	a	=	e1	*	p;

				if ( a > -eps && a < eps ) return -1;
				double	f	=	1 / a;

				vec3	s	=	o	-	v0;
				double	u	=	f	*	(s	*	p);
				if ( u < 0.0 || u > 1.0 ) return -1;

				vec3	q	=	s	^	e1;
				double	v	=	f	*	(d	*	q);
				if ( v < 0.0 || u + v > 1.0) return -1;

				return	f	*	(e2	*	q);
			}
		}	// namespace Model3D
	}	// namespace Graphics
}	// namespace LLib