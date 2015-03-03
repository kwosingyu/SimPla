/**
 * @file dataspace.h
 *
 *  Created on: 2014年11月10日
 *  @author: salmon
 */

#ifndef CORE_DATASET_DATASPACE_H_
#define CORE_DATASET_DATASPACE_H_

#include <stddef.h>
#include <map>
#include <tuple>

#include "../gtl/properties.h"

namespace simpla
{

struct DataSet;
/**
 * @ingroup data_interface
 * @brief  Define the size and  shape of data set in memory/file
 *  Ref. http://www.hdfgroup.org/HDF5/doc/UG/UG_frame12Dataspaces.html
 */
class DataSpace
{
public:
	Properties properties;

	// Creates a null dataspace
	DataSpace();

	DataSpace(int rank, size_t const * dims, const size_t * gw = nullptr);

	// Copy constructor: makes a copy of the original DataSpace object.
	DataSpace(const DataSpace& other);

	// Assignment operator
	DataSpace& operator=(const DataSpace& rhs);

	// Destructor: properly terminates access to this dataspace.
	~DataSpace();

	void swap(DataSpace &);

	void create(int rank, const size_t * dims, const size_t * gw = nullptr);

	void select_hyperslab(size_t const *offset, size_t const * count,
			size_t const * stride = nullptr, size_t const * block = nullptr);

	bool is_valid() const;

	bool is_simple() const
	{
		/// TODO support  complex selection of data space
		/// @ref http://www.hdfgroup.org/HDF5/doc/UG/UG_frame12Dataspaces.html
		return true;
	}

	size_t size() const;

	/**
	 * @return <ndims,dimensions,start,count,stride,block>
	 */
	std::tuple<size_t, size_t const *, size_t const *, size_t const *,
			size_t const *, size_t const *> shape() const;

	size_t const * ghost_width() const;
	DataSpace local() const;
	DataSpace global() const;

private:
	struct pimpl_s;
	std::unique_ptr<pimpl_s> pimpl_;

};
/**
 * @ingroup data_interface
 * create dataspace
 * @param args
 * @return
 */
template<typename ... Args>
DataSpace make_dataspace(Args && ... args)
{
	return DataSpace(std::forward<Args>(args)...);
}

/**@}  */

}  // namespace simpla

#endif /* CORE_DATASET_DATASPACE_H_ */