/*
 * distributed_array.h
 *
 *  created on: 2014-5-30
 *      Author: salmon
 */

#ifndef DISTRIBUTED_ARRAY_H_
#define DISTRIBUTED_ARRAY_H_

#include <stddef.h>
#include <string>
#include <tuple>
#include <memory>
namespace simpla
{
struct DataSet;
class Properties;
/* @brief  DistributedArray is used to manage the parallel
 * communication while using the n-dimensional regular array.
 * @note
 *  - DistributedArray is continue in each dimension, which means
 *    no `strides` or `blocks`
 *  -
 *
 * inspired by :
 *  - DMDA in PETSc
 *     http://www.mcs.anl.gov/petsc/petsc-current/docs/manualpages/DM/index.html
 *
 *
 *
 **/

struct DistributedArray: public std::enable_shared_from_this<DistributedArray>
{
	static constexpr size_t MAX_NUM_DIMS = 10;

//	template<typename ...Args>
//	DistributedArray(Args && ... args)
//	{
//		init(std::forward<Args>(args)...);
//	}

	DistributedArray();

	~DistributedArray();

	bool is_valid() const;

	Properties const &properties(std::string const& key = "") const;

	Properties &properties(std::string const& key = "");

	size_t num_of_dims() const;

	/**
	 * @return tuple<global_start,global_count>
	 */
	std::tuple<size_t const*, size_t const*> global_shape() const;

	/**
	 * @return tuple<local_outer_start,local_outer_count>
	 */
	std::tuple<size_t const*, size_t const*> local_shape() const;

	/**
	 * @return tuple<local_inner_start,local_inner_count>
	 */
	std::tuple<size_t const*, size_t const*> inner_shape() const;

	void init(size_t nd, size_t const * start, size_t const* count, size_t gw =
			2);

	bool sync_ghosts(DataSet* ds, size_t flag = 0) const;

//	void init(int argc = 0, char** argv = nullptr);
//
//	template<typename TI>
//	void init(size_t nd, TI const & b, TI const & e, size_t gw = 2)
//	{
//		ndims = nd;
//
//		for (int i = 0; i < nd; ++i)
//		{
//			global_begin_[i] = b[i];
//			global_end_[i] = e[i];
//		}
//		Decompose(gw);
//	}
//
//	size_t size() const
//	{
//		size_t res = 1;
//
//		for (int i = 0; i < ndims; ++i)
//		{
//			res *= (local_.inner_end[i] - local_.inner_begin[i]);
//		}
//		return res;
//	}
//	size_t memory_size() const
//	{
//		size_t res = 1;
//
//		for (int i = 0; i < ndims; ++i)
//		{
//			res *= (local_.outer_end[i] - local_.outer_begin[i]);
//		}
//		return res;
//	}
//
//	void Decompose(size_t gw = 2);
//
//	size_t global_begin_[ndims];
//	size_t global_end_[ndims];
//	size_t global_strides_[ndims];
//
//	struct sub_array_s
//	{
//		size_t outer_begin[3];
//		size_t outer_end[3];
//		size_t inner_begin[3];
//		size_t inner_end[3];
//	};
//	sub_array_s local_;
//
//	struct send_recv_s
//	{
//		int dest;
//		int send_tag;
//		int recv_tag;
//		size_t send_begin[ndims];
//		size_t send_end[ndims];
//		size_t recv_begin[ndims];
//		size_t recv_end[ndims];
//	};
//
//	std::vector<send_recv_s> send_recv_; // dest, send_tag,recv_tag, sub_array_s
//
//	template<typename TI>
//	int hash(TI const & d) const
//	        {
//		int res = 0;
//		for (int i = 0; i < ndims; ++i)
//		{
//			res += ((d[i] - global_begin_[i]
//			        + (global_end_[i] - global_begin_[i]))
//			        % (global_end_[i] - global_begin_[i])) * global_strides_[i];
//		}
//		return res;
//	}
private:
	struct pimpl_s;
	pimpl_s* pimpl_;

}
;

//void update_ghosts(void * data, DataType const & data_type,
//		DistributedArray const & global_array);
//
//template<typename TV>
//void update_ghosts(std::shared_ptr<TV> data,
//		DistributedArray const & global_array)
//{
//	update_ghosts(data.get(), DataType::create<TV>(), global_array);
//}
//
//template<typename TV>
//void update_ghosts(TV * data, DistributedArray const & global_array)
//{
//	update_ghosts(data, DataType::create<TV>(), global_array);
//}
}
// namespace simpla

#endif /* DISTRIBUTED_ARRAY_H_ */
