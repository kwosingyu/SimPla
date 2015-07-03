//
// Created by salmon on 7/3/15.
//

#ifndef SIMPLA_MESH_LAYOUT_H
#define SIMPLA_MESH_LAYOUT_H

#include "../parallel/mpi_comm.h"
#include "../parallel/mpi_update.h"
#include "../dataset/dataspace.h"

#include "mesh_ids.h"

#include <vector>
#include <vector>
#include <memory>

namespace simpla
{
template<int NDIMS> struct MeshBlock;

template<int NDIMS>
struct MeshLayout
{
	static constexpr int ndims = NDIMS;

	typedef MeshIDs_<ndims> MeshIDs;

	typedef typename MeshIDs::id_type id_type;


	static constexpr size_t DEFAULT_GHOST_WIDTH;


	id_type m_id_zero_;

	id_type m_id_max_;


	void deploy(size_t const *gw = nullptr)
	{
/**
* Decompose
*/

		if (GLOBAL_COMM.num_of_process() > 1)
		{
			auto idx_b = MeshIDs::unpack_index(m_id_zero_);

			auto idx_e = MeshIDs::unpack_index(m_id_max_);

			GLOBAL_COMM.
					decompose(static_cast<int>(ndims), &idx_b[0], &idx_e[0]
			);

			typename MeshIDs::index_tuple ghost_width;

			if (gw != nullptr)
			{
				ghost_width = gw;
			}
			else
			{
				ghost_width = DEFAULT_GHOST_WIDTH;
			}

			for (
					int i = 0;
					i < ndims;
					++i)
			{

				if (idx_b[i] + 1 == idx_e[i])
				{
					ghost_width[i] = 0;
				}
				else if (idx_e[i] <= idx_b[i] + ghost_width[i] * 2)
				{
					ERROR(
							"Dimension is to small to split!["
//				" Dimensions= " + type_cast < std::string
//				> (MeshIDs ::unpack_index(
//								m_id_max_ - m_id_min_))
//				+ " , Local dimensions=" + type_cast
//				< std::string
//				> (MeshIDs ::unpack_index(
//								m_id_local_max_ - m_id_local_min_))
//				+ " , Ghost width =" + type_cast
//				< std::string > (ghost_width) +
									"]");
				}

			}

			m_id_local_min_ = MeshIDs::pack_index(idx_b);

			m_id_local_max_ = MeshIDs::pack_index(idx_e);

			m_id_memory_min_ = m_id_local_min_ - MeshIDs::pack_index(ghost_width);

			m_id_memory_max_ = m_id_local_max_ + MeshIDs::pack_index(ghost_width);


		}
		else
		{
			m_id_local_min_ = m_id_min_;

			m_id_local_max_ = m_id_max_;

			m_id_memory_min_ = m_id_local_min_;

			m_id_memory_max_ = m_id_local_max_;

		}

	}

	template<size_t IFORM>
	DataSpace dataspace() const
	{
		nTuple<size_t, ndims + 1> f_dims;
		nTuple<size_t, ndims + 1> f_offset;
		nTuple<size_t, ndims + 1> f_count;
		nTuple<size_t, ndims + 1> f_ghost_width;

		nTuple<size_t, ndims + 1> m_dims;
		nTuple<size_t, ndims + 1> m_offset;

		int f_ndims = ndims;

		f_dims = MeshIDs::unpack_index(m_id_max_ - m_id_min_);

		f_offset = MeshIDs::unpack_index(m_id_local_min_ - m_id_min_);

		f_count = MeshIDs::unpack_index(
				m_id_local_max_ - m_id_local_min_);

		m_dims = MeshIDs::unpack_index(
				m_id_memory_max_ - m_id_memory_min_);;

		m_offset = MeshIDs::unpack_index(m_id_local_min_ - m_id_min_);

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

		res.select_hyperslab(&f_offset[0], nullptr, &f_count[0], nullptr)
				.set_local_shape(&m_dims[0], &m_offset[0]);

		return std::move(res);

	}

	template<size_t IFORM>
	void ghost_shape(
			std::vector<mpi_ghosts_shape_s> *res) const
	{
		nTuple<size_t, ndims + 1> f_local_dims;
		nTuple<size_t, ndims + 1> f_local_offset;
		nTuple<size_t, ndims + 1> f_local_count;
		nTuple<size_t, ndims + 1> f_ghost_width;

		int f_ndims = ndims;

//		f_local_dims = MeshIDs ::unpack_index(
//				m_id_memory_max_ - m_id_memory_min_);

		f_local_offset = MeshIDs::unpack_index(
				m_id_local_min_ - m_id_memory_min_);

		f_local_count = MeshIDs::unpack_index(
				m_id_local_max_ - m_id_local_min_);

		f_ghost_width = MeshIDs::unpack_index(
				m_id_local_min_ - m_id_memory_min_);

		if ((IFORM == EDGE || IFORM == FACE))
		{
			f_ndims = ndims + 1;
			f_local_offset[ndims] = 0;
			f_local_count[ndims] = 3;
			f_ghost_width[ndims] = 0;
		}
		else
		{
			f_ndims = ndims;

//			f_local_dims[ndims] = 1;
			f_local_offset[ndims] = 0;
			f_local_count[ndims] = 1;
			f_ghost_width[ndims] = 0;

		}

		get_ghost_shape(f_ndims, &f_local_offset[0], nullptr, &f_local_count[0],
				nullptr, &f_ghost_width[0], res);

	}

	template<size_t IFORM>
	std::vector<mpi_ghosts_shape_s> ghost_shape() const
	{
		std::vector<mpi_ghosts_shape_s> res;
		ghost_shape<IFORM>(&res);
		return std::move(res);
	}


};
}// namespace simpla
#endif //SIMPLA_MESH_LAYOUT_H