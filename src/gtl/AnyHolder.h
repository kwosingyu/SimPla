//
// Created by salmon on 16-5-21.
//

#ifndef SIMPLA_ANYHOLDER_H
#define SIMPLA_ANYHOLDER_H

#include <memory>

namespace simpla { namespace gtl
{
class AnyHolder
{
public:
    template<typename U>
    std::shared_ptr<U> as()
    {

    }

    template<typename U>
    std::shared_ptr<const U> as() const
    {

    }
};
}}//namespace simpla { namespace gtl

#endif //SIMPLA_ANYHOLDER_H
