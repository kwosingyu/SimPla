/*
 * spIO.cpp
 *
 *  Created on: 2016年6月18日
 *      Author: salmon
 */

#include <memory>
#include "../src/io/IO.h"
#include "../src/parallel/DistributedObject.h"

#include "../src/data_model/DataSet.h"
#include "../src/data_model/DataType.h"
#include "../src/data_model/DataSpace.h"
#include "sp_cwrap.h"

//#include "spSimPlaWrap.h"
//#include "sp_def.h"

using namespace simpla;

struct spDataType_s;
typedef struct spDataType_s spDataType;

void spDataTypeCreate(spDataType **) { }

void spDataTypeDestroy(spDataType **) { }

int spDataTypeIsValid(spDataType const *) { return true; }

void spDataTypeExtent(spDataType *, int rank, size_t const *d) { }

void spDataTypePushBack(spDataType *, spDataType const *, char const name[]) { }

struct spDataSpace_s;
typedef struct spDataSpace_s spDataSpace;

void spDataSpaceCreateSimple(spDataSpace **, int ndims, size_t const *dims) { }

void spDataSpaceCreateUnordered(spDataSpace **, size_t num) { }

void spDataSpaceDestroy(spDataSpace **) { }

void spDataSpaceSelectHyperslab(spDataSpace *, ptrdiff_t const *offset, size_t const *count) { }

struct spDataSet_s;
typedef struct spDataSet_s spDataSet;

void spDataSetCreate(spDataSet **, void *d, spDataType const *dtype, spDataSpace const *mspace,
                     spDataSpace const *fspace) { }

void spDataSetDestroy(spDataSet *) { }


struct spIOStream_s
{
    std::shared_ptr<io::IOStream> m_stream_;
};
typedef struct spIOStream_s spIOStream;

void spIOStreamCreate(spIOStream **os)
{
    *os = new spIOStream;
    (*os)->m_stream_ = std::make_shared<io::HDF5Stream>();

}

void spIOStreamDestroy(spIOStream **os)
{
    if (*os != nullptr) { delete *os; };
    *os = nullptr;
};

void spIOStreamPWD(spIOStream *os, char url[])
{
    strcpy(url, os->m_stream_->pwd().c_str());
};

void spIOStreamOpen(spIOStream *os, const char url[])
{
    os->m_stream_->open(url);
}

void spIOStreamClose(spIOStream *os) { os->m_stream_->close(); }

void spIOStreamWrite(spIOStream *, char const name[], spDataSet const *) { }

void spIOStreamRead(spIOStream *, char const name[], spDataSet const *) { }

void spIOStreamWriteSimple(spIOStream *os, const char *url, int d_type,//
                           void *d, int ndims, size_t const *dims, size_t const *start, size_t const *count,
                           int flag)
{

    simpla::data_model::DataSet dset;
    dset.data_space = data_model::DataSpace(ndims, &count[0]);
    dset.memory_space = data_model::DataSpace(ndims, &dims[0]);
    dset.memory_space.select_hyperslab(&start[0], nullptr, &count[0], nullptr);
    switch (d_type)
    {
        case SP_TYPE_float:
            dset.data_type = data_model::DataType::create<float>();
            break;
        case SP_TYPE_double:
            dset.data_type = data_model::DataType::create<double>();
            break;

        case SP_TYPE_int:
            dset.data_type = data_model::DataType::create<int>();
            break;

        case SP_TYPE_long:
            dset.data_type = data_model::DataType::create<long>();
            break;
        case SP_TYPE_int64_t:
            dset.data_type = data_model::DataType::create<int64_t>();
            break;
        default:
            break;
    }

    dset.data = std::shared_ptr<void>(d, tags::do_nothing());

    INFORM << os->m_stream_->write(url, dset, flag) << std::endl;
}


struct spDistributedObject_s;
typedef struct spDistributedObject_s spDistributedObject;


struct spDistributedObject_s
{
    simpla::parallel::DistributedObject self;
};

void spDistributeObjectCreate(struct spDistributedObject_s **obj)
{
    *obj = new struct spDistributedObject_s;

}

void spDistributeObjectDestroy(struct spDistributedObject_s **obj)
{
    delete *obj;
    *obj = 0x0;
}

void spDistributeObjectNonblockingSync(struct spDistributedObject_s *obj)
{
    obj->self.sync();
}

void spDistributeObjectWait(struct spDistributedObject_s *obj)
{
    obj->self.wait();
}

void hdf5_write_field(const char *url, void *d, int ndims, size_type const *dims, const size_type *offset,
                      size_type const *count, int flag)
{
    simpla::data_model::DataSet dset;
    data_model::DataSpace d_space(ndims, &count[0]);
    data_model::DataSpace m_space(ndims, &dims[0]);
    m_space.select_hyperslab(&offset[0], nullptr, &count[0], nullptr);
    dset.memory_space = m_space;
    dset.data_space = d_space;
    dset.data_type = data_model::DataType::create<Real>();
    dset.data = std::shared_ptr<void>(reinterpret_cast<void *>(d), tags::do_nothing());

//	simpla::io::write(url, dset, flag);

}


void spDistributedObjectAddSendLink(spDistributedObject *, size_t id, const ptrdiff_t offset[3], const spDataSet *) { }

void spDistributedObjectAddRecvLink(spDistributedObject *, size_t id, const ptrdiff_t offset[3], spDataSet *) { }

int spDistributedObjectIsReady(spDistributedObject const *) { return true; }