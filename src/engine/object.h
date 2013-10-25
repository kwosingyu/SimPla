/*
 * object.h
 *
 *  Created on: 2012-10-31
 *      Author: salmon
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include <bits/shared_ptr_base.h>
#include <bits/shared_ptr.h>
#include <boost/property_tree/ptree.hpp>
#include <fetl/primitives.h>
#include <utilities/log.h>
#include <utilities/properties.h>
#include <iostream>
#include <string>
#include <typeinfo>
#include <include/simpla_defs.h>
namespace simpla
{
class Object
{
public:

	Object() :
			data_(nullptr)
	{
	}

	template<typename T>
	Object(std::shared_ptr<T> d) :
			data_(std::dynamic_pointer_cast<ByteType>(d))
	{
	}

	template<typename T>
	Object(T* d) :
			data_(std::shared_ptr<ByteType>(d))
	{
	}
	Object(Object const &) = default;

	Object(Object && rhs) = default;

	virtual ~Object()
	{
	}

	template<typename T>
	T & as()
	{
		if (!CheckType(typeid(T)))
		{
			ERROR << "This is not a " << typeid(T).name() << " !";
		}
		return (*dynamic_cast<T>(data_));
	}
	template<typename T>
	T const & as() const
	{
		if (!CheckType(typeid(T)))
		{
			ERROR << "Can not convert to type " << typeid(T).name();
		}
		return (*dynamic_cast<const T>(data_));
	}

	virtual void swap(Object & rhs)
	{
		properties.swap(rhs.properties);

		data_.swap(rhs.data_);

	}

// Metadata ------------------------------------------------------------

	virtual bool CheckType(std::type_info const &) const=0;

	bool IsEmpty() const
	{

		return (data_.get() == nullptr);

	}

public:
	PTree properties;
protected:
	std::shared_ptr<ByteType> data_;

};
template<typename T>
class ObjectWrapper: public Object
{
public:
	typedef T value_type;

	ObjectWrapper(std::shared_ptr<T> ptr) :
			Object(ptr)
	{

	}
	virtual bool CheckType(std::type_info const &tinfo) const
	{
		return tinfo == typeid(T);
	}

};
}
// namespace simpla

#endif /* OBJECT_H_ */
