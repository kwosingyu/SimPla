/*
 * complex_ops.h
 *
 *  Created on: 2013年12月30日
 *      Author: salmon
 */

#ifndef COMPLEX_OPS_H_
#define COMPLEX_OPS_H_
#include <complex>
namespace simpla
{

#define DEF_COMPLEX_OP(_OP_, _OTHER_)                                                                   \
template<typename T> inline auto operator _OP_(std::complex<T> const & l, _OTHER_ const & r)                    \
->std::complex<decltype(l.real() _OP_ r)>                                                                    \
{return std::move(std::complex<decltype(l.real() _OP_ r)>(l.real() _OP_ r,l.imag() _OP_ r));}                 \
template<typename T> inline auto operator _OP_(_OTHER_ const & l,  std::complex<T> const & r)                   \
->std::complex<decltype(l _OP_ r.real())>                                                                    \
{return std::move(std::complex<decltype(l _OP_ r.real())>(l _OP_ r.real(),l _OP_ r.imag()));}

//#define DEF_COMPLEX_OP(_OP_, _OTHER_)                                                                   \
//template<typename T> inline auto operator _OP_(std::complex<T> const & l, _OTHER_ r)                    \
//->decltype(l _OP_ static_cast<double>(r))                                                                    \
//{return std::move(l _OP_ static_cast<double>(r));}                 \
//template<typename T> inline auto operator _OP_(_OTHER_ l,  std::complex<T> const & r)                   \
//->decltype(static_cast<double>(l) _OP_ r)                                                                        \
//{return std::move(static_cast<double>(l) _OP_ r);}


#define DEF_COMPLEX_OP_BUNDLE(_OTHER_)                                                                  \
DEF_COMPLEX_OP(+, _OTHER_)                                                                              \
DEF_COMPLEX_OP(-, _OTHER_)                                                                              \
DEF_COMPLEX_OP(*, _OTHER_)                                                                              \
DEF_COMPLEX_OP(/, _OTHER_)

DEF_COMPLEX_OP_BUNDLE(int)
DEF_COMPLEX_OP_BUNDLE(long)
DEF_COMPLEX_OP_BUNDLE(unsigned int)
DEF_COMPLEX_OP_BUNDLE(unsigned long)

#undef DEF_COMPLEX_OP_BUNDLE
#undef DEF_COMPLEX_OP
}
// namespace simpla

#endif /* COMPLEX_OPS_H_ */
