/*	Octree
 *
 *	The octree consists of many bounding boxes as its tree nodes. Each bounding box has a
 *	maximum number of triangles in it, and this is controlled by the variable, "_bound_num",
 *	which is a user-specifified variable.
 *
 *	Note that the maximum number of triangles can not be too small. The more bounding box, 
 *	the more memory will consume; therefore, the stack may be overflow!
 *
 *																NTHU.IBRLab		demon
 *																				2004/07
 */

#ifndef _OCTREE_H_
#define _OCTREE_H_

#include	"../../algebra3/algebra3.h"
#include	"BoundBox.h"
#include	<vector>
using		std::vector;
using namespace LLib::Math;


namespace LLib
{
	namespace Graphics
	{
		namespace Model3D
		{
			typedef	struct		
			{ int v[3];	} tri_node;		// a triangle contains 3 vertex indices


			class Octree
			{
			public:
				Octree( void );
				~Octree( void );

				/*	bound_num:	maximum # of triangles in a bounding box
					vertex:		coordinates of vertices
					triangle:	3 indices of vertex
				*/
				Octree(int bound_num, vector <vec3> & vertex, vector <tri_node> & triangle);

				/*	building an octree. call this function after initializing data */
				void	build_tree( void );

				void	set_data(int bound_num, vector <vec3> & vertex, vector <tri_node>  & triangle);
				
				/*	return all the bounds of leaf nodes 
					the bound is at the back-right-up corner and front-left-down corner
					therefore, any one pair represents a bound of a bounding box
				*/
				vector <vec3>		get_leaves_bound();	
				vector <vec3>		get_vertex()	{ return _vertex; }
				vector <tri_node>	get_triangle()	{ return _triangle; }	

				/*	given a shooted-ray; this function will set all the intersected triangles
					into t_idx */
				void	get_intersect_triangles(vec3 & pos, vec3 & shoot_ray, vector <int> & t_idx);

				/* print information about octree */
				void	dump_info();


			public:
				vector <vec3>		_vertex;		// vertices coordinates
				vector <tri_node>	_triangle;		// triangles indices of vertex
				int					_bound_num;		// maximum # of triangles in a bounding box
			private:
				BoundBox		*	root;			// octree root
				

				int		_total_bound_box;	// for printing some information; # of total bounding boxes
				int		_total_leaves;
				int		_max_depth;			// maximum depth of the tree

				void	free_octree(BoundBox * p);
				void	find_intersect_triangles(vec3 & pos, vec3 & shoot_ray, vector <int> & t_idx, BoundBox * p);
				void	init_root( void );
				void	sub_divide(BoundBox * p, int current_depth);
				void	find_bound(int position, vec3 & p_blu, vec3 & p_frd, vec3 & blu, vec3 & frd);
				void	find_inside_triangles(vec3 & blu, vec3 & frd, int total_p_t, int *p_t_idx, int & total_t, int *t_idx);

				void	traverse(BoundBox * p, vector <vec3> & container);
				
				/*	given a triangle's 3 vertices and ray position and direction, to determine
					whether the ray is intersected with this triangle by checking the return value.
					The return value means that the distance between ray position and point which
					is intersected with triangle. return negative means no intersection.
				*/
				double	is_intersect_triangle(vec3 & o, vec3 & d, vec3 & v0, vec3 & v1, vec3 & v2);
			};
		}	// namespace Model3D
	}	// namespace Graphics
}	// namespace LLib


#endif