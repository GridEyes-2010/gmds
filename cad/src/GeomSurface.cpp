/*----------------------------------------------------------------------------*/
/** \file    GeomSurface.cpp
 *  \author  legoff
 *  \date    03/07/2013
 */
/*----------------------------------------------------------------------------*/
// GMDS File Headers
/*----------------------------------------------------------------------------*/
#include <gmds/cad/GeomSurface.h>
/*----------------------------------------------------------------------------*/
namespace gmds{
/*----------------------------------------------------------------------------*/
namespace cad{
/*----------------------------------------------------------------------------*/
void GeomSurface::project(math::Point& AP) const
{
	AP = closestPoint(AP);
}
/*----------------------------------------------------------------------------*/
} // namespace cad
/*----------------------------------------------------------------------------*/
} // namespace gmds
/*----------------------------------------------------------------------------*/