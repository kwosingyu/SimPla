/**
 * @file topology.h.h
 * @author salmon
 * @date 2015-10-14.
 */

#ifndef SIMPLA_TOPOLOGY_H_H
#define SIMPLA_TOPOLOGY_H_H
namespace simpla
{

template<typename.../* tags */> struct Topology;

namespace topology
{
namespace tags
{

//! @ref http://www.xdmf.org/index.php/XDMF_Model_and_Format
//! @addgroup Linear
//! @{
struct Polyvertex; //- a group of unconnected points
struct Polyline; //- a group of line segments
struct Polygon; //
struct Triangle; //
struct Quadrilateral; //
struct Tetrahedron; //
struct Pyramid; //
struct Wedge; //
struct Hexahedron; //
//! @}
//! @addgroup Quadratic
//! @{
struct Edge_3; //- Quadratic line with 3 nodes
struct Tri_6; //
struct Quad_8; //
struct Tet_10; //
struct Pyramid_13; //
struct Wedge_15; //
struct Hex_20; //
//! @}
//! @addgroup Arbitrary
//! @{
struct Mixed; //- a mixture of unstructured cells
//! @}
//! @addgroup Structured
//! @{
struct Curvilinear; // - Curvilinear
struct RectMesh; // - Axis are perpendicular
struct CoRectMesh; // - Axis are perpendicular and spacing is constant
//! @}

}// namespace tags


namespace traits
{
template<typename  ...> struct point_type;
}
}// namespace topology

}// namespace simpla

#endif //SIMPLA_TOPOLOGY_H_H