/*
 * geometry_algorithm.h
 *
 *  Created on: 2014年4月21日
 *      Author: salmon
 */

#ifndef GEOMETRY_ALGORITHM_H_
#define GEOMETRY_ALGORITHM_H_

#include <algorithm>
#include <cmath>
#include <complex>
#include <valarray>

#include "../fetl/ntuple.h"
#include "../fetl/primitives.h"

namespace simpla
{
template<typename TL, typename TR>
auto _DOT3(nTuple<3, TL> const & l, nTuple<3, TR> const & r)->decltype(l[0]*r[0])
{
	return l[0] * r[0] + l[1] * r[1] + l[2] * r[2];
}
inline nTuple<3, Real> DistPoint2Line(nTuple<3, Real> const &x, nTuple<3, Real> const &x0, nTuple<3, Real> const &x1)
{
	nTuple<3, Real> u, v;
	v = x1 - x0;
	u = Cross(Cross(x - x0, v), v) / _DOT3(v, v);
	return std::move(u);
}
inline Real DistPoint2Plane(nTuple<3, Real> const &x, nTuple<3, Real> const &x0, nTuple<3, Real> const &x1,
        nTuple<3, Real> const &x2)
{
	nTuple<3, Real> v;
	v = Cross(x1 - x0, x2 - x0);
	return _DOT3(x - x0, v) / std::sqrt(_DOT3(v, v));
}

/**
 *
 *
 *     x' o
 *       /
 *      /
 *     o------------------o
 *  p0  \                      p1
 *       \
 *        o
 *        x
 *
 *
 */
template<typename TPlane>
inline void Relection(TPlane const & p, nTuple<3, Real>*x, nTuple<3, Real> * v)
{
	nTuple<3, Real> u;

	u = Cross(p[1] - p[0], p[2] - p[0]);

	Real a = _DOT3(u, *x - p[0]);

	if (a < 0)
	{
		Real b = 1.0 / _DOT3(u, u);
		*x -= 2 * a * u * b;
		*v -= 2 * _DOT3(u, *v) * u * b;
	}

}
template<typename TM, typename TDict, typename TSurface>
void CreateSurface(TM const & mesh, TDict const & dict, TSurface * surf)
{
	if (dict["Width"].is_number())
	{
		CreateSurface(surf, dict["Width"].template as<Real>(), surf);
	}
	else
	{
		WARNING << "illegal configuation!";
	}
}
template<typename TM, typename TSurface>
void CreateSurface(TM const & mesh, Real width, TSurface * surf)
{

	typedef typename TSurface::plane_type plane_type;

	auto extent = mesh.GetExtent();
	auto dims = mesh.GetDimensions();
	auto xmin = extent.first;
	auto xmax = extent.second;
	auto d = mesh.GetDx();
	nTuple<3, Real> x0 = { 0, 0, 0 };
	nTuple<3, Real> x1 = { d[0], 0, 0 };
	nTuple<3, Real> x2 = { 0, d[1], 0 };
	nTuple<3, Real> x3 = { 0, 0, d[2] };

	for (auto s : mesh.GetRange(VERTEX))
	{
		auto x = mesh.GetCoordinates(s);

		if (x[0] < xmin[0] + width)
		{
			surf->insert(s, plane_type( { x0, x1, x2 }));
			continue;
		}
		else if (x[0] > xmax[0] - width)
		{
			surf->insert(s, plane_type( { x0, x2, x1 }));
			continue;
		}

		if (x[1] < xmin[1] + width)
		{
			surf->insert(s, plane_type( { x0, x1, x2 }));
			continue;
		}
		else if (x[1] > xmax[1] + width)
		{
			surf->insert(s, plane_type( { x0, x1, x2 }));
			continue;
		}

		if (x[2] < xmin[2] + width)
		{
			surf->insert(s, plane_type( { x0, x1, x2 }));
			continue;
		}
		else if (x[2] > xmax[2] - width)
		{
			surf->insert(s, plane_type( { x0, x1, x2 }));
			continue;
		}

	}
}
}  // namespace simpla

#endif /* GEOMETRY_ALGORITHM_H_ */