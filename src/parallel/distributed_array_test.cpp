/*
 * distributed_array_test.cpp
 *
 *  created on: 2014-5-30
 *      Author: salmon
 */

#include <gtest/gtest.h>
#include "distributed_array.h"
#include "../utilities/ntuple.h"
#include "../utilities/pretty_stream.h"
#include <stddef.h>
#include "update_ghosts.h"

using namespace simpla;

class TestDistArray: public testing::TestWithParam<nTuple<3, size_t> >
{

protected:
	virtual void SetUp()
	{

		global_begin = 0;
		global_end = global_begin + GetParam();
	}
public:
	nTuple<3, size_t> global_begin;
	nTuple<3, size_t> global_end;
	static constexpr   unsigned int   NDIMS = 3;
	DistributedArray<NDIMS> darray;
};

//TEST_P(TestDistArray, Init)
//{
//	darray.Init(3, 1, 2, global_start, global_count);
//
//	CHECK(global_start);
//	CHECK(global_count);
//	CHECK(darray.local_.outer_start);
//	CHECK(darray.local_.outer_count);
//	CHECK(darray.local_.inner_start);
//	CHECK(darray.local_.inner_count);
//
//}

TEST_P(TestDistArray, UpdateGhost)
{
	GLOBAL_COMM.Init();

	darray.global_begin_=global_begin;
	darray.global_end_=global_end;

	darray.Decompose(GLOBAL_COMM.get_size(), GLOBAL_COMM.get_rank(), 2);

	std::vector<double> data(darray.memory_size());

	std::fill(data.begin(), data.end(),GLOBAL_COMM.get_rank());
	size_t count =0;
	for(auto & v:data)
	{
		v=count+(GLOBAL_COMM.get_rank()+1)*1000;
		++count;
	}

	UpdateGhosts(&data[0],darray);

	MPI_Barrier( GLOBAL_COMM.comm());

	if(GLOBAL_COMM.get_rank()==0)
	{
		count =0;
		for(auto const & v:data)
		{
			if((count%(darray.local_.outer_end[1]-darray.local_.outer_begin[1]))==0)
			{
				std::cout<<std::endl<<"["<< GLOBAL_COMM.get_rank()<<"/"<<GLOBAL_COMM.get_size()<<"]";
			}

			std::cout<<v<<" ";

			++count;
		}
		std::cout<<std::endl;
	}
	MPI_Barrier( GLOBAL_COMM.comm());
}

INSTANTIATE_TEST_CASE_P(Parallel, TestDistArray, testing::Values(nTuple<3, size_t>( { 10, 20, 1 })));
