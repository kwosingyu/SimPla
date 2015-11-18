/**
 * @file dataset.h
 * @author salmon
 * @date 2015-10-14.
 */

#ifndef SIMPLA_DATASET_H
#define SIMPLA_DATASET_H

#include "../../dataset/dataspace.h"
#include "../manifold_traits.h"

namespace simpla
{
template<typename ...> struct DataSetPolicy;

template<typename TGeo>
struct DataSetPolicy<TGeo>
{
private:

    typedef TGeo geometry_type;

    typedef DataSetPolicy<geometry_type> this_type;

    geometry_type const &m_geo_;


public:
    DataSetPolicy(geometry_type &geo) : m_geo_(geo) { }

    virtual ~DataSetPolicy() { }

    template<typename TDict> void load(TDict const &) { }

    template<typename OS> OS &print(OS &os) const
    {
        os << "\t DataSetPolicy={ Default }," << std::endl;


        return os;
    }

    void deploy() { }


    template<size_t IFORM>
    DataSpace dataspace() const
    {
        return dataspace<IFORM>(m_geo_.template range<IFORM>());
    }

    template<size_t IFORM>
    DataSpace dataspace(typename geometry_type::range_type const &r) const
    {

        static constexpr int ndims = geometry_type::ndims;

        nTuple<size_t, ndims + 1> f_dims;
        nTuple<size_t, ndims + 1> f_offset;
        nTuple<size_t, ndims + 1> f_count;
        nTuple<size_t, ndims + 1> f_ghost_width;

        nTuple<size_t, ndims + 1> m_dims;
        nTuple<size_t, ndims + 1> m_offset;

        int f_ndims = ndims;


        f_dims = (m_geo_.m_max_ - m_geo_.m_min_);

        f_offset = (m_geo_.m_local_min_ - m_geo_.m_min_);

        f_count = (m_geo_.m_local_max_ - m_geo_.m_local_min_);

        m_dims = (m_geo_.m_memory_max_ - m_geo_.m_memory_min_);

        m_offset = (m_geo_.m_local_min_ - m_geo_.m_min_);

        if ((IFORM == EDGE || IFORM == FACE))
        {
            f_ndims = ndims + 1;
            f_dims[ndims] = 3;
            f_offset[ndims] = 0;
            f_count[ndims] = 3;
            m_dims[ndims] = 3;
            m_offset[ndims] = 0;
        }
        else
        {
            f_ndims = ndims;
            f_dims[ndims] = 1;
            f_offset[ndims] = 0;
            f_count[ndims] = 1;
            m_dims[ndims] = 1;
            m_offset[ndims] = 0;
        }

        DataSpace res(f_ndims, &(f_dims[0]));

        res.select_hyperslab(&f_offset[0], nullptr, &f_count[0], nullptr).set_local_shape(&m_dims[0], &m_offset[0]);

        return std::move(res);

    }
};//template<typename TGeo> struct DataSetPolicy


namespace traits
{

template<typename TGeo>
struct type_id<DataSetPolicy<TGeo>>
{
    static std::string name()
    {
        return "DataSetPolicy<" + type_id<TGeo>::name() + ">";
    }
};
}
}//namespace simpla
#endif //SIMPLA_DATASET_H
