/**
 * @file datatype.h
 *
 *  created on: 2014-6-2
 *      Author: salmon
 */

#ifndef DATA_TYPE_H_
#define DATA_TYPE_H_

#include <stddef.h>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <vector>

#include "../gtl/type_traits.h"

namespace simpla
{
/**
 *  @ingroup data_interface
 *
 *  \brief  Description of data type
 *
 *  @todo this class should meet the requirement of XDR
 *  http://en.wikipedia.org/wiki/External_Data_Representation#XDR_data_types
 *  see   eXternal Data Representation Standard: Protocol Specification
 *        eXternal Data Representation: Sun Technical Notes
 *        XDR: External Data Representation Standard, RFC 1014, Sun Microsystems, Inc., USC-ISI.
 *        doc/reference/xdr/
 *
 */
struct DataType
{
	DataType();

	DataType(std::type_index t_index, size_t ele_size_in_byte, int ndims = 0, size_t const *dims = nullptr,
			std::string name = "");

	DataType(const DataType &other);

	~DataType();

	DataType &operator=(DataType const &other);

	void swap(DataType &);

	bool is_valid() const;

	std::string name() const;

	size_t size() const;

	size_t size_in_byte() const;

	size_t ele_size_in_byte() const;

	int rank() const;

	DataType element_type() const;

	size_t extent(size_t n) const;

	void extent(size_t *d) const;

	void extent(int rank, size_t const *d);

	std::vector<size_t> const &extents() const;

	bool is_compound() const;

	bool is_array() const;

	bool is_opaque() const;

	bool is_same(std::type_index const &other) const;

	template<typename T>
	bool is_same() const
	{
		return is_same(std::type_index(typeid(T)));
	}

	void push_back(DataType &&dtype, std::string const &name, int pos = -1);

	std::vector<std::tuple<DataType, std::string, int>> const &members() const;

private:
	struct pimpl_s;
	std::unique_ptr<pimpl_s> pimpl_;

};

namespace traits
{
template<typename T> struct rank;
template<typename T> struct extents;
template<typename T> struct value_type;

std::ostream &print(std::ostream &os, DataType const &self);

template<typename T>
struct datatype
{
	HAS_STATIC_MEMBER_FUNCTION (datatype)

	static DataType create_(std::string const &name, std::integral_constant<bool, true>)
	{
		return T::datatype();
	}

	static DataType create_(std::string const &name, std::integral_constant<bool, false>)
	{

		typedef typename std::remove_cv<T>::type obj_type;

		typedef typename traits::value_type<obj_type>::type element_type;

		size_t ele_size_in_byte = sizeof(element_type) / sizeof(char);

		return std::move(

				DataType(std::type_index(typeid(element_type)),

						ele_size_in_byte,

						rank<obj_type>::value,

						&traits::seq_value<extents_t<obj_type> >::value[0],

						name)

		);


	}

	static DataType create(std::string const &name = "")
	{
		return create_(name, std::integral_constant<bool, has_static_member_function_datatype<T>::value>());
	}


};
//template<typename T>
//DataType create_opaque_datatype(std::string const & name = "")

}// namespace traits

}
// namespace simpla

#endif /* DATA_TYPE_H_ */