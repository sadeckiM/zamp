#ifndef VECTOR3D_HH
#define VECTOR3D_HH

#include "geomVector.hh"
#include <sstream>
#include <string>

/*!
 * \file
 * \brief Deklaracja instacji szablonu geom::Vector
 */

 /*!
  * \brief Instacja szablonu geom::Vector<typename,int> dla przestrzeni 3D.
  */
 typedef geom::Vector<double,3>  Vector3D;
 typedef geom::Vector<int, 3> Vector3D_int;

#endif
