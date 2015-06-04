/**
 * @file  coordinates.h
 *
 *  Created on: 2014年12月30日
 *      Author: salmon
 */

#ifndef CORE_MESH_STRUCTURED_COORDINATES_COORDINATES_H_
#define CORE_MESH_STRUCTURED_COORDINATES_COORDINATES_H_

namespace simpla
{

/**
 * @ingroup diff_geo
 * @addtogroup  geometry Geometry
 * @brief  This module define coordinates and metric on the @ref configuration_space (\f$ \mathbb{R}^{4} \f$)
 *
 * ## Summary
 *   \note Contemporary differential geometry is intrinsic, meaning that the spaces it considers are smooth
 *   manifolds whose geometric structure is governed by a Riemannian metric, which determines how distances are
 *   measured near each point, and not a priori parts of some ambient flat Euclidean space.
 *    -- http://en.wikipedia.org/wiki/Geometry#Contemporary_geometry
 *
 *   Geometry describe temporal-spatial coordinates and metric of @ref manifold.
 *
 * ## Requirements
 *
 * The following table lists requirements for a Geometry type G,
 *
 *  Pseudo-Signature  		| Semantics
 *  -------------------|-------------
 *  `G::G( const G& ) 		`| Copy constructor.
 *  `G::~G() 				`| Destructor.
 *  `G::update()			`| update internal stat of coordinates system
 *  `G::coordinate_tuple	`| Data type of coordinates, i.e. nTuple<3,Real>
 *  `G::topology_type		`| Base topology type
 *  `tuple<coordinate_tuple,TV> G::pull_back(tuple<coordinate_tuple,TV> const &) `| pull back a function/value/tensor on _this_ coordinates to Cartesian coordinates system
 *  `tuple<coordinate_tuple,TV> G::push_forward(tuple<coordinate_tuple,TV> const &) `| push forward vector Cartesian coordinates to _this_ coordinates system
 *  `coordinate_tuple  G::mapto(coordinates const &) `| map _Cartesian_ coordinates to _this_  coordinates system
 *  `coordinate_tuple  G::inv_mapto(coordinates const &) `| map _this_ coordinates to _Cartesian_  coordinates system
 *  `Domain domain()const	`| get domain
 *  `Real G::time()const	`| get time
 *  `void G::time(Real)		`| set time
 *  `Real G::dt()const		`| get time step
 *  `void G::dt(Real)		`| set time step
 */

}  // namespace simpla
#endif /* CORE_MESH_STRUCTURED_COORDINATES_COORDINATES_H_ */
