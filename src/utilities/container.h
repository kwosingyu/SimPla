/*
 * container.h
 *
 *  Created on: 2013年11月23日
 *      Author: salmon
 */

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <cstddef>
#include <list>
#include <memory>
#include <vector>

namespace simpla
{
template<typename T> struct Container
{
	typedef std::vector<T> type;

	static type Create(size_t num)
	{
		return std::move(type(num, T()));
	}
};

template<typename T> using FixedSizeSmallObjectAllocater = std::allocator<T>;

template<typename T> struct Container<std::list<T> >
{
	typedef std::list<T, FixedSizeSmallObjectAllocater<T> > ele_type;
	typedef std::vector<ele_type> type;

	typedef FixedSizeSmallObjectAllocater<T> allocator_type;
//	static FixedSizeSmallObjectAllocater<T> allocator_;

	static type Create(size_t num)
	{
		allocator_type alloc;
		return std::move(type(num, ele_type(alloc)));
	}
};

}  // namespace simpla

#endif /* CONTAINER_H_ */