/**
 * @file use_case.h
 *
 *  Created on: 2014-11-21
 *      Author: salmon
 */

#ifndef CORE_APPLICATION_USE_CASE_H_
#define CORE_APPLICATION_USE_CASE_H_

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>

#include "../gtl/utilities/utilities.h"
#include "../gtl/design_pattern/SingletonHolder.h"
#include "../gtl/utilities/ConfigParser.h"


namespace simpla { namespace use_case
{
/** @ingroup task_flow
 * @{
 *
 *  */
//
class UseCase
{
    std::string info;
public:

    UseCase()
    {
    }

    virtual ~UseCase()
    {
    }


    virtual std::string const &description() = 0;

    virtual void setup(int argc, char **argv) = 0;

    virtual void accept_signal() { };

    virtual void teardown() { };

    virtual void check_point() { }

    virtual void dump() { }


    virtual void body() = 0;

    virtual void next_time_step(Real dt) { }

    void run();
};


class UseCaseList : public std::map<std::string, std::shared_ptr<UseCase>>
{
    typedef std::map<std::string, std::shared_ptr<UseCase>> base_type;

    std::string add(std::string const &name,
                    std::shared_ptr<UseCase> const &p);

public:
    template<typename T>
    std::string add(std::string const &name, std::shared_ptr<T> const &p)
    {
        base_type::operator[](name) = std::dynamic_pointer_cast<UseCase>(p);
        return "UseCase" + traits::type_cast<std::string>(list_.size()) + "_" + name;
    }

    template<typename T>
    std::string add(std::string const &name)
    {
        return add(name, std::make_shared<T>());
    }

    std::ostream &print(std::ostream &os);

    void run();
};
template<typename T, typename ...Args>
std::string register_app(std::string const &name, Args &&...args)
{
    SingletonHolder<UseCaseList>::instance()[name] = std::dynamic_pointer_cast<
            UseCase>(std::make_shared<T>(std::forward<Args>(args)...));

    return name;
}

#define USE_CASE(_app_name, _app_desc) \
struct _app_name:public UseCase  \
{ \
    static const std::string info; \
    typedef _app_name  this_type; \
    _app_name () {}\
    _app_name(this_type const &)=delete; \
    virtual ~_app_name  () {}\
    std::string const & description(){return info;} \
private:\
    void  body();\
};\
const std::string   _app_name::info =  register_app<_app_name>(( #_app_name))+_app_desc ; \
void _app_name::body()



/** @} */
}  // namespace use_case
}  // namespace simpla

#endif /* CORE_APPLICATION_USE_CASE_H_ */
