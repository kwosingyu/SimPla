/**
 * @file parallel_tbb.h
 *
 *  Created on: 2014-9-4
 *      Author: salmon
 */

#ifndef PARALLEL_TBB_H_
#define PARALLEL_TBB_H_

//#define TBB_IMPLEMENT_CPP0X true

#include <tbb/tbb.h>

namespace simpla { namespace parallel
{
using tbb::parallel_for;
using tbb::parallel_do;
using tbb::parallel_reduce;
using tbb::concurrent_unordered_map;
using tbb::concurrent_unordered_set;
using tbb::concurrent_hash_map;
using tbb::blocked_range;
//using tbb::blocked_range2d;
//using tbb::blocked_range3d;

namespace tags
{
using tbb::split;
using tbb::proportional_split;
}

}}  // namespace simpla { namespace parallel


#endif /* PARALLEL_TBB_H_ */
