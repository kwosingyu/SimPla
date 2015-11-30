/**
 * @file polygon.cpp
 * @author salmon
 * @date 2015-11-18.
 */
#include "../gtl/ntuple_ext.h"
#include "polygon.h"
#include "geo_algorithm.h"

namespace simpla { namespace geometry
{

int Polygon<2>::box_intersection(point_type *x0, point_type *x1) const
{
    return geometry::box_intersection(m_x0_, m_x1_, x0, x1);
}

Real  Polygon<2>::nearest_point(point_type *p) const
{
    vector_type v0;

    return normals(p, &v0);
}

Real Polygon<2>::normals(point_type *x, vector_type *v0) const
{

    typedef nTuple<Real, 2> Vec2;

    point2d_type x0;

    x0 = *x;

    point2d_type p0 = m_polygon_.back();

    point2d_type p;

    auto it = m_polygon_.begin();

    Real d2 = std::numeric_limits<Real>::max();

    while (it != m_polygon_.end())
    {

        auto const &p1 = *it;


        Vec2 u, v;

        u = x0 - p0;
        v = p1 - p0;

        Real v2 = inner_product(v, v);

        auto s = inner_product(u, v) / v2;

        if (s < 0) { s = 0; }
        else if (s > 1) { s = 1; }

        p = p0 * (1 - s) + s * (p0);

        Real dd = (u[1] * v[0] - u[0] * v[1]) / std::sqrt(v2);

        if (std::abs(dd) < std::abs(d2))
        {
            d2 = dd;
            (*x)[0] = p0[0];
            (*x)[1] = p0[1];

            (*v0)[0] = p1[1] - p0[1];
            (*v0)[1] = p0[0] - p1[0];
        }
        p0 = p1;
        ++it;
    }

    (*v0)[2] = 0;

    return d2;
}


void Polygon<2>::push_back(point_type const &pp)
{
    point2d_type p;
    p = pp;
    m_polygon_.push_back(p);
}

void Polygon<2>::deploy()
{
    size_t num_of_vertex_ = m_polygon_.size();
    constant_.resize(num_of_vertex_);
    multiple_.resize(num_of_vertex_);

    for (size_t i = 0, j = num_of_vertex_ - 1; i < num_of_vertex_; i++)
    {
        if (m_polygon_[j][1] == m_polygon_[i][1])
        {
            constant_[i] = m_polygon_[i][0];
            multiple_[i] = 0;
        }
        else
        {
            constant_[i] = m_polygon_[i][0]
                           - (m_polygon_[i][1] * m_polygon_[j][0])
                             / (m_polygon_[j][1] - m_polygon_[i][1])
                           + (m_polygon_[i][1] * m_polygon_[i][0])
                             / (m_polygon_[j][1] - m_polygon_[i][1]);
            multiple_[i] = (m_polygon_[j][0] - m_polygon_[i][0])
                           / (m_polygon_[j][1] - m_polygon_[i][1]);
        }
        j = i;
    }

    m_x0_ = m_polygon_.front();
    m_x1_ = m_polygon_.front();

    for (auto const &p:m_polygon_)
    {
        geometry::extent_box(&m_x0_, &m_x1_, p);
    }

    m_x0_[2] = std::numeric_limits<Real>::min();
    m_x1_[2] = std::numeric_limits<Real>::max();

}


int  Polygon<2>::within(point_type const &p) const
{
    Real x = p[0];
    Real y = p[1];
    size_t num_of_vertex_ = m_polygon_.size();

    bool oddNodes = false;

    for (size_t i = 0, j = num_of_vertex_ - 1; i < num_of_vertex_; i++)
    {
        if (((m_polygon_[i][1] < y) && (m_polygon_[j][1] >= y))
            || ((m_polygon_[j][1] < y) && (m_polygon_[i][1] >= y)))
        {
            oddNodes ^= (y * multiple_[i] + constant_[i] < x);
        }

        j = i;
    }

    return oddNodes ? 1 : 0;
}

}//namespace  geometry
}//namespace simpla