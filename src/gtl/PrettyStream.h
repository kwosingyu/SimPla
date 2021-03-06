/**
 * @file  pretty_stream.h
 *
 *  created on: 2013-11-29
 *      Author: salmon
 */

#ifndef PRETTY_STREAM_H_
#define PRETTY_STREAM_H_

#include <stddef.h>
#include <complex>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "type_traits.h"


namespace simpla
{

/**
 * @ingroup utilities
 * @addtogroup fancy_print Fancy print
 * @{
 */

template<typename TV, typename TI> inline TV const *
printNdArray(std::ostream &os, TV const *v, int rank, TI const *d,
             std::string const &left_brace = "{", std::string const &sep = ",",
             std::string const &right_brace = "}")
{
    constexpr int ELE_NUM_PER_LINE = 10;
    if (rank == 1)
    {
        os << left_brace << *v;
        ++v;
        for (int s = 1; s < d[0]; ++s)
        {
            os << sep << "\t";
            if (s % ELE_NUM_PER_LINE == 0 && s != 0)
            {
                os << std::endl;
            }
            os << (*v);
            ++v;
        }
        os << right_brace << std::endl;
        return v;
    }
    else
    {

        os << left_brace;
        v = printNdArray(os, v, rank - 1, d + 1, left_brace, sep, right_brace);

        for (int s = 1; s < d[0]; ++s)
        {
            os << sep << std::endl;
            v = printNdArray(os, v, rank - 1, d + 1, left_brace, sep,
                             right_brace);
        }
        os << right_brace << std::endl;
        return (v);
    }
}


template<typename TX, typename TY, typename ...Others> std::istream &
get_(std::istream &is, size_t num, std::map<TX, TY, Others...> &a)
{

    for (size_t s = 0; s < num; ++s)
    {
        TX x;
        TY y;
        is >> x >> y;
        a.emplace(x, y);
    }
    return is;
}

template<typename TI>
std::ostream &ContainerOutPut1(std::ostream &os, TI const ib, TI const ie)
{
    if (ib == ie)
    {
        return os;
    }

    TI it = ib;

    os << *it;

    size_t s = 0;

    for (++it; it != ie; ++it)
    {
        os << " , " << *it;

        ++s;
        if (s % 10 == 0)
        {
            os << std::endl;
        }
    }

    return os;
}

template<typename TI>
std::ostream &ContainerOutPut2(std::ostream &os, TI const &ib, TI const &ie)
{
    if (ib == ie)
    {
        return os;
    }

    TI it = ib;

    os << it->first << "=" << it->second;

    ++it;

    for (; it != ie; ++it)
    {
        os << " , " << it->first << " = " << it->second << "\n";
    }
    return os;
}

template<typename TI, typename TFUN>
std::ostream &ContainerOutPut3(std::ostream &os, TI const &ib, TI const &ie,
                               TFUN const &fun)
{
    if (ib == ie)
    {
        return os;
    }

    TI it = ib;

    fun(os, it);

    ++it;

    for (; it != ie; ++it)
    {
        os << " , ";
        fun(os, it);
    }
    return os;
}
} // namespace simpla

namespace std
{

template<typename T> std::ostream &
operator<<(std::ostream &os, const std::complex<T> &tv)
{
    os << "{" << tv.real() << "," << tv.imag() << "}";
    return (os);
}

template<typename T1, typename T2> std::ostream &
operator<<(std::ostream &os, std::pair<T1, T2> const &p)
{
    os << p.first << " = { " << p.second << " }";
    return (os);
}

template<typename T1, typename T2> std::ostream &
operator<<(std::ostream &os, std::map<T1, T2> const &p)
{
    for (auto const &v : p)
        os << v << "," << std::endl;
    return (os);
}

template<typename TV, typename ...Others> std::istream &
operator>>(std::istream &is, std::vector<TV, Others...> &a)
{
    for (auto &v : a)
    {
        is >> v;
    }
//	std::copy(std::istream_iterator<TV>(is), std::istream_iterator<TV>(), std::back_inserter(a));
    return is;

}

template<typename U, typename ...Others>
std::ostream &operator<<(std::ostream &os, std::vector<U, Others...> const &d)
{
    return simpla::ContainerOutPut1(os, d.begin(), d.end());
}

template<typename U, typename ...Others>
std::ostream &operator<<(std::ostream &os, std::list<U, Others...> const &d)
{
    return simpla::ContainerOutPut1(os, d.begin(), d.end());
}

template<typename U, typename ...Others>
std::ostream &operator<<(std::ostream &os, std::set<U, Others...> const &d)
{
    return simpla::ContainerOutPut1(os, d.begin(), d.end());
}

template<typename U, typename ...Others>
std::ostream &operator<<(std::ostream &os,
                         std::multiset<U, Others...> const &d)
{
    return simpla::ContainerOutPut1(os, d.begin(), d.end());
}

template<typename TX, typename TY, typename ...Others> std::ostream &
operator<<(std::ostream &os, std::map<TX, TY, Others...> const &d)
{
    return simpla::ContainerOutPut2(os, d.begin(), d.end());
}

template<typename TX, typename TY, typename ...Others> std::ostream &
operator<<(std::ostream &os, std::multimap<TX, TY, Others...> const &d)
{
    return simpla::ContainerOutPut2(os, d.begin(), d.end());
}

namespace _impl
{
template<typename  ...Args>
std::ostream &print_helper(std::ostream &os, std::tuple<Args ...> const &v, std::integral_constant<int, 0>)
{
    return os;
};

template<typename  ...Args, int N>
std::ostream &print_helper(std::ostream &os, std::tuple<Args ...> const &v, std::integral_constant<int, N>)
{
    os << " , " << std::get<sizeof...(Args) - N>(v);
    print_helper(os, v, std::integral_constant<int, N - 1>());
    return os;
};
}


template<typename T, typename ...Args>
std::ostream &operator<<(std::ostream &os, std::tuple<T, Args ...> const &v)
{
    os << "{ " << std::get<0>(v);

    _impl::print_helper(os, v, std::integral_constant<int, sizeof...(Args)>());

    os << "}";

    return os;
};

namespace traits
{
template<typename T>
struct is_printable
{
private:
    HAS_CONST_MEMBER_FUNCTION(print);

public:

    static constexpr bool value = has_const_member_function_print<T, std::ostream &>::value ||
                                  has_const_member_function_print<T, std::ostream &, int>::value;

};

template<typename T> using is_printable_t=std::enable_if_t<is_printable<T>::value>;


}//namespace traits{


template<typename T, typename ...Others>
std::ostream &print(std::ostream &os, T const &first, Others &&... others)
{
    os << first << " , ";

    print(os, std::forward<Others>(others)...);

    return os;
};

template<typename T>
std::ostream &print(std::ostream &os, T const &v, traits::is_printable_t<T> *_p = nullptr)
{
    return v.print(os, 1);
}


/** @}*/
}  // namespace std

#endif /* PRETTY_STREAM_H_ */
