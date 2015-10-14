/**
 * @file dataset.h
 *
 *  Created on: 2014-11-10
 *      Author: salmon
 */

#ifndef CORE_DATASET_DATASET_H_
#define CORE_DATASET_DATASET_H_

#include <stddef.h>
#include <algorithm>
#include <memory>
#include <type_traits>

#include "../check_concept.h"
#include "../properties.h"
#include "../type_traits.h"
#include "../utilities/memory_pool.h"
#include "dataspace.h"
#include "datatype.h"

namespace simpla
{
/**
 * @addtogroup dataset Dataset
 * @brief This section describes the interface of data set.
 *
 * @ref dataset  is a group of classes used to exchange data between different libraries
 * and program languages in the memory. For example, we can transfer an array
 * of particle structure in memory to hdf5 library, and save it to disk.
 */

/**
 * @ingroup dataset
 *
 * @brief Describe structure of data in the memory.
 *
 * A DataSet is composed of a pointer to raw data , a description
 * of element data type (DataType), a description of memory layout of
 * data set (DataSpace),and a container of meta data (Properties).
 */

struct DataSet
{
	std::shared_ptr<void> data;
	DataType datatype;
	DataSpace dataspace;
	Properties properties;

	typedef long index_type;

	DataSet()
	{
	}
	DataSet(DataSet const &other) :
			data(other.data), datatype(other.datatype), dataspace(
					other.dataspace), properties(other.properties)
	{
	}
//	DataSet(DataSet &&other) :
//			data(other.data), datatype(other.datatype), dataspace(
//					other.dataspace), properties(other.properties)
//	{
//	}
	~DataSet()
	{
	}
	bool is_valid() const
	{
		return data != nullptr && datatype.is_valid() && dataspace.is_valid();
	}

	std::ostream & print(std::ostream & os) const;
};

namespace _impl
{
HAS_MEMBER_FUNCTION(dataset)
}  // namespace _impl

template<typename T>
auto make_dataset(T & d)->
typename std::enable_if<_impl:: has_member_function_dataset<T >::value,
decltype(d.dataset())>::type
{
	return std::move(d.dataset());
}

template<typename T>
auto make_dataset(T * d)->
typename std::enable_if< _impl:: has_member_function_dataset<T >::value,
decltype(d->dataset())>::type
{
	return std::move(d->dataset());
}

template<typename T, typename TI>
DataSet make_dataset(T * p, int rank, TI const * dims, Properties const & prop =
		Properties())
{

	DataSet res;

	res.datatype = traits::datatype<T>::create();
	res.dataspace = DataSpace::create_simple(rank, dims);
//	res.data = std::shared_ptr<void>(
//			const_cast<void*>(reinterpret_cast<typename std::conditional<
//					std::is_const<T>::value, void const *, void *>::type>(p)),
//			do_nothing());
	res.properties = prop;

	return std::move(res);
}

template<typename T, typename TI>
DataSet make_dataset(std::shared_ptr<T> p, int rank, TI const * dims,
		Properties const & prop = Properties())
{

	DataSet res;
	res.data = p;
	res.datatype = traits::datatype<T>::create();
	res.dataspace = make_dataspace(rank, dims);
	res.properties = prop;

	return std::move(res);
}

template<typename T>
DataSet make_dataset(std::vector<T> const & p)
{

	DataSet res;
	long num = p.size();
	res.datatype = traits::datatype<T>::create();
	res.dataspace = DataSpace::create_simple(1, &num);
//	res.data = std::shared_ptr<void>(
//			const_cast<void*>(reinterpret_cast<void const *>(&p[0])),
//			do_nothing());

	return std::move(res);
}
/**@}*/

}  // namespace simpla

#endif /* CORE_DATASET_DATASET_H_ */
