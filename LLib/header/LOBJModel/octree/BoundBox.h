#ifndef _BOUND_BOX_H_
#define _BOUND_BOX_H_

#include	"../../algebra3/algebra3.h"
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
using namespace LLib::Math;

/* position of the sub-dividing space position
   F indicates those cubes exist at the front side, and B indicates those at the back side.
   L means left side, while R means right side
   UP means up side, while DOWN means down side
*/
namespace LLib
{
	namespace Graphics
	{
		namespace Model3D
		{
			enum POSITION { F_LUP, F_RUP, F_LDOWN, F_RDOWN, B_LUP, B_RUP, B_LDOWN, B_RDOWN };
			#define	SWAP(a, b, c)		((c) = (a), (a) = (b), (b) = (c))


			class BoundBox
			{
			public:
				BoundBox( void );
				~BoundBox( void );

				void	set_inside_triangles(int num_in_box, int *t_idx);
				void	set_child(int position, vec3 & blu, vec3 & frd, int num_in_box, int *t_idx);	
				void	set_bound(vec3 & blu, vec3 & frd);

				/*	when this function is called, the t_idx will be free, since the
					triangle data is stored at the leaf node, not internal node */
				void	set_internal( void );

				int		get_num_in_box( void )	{ return _num_in_box; }
				int*	get_triangles( void )	{ return t_idx; }
				void	get_bound(vec3 & blu, vec3 & frd);
				BoundBox * get_child(int position)	{ return child[position]; }

				bool	is_internal( void )		{ return _is_internal_node; }

				/*	given a shooted-ray, determine whether this ray intersects
					with the bounding box by considering return minimum distance
					return negative means no intersection, otherwise return positive number
				*/
				double	is_intersect( vec3 & o, vec3 & d );


			private:
				int			_num_in_box;	// number of triangles exists in current bounding box
				int			*t_idx;			// contains a list of indices of these triangles
				BoundBox	*child[8];		// 8 children pointers of sub-bounding boxes
				bool		_is_internal_node;	// if true, this means the bounding box is located
												// at the internal node, and t_idx is null (for saving memory)

				vec3		_blu, _frd;		// bounding coordinates of box. (Back left-up and Fore right-down)
			};
		}	// namespace Model3D
	}	// namespace Graphics
}	// namespace LLib
#endif