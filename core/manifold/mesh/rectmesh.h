/**
 * @file rect_mesh.h
 * @author salmon
 * @date 2015-10-27.
 */

#ifndef SIMPLA_RECT_MESH_H
#define SIMPLA_RECT_MESH_H

#include <limits>

#include "mesh_ids.h"
#include "mesh_block.h"
#include "map_linear.h"
#include "../../gtl/design_pattern/singleton_holder.h"
#include "../../gtl/utilities/memory_pool.h"

namespace simpla { namespace mesh
{
/**
 * @ingroup mesh
 *
 * @brief non-Uniform structured mesh
 */
template<typename TMap = LinearMap, int MeshLevel = 4>
struct RectMesh : public MeshBlock<MeshLevel>, public TMap
{

private:
    typedef RectMesh<TMap, MeshLevel> this_type;
    typedef MeshBlock<MeshLevel> base_type;
    typedef TMap map_type;
public:
    using base_type::ndims;

    using typename base_type::id_type;
    using typename base_type::id_tuple;
    using typename base_type::index_type;
    using typename base_type::index_tuple;
    using typename base_type::difference_type;

    typedef nTuple<Real, ndims> point_type;
    typedef nTuple<Real, ndims> vector_type;

private:
    point_type m_coords_min_ = {0, 0, 0};

    point_type m_coords_max_ = {1, 1, 1};

    vector_type m_dx_ = {1, 1, 1};; //!< width of cell, except m_dx_[i]=0 when m_dims_[i]==1

    bool m_is_valid_ = false;
public:


    RectMesh() : base_type()
    {

    }


    RectMesh(this_type const &other) :
            base_type(other), m_dx_(other.m_dx_)
    {

    }

    virtual  ~RectMesh() { }

    virtual void swap(this_type &other)
    {

        base_type::swap(other);

        map_type::swap(other);

        std::swap(m_coords_min_, other.m_coords_min_);
        std::swap(m_coords_max_, other.m_coords_max_);
        std::swap(m_dx_, other.m_dx_);


        deploy();
        other.deploy();

    }

    this_type &operator=(this_type const &other)
    {
        this_type(other).swap(*this);
        return *this;
    }


    template<typename TDict>
    void load(TDict const &dict)
    {
        box(dict["Geometry"]["Box"].template as<std::tuple<point_type, point_type> >());

        base_type::dimensions(
                dict["Geometry"]["Topology"]["Dimensions"].template as<index_tuple>(index_tuple{10, 1, 1}));
    }

    template<typename OS>
    OS &print(OS &os) const
    {

        os << "\t\tTopology = {" << std::endl
        << "\t\t Type = \"RectMesh\",  }," << std::endl
        << "\t\t Extents = {" << box() << "}," << std::endl
        << "\t\t Dimensions = " << base_type::dimensions() << "," << std::endl
        << "\t\t}, " << std::endl;

        return os;
    }


    bool is_valid() const { return m_is_valid_; }

    //================================================================================================
    // @name Coordinates dependent
    //    point_type epsilon() const { return EPSILON * m_from_topology_scale_; }

    template<typename X0, typename X1>
    void box(X0 const &x0, X1 const &x1)
    {
        m_coords_min_ = x0;
        m_coords_max_ = x1;
    }

    template<typename T0> void box(T0 const &b)
    {
        box(simpla::traits::get<0>(b), simpla::traits::get<1>(b));
    }


    std::tuple<point_type, point_type> box() const
    {
        return (std::make_tuple(m_coords_min_, m_coords_max_));
    }

    std::tuple<point_type, point_type> local_box() const
    {
        point_type l_min, l_max;

        std::tie(l_min, l_max) = base_type::local_box();

        l_min = inv_map(l_min);

        l_max = inv_map(l_max);

        return (std::make_tuple(l_min, l_max));
    }

    template<typename ...Args>
    void extents(Args &&...args)
    {
        box(std::forward<Args>(args)...);
    }

    auto extents() const
    DECL_RET_TYPE(box())

    constexpr auto dx() const DECL_RET_TYPE(m_dx_);

private:
    using map_type::map;
    using map_type::inv_map;
public:
    template<typename ...Args>
    point_type point(Args &&...args) const
    {
        return std::move(map_type::map(base_type::point(std::forward<Args>(args)...)));
    }

/**
 * @bug: truncation error of coordinates transform larger than 1000
 *     epsilon (1e4 epsilon for cylindrical coordinates)
 * @param args
 * @return
 *
 */
    virtual point_type coordinates_local_to_global(std::tuple<id_type, point_type> const &t) const
    {
        return std::move(map(base_type::coordinates_local_to_global(t)));
    }

    virtual std::tuple<id_type, point_type> coordinates_global_to_local(point_type x, int n_id = 0) const
    {
        return std::move(base_type::coordinates_global_to_local(inv_map(x), n_id));
    }

    virtual id_type id(point_type x, int n_id = 0) const
    {
        return std::get<0>(base_type::coordinates_global_to_local(inv_map(x), n_id));
    }

    //===================================
    //
private:
    constexpr size_t hash_(id_type s) const
    {
        return base_type::hash(s & (~base_type::_DA)) * base_type::NUM_OF_NODE_ID +
               base_type::node_id(s);
    }

public:
    virtual Real volume(id_type s) const
    {
        return m_volume_.get()[hash_(s)];
    }

    virtual Real dual_volume(id_type s) const
    {
        return m_dual_volume_.get()[hash_(s)];
    }

    virtual Real inv_volume(id_type s) const
    {

        return m_inv_volume_.get()[hash_(s)];
    }

    virtual Real inv_dual_volume(id_type s) const
    {
        return m_inv_dual_volume_.get()[hash_(s)];
    }

    virtual point_type const &vertex(id_type s) const
    {
        return m_vertices_.get()[base_type::hash(s) * base_type::NUM_OF_NODE_ID + base_type::sub_index(s)];
    }


private:
    std::shared_ptr<Real> m_volume_;
    std::shared_ptr<Real> m_dual_volume_;
    std::shared_ptr<Real> m_inv_volume_;
    std::shared_ptr<Real> m_inv_dual_volume_;
    std::shared_ptr<point_type> m_vertices_;
public:

    virtual void deploy();

    template<typename TGeo> void update_volume(TGeo const &geo);
};//struct RectMesh

template<typename TMap, int MeshLevel>
void RectMesh<TMap, MeshLevel>::deploy()
{
    base_type::deploy();

    auto dims = base_type::dimensions();

    map_type::set(base_type::box(), box(), dims);

    for (int i = 0; i < ndims; ++i)
    {
        ASSERT(dims[i] >= 1);

        ASSERT((m_coords_max_[i] - m_coords_min_[i] > EPSILON));

        m_dx_[i] = (m_coords_max_[i] - m_coords_min_[i]) / static_cast<Real>(dims[i]);
    }

    /**
     *\verbatim
     *                ^y
     *               /
     *        z     /
     *        ^    /
     *        |  110-------------111
     *        |  /|              /|
     *        | / |             / |
     *        |/  |            /  |
     *       100--|----------101  |
     *        | m |           |   |
     *        |  010----------|--011
     *        |  /            |  /
     *        | /             | /
     *        |/              |/
     *       000-------------001---> x
     *
     *\endverbatim
     */



    m_is_valid_ = true;
}

template<typename TMap, int MeshLevel>
template<typename TGeo>
void RectMesh<TMap, MeshLevel>::update_volume(TGeo const &geo)
{

    auto memory_block_range = base_type::template make_range<VERTEX>(base_type::memory_index_box());

    size_t num = memory_block_range.size() * base_type::NUM_OF_NODE_ID;

    m_volume_ = SingletonHolder<MemoryPool>::instance().alloc<Real>(num);
    m_inv_volume_ = SingletonHolder<MemoryPool>::instance().alloc<Real>(num);
    m_dual_volume_ = SingletonHolder<MemoryPool>::instance().alloc<Real>(num);
    m_inv_dual_volume_ = SingletonHolder<MemoryPool>::instance().alloc<Real>(num);

#ifdef USE_PARALLEL_FOR
    parallel_for(memory_block_range, [&](range_type const &range){
#else
    auto const &range = memory_block_range;
#endif
    for (auto const &s:range)
    {
        size_t n = this->hash(s) * base_type::NUM_OF_NODE_ID;

        base_type::get_element_volume_in_cell(geo, s, m_volume_.get() + n, m_inv_volume_.get() + n,
                                              m_dual_volume_.get() + n, m_inv_dual_volume_.get() + n);

    }
#ifdef USE_PARALLEL_FOR
    });
#endif

}

}}  // namespace mesh // namespace simpla

#endif //SIMPLA_RECT_MESH_H