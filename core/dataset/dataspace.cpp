/**
 * @file dataspace.cpp
 *
 *  Created on: 2014年11月13日
 *      @author: salmon
 */

#include <algorithm>
#include <utility>

#include "../gtl/ntuple.h"
#include "../numeric/geometric_algorithm.h"
#include "dataspace.h"

namespace simpla
{
struct DataSpace::pimpl_s
{
	typedef nTuple<size_t, 10> index_tuple;

	size_t m_ndims_ = 3;

	index_tuple m_dimensions_;
	index_tuple m_count_;
	index_tuple m_offset_;
	index_tuple m_stride_;
	index_tuple m_block_;
	index_tuple m_local_ghost_width_;

};

//===================================================================

DataSpace::DataSpace()
		: pimpl_ { nullptr }
{
}
DataSpace::DataSpace(const DataSpace& other)
		: pimpl_ { new pimpl_s }
{
	if (other.pimpl_ != nullptr)
	{
		pimpl_->m_ndims_ = other.pimpl_->m_ndims_;
		pimpl_->m_dimensions_ = other.pimpl_->m_dimensions_;
		pimpl_->m_count_ = other.pimpl_->m_count_;
		pimpl_->m_offset_ = other.pimpl_->m_offset_;
		pimpl_->m_stride_ = other.pimpl_->m_stride_;
		pimpl_->m_block_ = other.pimpl_->m_block_;
		pimpl_->m_local_ghost_width_ = other.pimpl_->m_local_ghost_width_;
	}

}

DataSpace::DataSpace(int rank, size_t const * dims, const size_t * gw)
{
	create(rank, dims, gw);
}
DataSpace::~DataSpace()
{
}

DataSpace& DataSpace::operator=(const DataSpace& rhs)
{
	DataSpace(rhs).swap(*this);
	return *this;
}

void DataSpace::swap(DataSpace &other)
{
	std::swap(pimpl_->m_ndims_, other.pimpl_->m_ndims_);
	std::swap(pimpl_->m_dimensions_, other.pimpl_->m_dimensions_);
	std::swap(pimpl_->m_count_, other.pimpl_->m_count_);
	std::swap(pimpl_->m_offset_, other.pimpl_->m_offset_);
	std::swap(pimpl_->m_stride_, other.pimpl_->m_stride_);
	std::swap(pimpl_->m_block_, other.pimpl_->m_block_);
	std::swap(pimpl_->m_local_ghost_width_, other.pimpl_->m_local_ghost_width_);
}
void DataSpace::create(int rank, const size_t * dims, const size_t * gw)
{
	if (pimpl_ == nullptr)
		pimpl_ = std::unique_ptr<pimpl_s> { new pimpl_s };

	pimpl_->m_ndims_ = rank;
	pimpl_->m_dimensions_ = dims;
	pimpl_->m_count_ = dims;
	pimpl_->m_offset_ = 0;
	pimpl_->m_stride_ = 1;
	pimpl_->m_block_ = 1;

	if (gw == nullptr)
	{
		pimpl_->m_local_ghost_width_ = 2;
	}
	else
	{
		pimpl_->m_local_ghost_width_ = gw;
	}

}

bool DataSpace::is_valid() const
{
	return pimpl_ != nullptr;
}

DataSpace DataSpace::local() const
{
	pimpl_s::index_tuple local_dims, local_offset;

	local_dims = (pimpl_->m_count_ + pimpl_->m_local_ghost_width_ * 2)
			* pimpl_->m_stride_;

	local_offset = pimpl_->m_local_ghost_width_ * pimpl_->m_stride_;

	DataSpace res(pimpl_->m_ndims_, &local_dims[0], nullptr);

	res.select_hyperslab(&pimpl_->m_count_[0], &local_offset[0],
			&pimpl_->m_stride_[0], &pimpl_->m_block_[0]);

	return std::move(res);
}
DataSpace DataSpace::global() const
{
	return *this;
}
size_t const * DataSpace::ghost_width() const
{
	return &(pimpl_->m_local_ghost_width_[0]);
}

size_t DataSpace::size() const
{
	size_t res = 1;

	for (int i = 0; i < pimpl_->m_ndims_; ++i)
	{
		res *= pimpl_->m_dimensions_[i];
	}

	return res;
}

std::tuple<size_t, size_t const *, size_t const *, size_t const *,
		size_t const *, size_t const *> DataSpace::shape() const
{
	return std::make_tuple(pimpl_->m_ndims_, //
			&pimpl_->m_dimensions_[0], //
			&pimpl_->m_count_[0], //
			&pimpl_->m_offset_[0], //
			&pimpl_->m_stride_[0], //
			&pimpl_->m_block_[0]);
}

void DataSpace::select_hyperslab(size_t const * count, size_t const * offset,
		size_t const * stride, size_t const * block)
{
	if (pimpl_ != nullptr)
	{
		if (offset != nullptr)
			pimpl_->m_offset_ = offset;
		if (count != nullptr)
			pimpl_->m_count_ = count;
		if (stride != nullptr)
			pimpl_->m_stride_ = stride;
		if (block != nullptr)
			pimpl_->m_block_ = block;
	}
	else
	{
		RUNTIME_ERROR("DataSpace is invalid!");
	}

}

//
//void decomposer_(size_t num_process, size_t process_num, size_t gw,
//		size_t ndims, size_t const *global_start, size_t const * global_count,
//		size_t * local_outer_start, size_t * local_outer_count,
//		size_t * local_inner_start, size_t * local_inner_count)
//{
//
////FIXME this is wrong!!!
//	for (int i = 0; i < ndims; ++i)
//	{
//		local_outer_count[i] = global_count[i];
//		local_outer_start[i] = global_start[i];
//		local_inner_count[i] = global_count[i];
//		local_inner_start[i] = global_start[i];
//	}
//
//	if (num_process <= 1)
//		return;
//
//	int n = 0;
//	long L = 0;
//	for (int i = 0; i < ndims; ++i)
//	{
//		if (global_count[i] > L)
//		{
//			L = global_count[i];
//			n = i;
//		}
//	}
//
//	if ((2 * gw * num_process > global_count[n] || num_process > global_count[n]))
//	{
//
//		RUNTIME_ERROR("Array is too small to split");
//
////		if (process_num > 0)
////			local_outer_end = local_outer_begin;
//	}
//	else
//	{
//		local_inner_start[n] = (global_count[n] * process_num) / num_process
//				+ global_start[n];
//		local_inner_count[n] = (global_count[n] * (process_num + 1))
//				/ num_process + global_start[n];
//		local_outer_start[n] = local_inner_start[n] - gw;
//		local_outer_count[n] = local_inner_count[n] + gw;
//	}
//
//}
//
//void DataSpace::pimpl_s::decompose()
//{
//
////	local_shape_.dimensions = global_shape_.dimensions;
////	local_shape_.count = global_shape_.count;
////	local_shape_.offset = global_shape_.offset;
////	local_shape_.stride = global_shape_.stride;
////	local_shape_.block = global_shape_.block;
//
////	if (!GLOBAL_COMM.is_valid()) return;
////
////	int num_process = GLOBAL_COMM.get_size();
////	unsigned int process_num = GLOBAL_COMM.get_rank();
////
////	decomposer_(num_process, process_num, gw_, ndims_,  //
////			&global_shape_.offset[0], &global_shape_.count[0],  //
////			&local_outer_shape_.offset[0], &local_outer_shape_.count[0],  //
////			&local_inner_shape_.offset[0], &local_inner_shape_.count[0]);
////
////	self_id_ = (process_num);
////
////	for (int dest = 0; dest < num_process; ++dest)
////	{
////		if (dest == self_id_)
////			continue;
////
////		sub_array_s node;
////
////		decomposer_(num_process, dest, gw, ndims_, &global_shape_.offset[0],
////				&global_shape_.count[0], &node.outer_offset[0],
////				&node.outer_count[0], &node.inner_offset[0],
////				&node.inner_count[0]
////
////				);
////
////		sub_array_s remote;
////
////		for (unsigned long s = 0, s_e = (1UL << (ndims_ * 2)); s < s_e; ++s)
////		{
////			remote = node;
////
////			bool is_duplicate = false;
////
////			for (int i = 0; i < ndims_; ++i)
////			{
////
////				int n = (s >> (i * 2)) & 3UL;
////
////				if (n == 3)
////				{
////					is_duplicate = true;
////					continue;
////				}
////
////				auto L = global_shape_.count[i] * ((n + 1) % 3 - 1);
////
////				remote.outer_offset[i] += L;
////				remote.inner_offset[i] += L;
////
////			}
////			if (!is_duplicate)
////			{
////				bool f_inner = Clipping(ndims_, local_outer_shape_.offset,
////						local_outer_shape_.count, remote.inner_offset,
////						remote.inner_count);
////				bool f_outer = Clipping(ndims_, local_inner_shape_.offset,
////						local_inner_shape_.count, remote.outer_offset,
////						remote.outer_count);
////
////				bool flag = f_inner && f_outer;
////
////				for (int i = 0; i < ndims_; ++i)
////				{
////					flag = flag && (remote.outer_count[i] != 0);
////				}
////				if (flag)
////				{
////					send_recv_.emplace_back(
////							send_recv_s(
////									{ dest, hash(&remote.outer_offset[0]), hash(
////											&remote.inner_offset[0]),
////											remote.outer_offset,
////											remote.outer_count,
////											remote.inner_offset,
////											remote.inner_count }));
////				}
////			}
////		}
////	}
//
//	is_valid_ = true;
//}

//bool DataSpace::sync(std::shared_ptr<void> data, DataType const & datatype,
//		size_t flag)
//{
//#if  !NO_MPI || USE_MPI
//	if (!GLOBAL_COMM.is_valid() || pimpl_->send_recv_.size() == 0)
//	{
//		return true;
//	}
//
//	MPI_Comm comm = GLOBAL_COMM.comm();
//
//	MPI_Request request[pimpl_->send_recv_.size() * 2];
//
//	int count = 0;
//
//	for (auto const & item : pimpl_->send_recv_)
//	{
//
//		MPIDataType send_type = MPIDataType::create(datatype, pimpl_->local_shape_.ndims ,
//		&pimpl_->local_shape_.dimensions[0], & item.send.offset[0],
//		&item.send.stride[0], &item.send.count[0], &item.send.block[0]);
//
//		dims_type recv_offset;
//		recv_offset = item.recv.offset - pimpl_->local_shape_.offset;
//
//		MPIDataType recv_type = MPIDataType::create(datatype, pimpl_->local_shape_.ndims ,
//		&pimpl_->local_shape_.dimensions[0], & item.recv.offset[0],
//		&item.recv.stride[0], &item.recv.count[0], &item.recv.block[0]);
//
//		MPI_Isend(data.get(), 1, send_type.type(), item.dest, item.send_tag,
//		comm, &request[count * 2]);
//		MPI_Irecv(data.get(), 1, recv_type.type(), item.dest, item.recv_tag,
//		comm, &request[count * 2 + 1]);
//
//		++count;
//	}
//
//	MPI_Waitall(pimpl_->send_recv_.size() * 2, request, MPI_STATUSES_IGNORE);
//
//#endif //#if  !NO_MPI || USE_MPI
//
//	return true;
//}

}
// namespace simpla
