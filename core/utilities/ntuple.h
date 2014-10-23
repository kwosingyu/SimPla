/*Copyright (C) 2007-2011 YU Zhi. All rights reserved.
 * $Id: nTuple.h 990 2010-12-14 11:06:21Z salmon $
 * nTuple.h
 *
 *  created on: Jan 27, 2010
 *      Author: yuzhi
 */

#ifndef INCLUDEnTuple_H_
#define INCLUDEnTuple_H_

#include <stddef.h>
#include <ostream>

#include "expression_template.h"
#include "primitives.h"
#include "sp_integer_sequence.h"
#include "sp_type_traits.h"

namespace simpla
{
/**
 * \brief nTuple :n-tuple
 *
 *   Semantics:
 *    n-tuple is a sequence (or ordered list) of n elements, where n is a positive
 *      size_t   eger. There is also one 0-tuple, an empty sequence. An n-tuple is defined
 *    inductively using the construction of an ordered pair. Tuples are usually
 *    written by listing the elements within parenthesis '( )' and separated by
 *    commas; for example, (2, 7, 4, 1, 7) denotes a 5-tuple.
 *    \note  http://en.wikipedia.org/wiki/Tuple
 *   Implement
 *   template< size_t     n,typename T> struct nTuple;
 *   nTuple<double,5> t={1,2,3,4,5};
 *
 *   nTuple<T,N...> primary ntuple
 *   nTuple<Expression<TOP,TExpr>> unary nTuple expression
 *   nTuple<Expression<TOP,TExpr1,TExpr2>> binary nTuple expression
 *
 **/

/// n-dimensional primary type
template<typename, size_t... > struct nTuple;

template<typename TV> struct nTuple<TV>
{
	typedef TV value_type;

	typedef void sub_type;

	typedef integer_sequence<size_t> dimensions;

	typedef value_type pod_type;

	static constexpr size_t ndims = 0;

};

template<typename TV, size_t N, size_t ...M>
struct nTuple<TV, N, M...>
{

	typedef TV value_type;

	typedef typename std::conditional<(sizeof...(M) == 0), value_type,
			nTuple<value_type, M...>>::type sub_type;

	typedef integer_sequence<size_t, N, M...> dimensions;

	typedef typename std::conditional<(sizeof...(M) == 0), value_type,
			typename nTuple<value_type, M...>::pod_type>::type pod_type[N];

	static constexpr size_t ndims = 1 + sizeof...(M);

	static constexpr size_t dims = N;

	typedef nTuple<value_type, N, M...> this_type;

	sub_type data_[dims];

	sub_type & operator[](size_t s)
	{
		return data_[s];
	}

	sub_type const & operator[](size_t s) const
	{
		return data_[s];
	}

//	template<typename TI>
//	auto operator[](TI const & s)
//	DECL_RET_TYPE((get_value(data_,s)))
//
//	template<typename TI>
//	auto operator[](TI const & s) const
//	DECL_RET_TYPE((get_value(data_,s)))
//
//	template<size_t K>
//	sub_type const & operator[](integer_sequence<size_t, K>)
//	{
//		return data_[K];
//	}
//
//	template<size_t K>
//	sub_type const & operator[](integer_sequence<size_t, K>) const
//	{
//		return data_[K];
//	}
//
//	template<size_t K, size_t ...L>
//	auto operator[](integer_sequence<size_t, K, L...>)
//	DECL_RET_TYPE((data_[K][integer_sequence<size_t, L...>()]))
//
//	template<size_t K, size_t ...L>
//	auto operator[](integer_sequence<size_t, K, L...>) const
//	DECL_RET_TYPE((data_[K][integer_sequence<size_t, L...>()]))
//
//	template<typename TIdx>
//	auto operator[](TIdx const * idx)
//	DECL_RET_TYPE((get_value(data_,idx)))
//
//	template<typename TIdx>
//	auto operator[](TIdx const * idx) const
//	DECL_RET_TYPE((get_value(data_,idx)))

	template<typename TR> inline this_type &
	operator =(TR const &rhs)
	{
		_seq_for<dims>::eval(_impl::_assign(), data_, rhs);
		return (*this);
	}

	template<typename TR>
	inline this_type & operator +=(TR const &rhs)
	{
		_seq_for<dims>::eval(_impl::plus_assign(), data_, rhs);
		return (*this);
	}

	template<typename TR>
	inline this_type & operator -=(TR const &rhs)
	{
		_seq_for<dims>::eval(_impl::minus_assign(), data_, rhs);
		return (*this);
	}

	template<typename TR>
	inline this_type & operator *=(TR const &rhs)
	{
		_seq_for<dims>::eval(_impl::multiplies_assign(), data_, rhs);
		return (*this);
	}

	template<typename TR>
	inline this_type & operator /=(TR const &rhs)
	{
		_seq_for<dims>::eval(_impl::divides_assign(), data_, rhs);
		return (*this);
	}

//	template<size_t NR, typename TR>
//	void operator*(nTuple<NR, TR> const & rhs) = delete;
//
//	template<size_t NR, typename TR>
//	void operator/(nTuple<NR, TR> const & rhs) = delete;

};

template<typename ...> struct make_pod_array;

template<typename TV, typename TI, TI ... N>
struct make_pod_array<TV, integer_sequence<TI, N...>>
{
	typedef typename nTuple<TV, N...>::pod_type type;
};

template<typename ...>class Expression;

template<typename > struct nTuple_traits;

template<typename ... T>
struct nTuple<Expression<T...>> : public Expression<T...>
{
	typedef nTuple<Expression<T...>> this_type;
	typedef typename nTuple_traits<this_type>::value_type value_type;
	typedef typename nTuple_traits<this_type>::dimensions dimensions;

	typedef typename make_pod_array<value_type, dimensions>::type pod_type;

	operator bool() const
	{
		return seq_reduce(dimensions(), _impl::logical_and(), *this);
	}

	using Expression<T...>::Expression;

};

namespace _impl
{

template<typename T> struct reference_traits;

template<typename T, size_t M, size_t ...N>
struct reference_traits<nTuple<T, M, N...>>
{
	typedef nTuple<T, M, N...> const& type;
};

template<typename ...T>
struct reference_traits<nTuple<Expression<T...> >>
{
	typedef nTuple<Expression<T...> > type;
};

}  // namespace _impl

template<typename TV>
struct nTuple_traits
{
	typedef integer_sequence<size_t> dimensions;

	static constexpr size_t ndims = 0;

	typedef TV value_type;

};

template<typename TV, size_t ...N>
struct nTuple_traits<nTuple<TV, N...> >
{

	typedef typename nTuple_traits<TV>::value_type value_type;

	typedef typename cat_integer_sequence<integer_sequence<size_t, N...>,
			typename nTuple_traits<TV>::dimensions>::type dimensions;

	typedef typename make_pod_array<value_type, dimensions>::type pod_type;

	static constexpr size_t ndims = dimensions::size();

};

template<typename TOP, typename TL>
struct nTuple_traits<nTuple<Expression<TOP, TL> > >
{
private:
	typedef typename nTuple_traits<TL>::dimensions d_seq_l;
	typedef typename nTuple_traits<TL>::value_type value_type_l;
public:
	typedef d_seq_l dimensions;

	typedef decltype(std::declval<TOP>()(std::declval<value_type_l>())) value_type;

	typedef typename make_pod_array<value_type, dimensions>::type pod_type;

};
template<typename TOP, typename TL, typename TR>
struct nTuple_traits<nTuple<Expression<TOP, TL, TR>> >
{
private:
	typedef typename nTuple_traits<TL>::dimensions d_seq_l;
	typedef typename nTuple_traits<TR>::dimensions d_seq_r;
	typedef typename nTuple_traits<TL>::value_type value_type_l;
	typedef typename nTuple_traits<TR>::value_type value_type_r;
public:
	typedef d_seq_l dimensions;

	typedef decltype(std::declval<TOP>()(std::declval<value_type_l>(),
					std::declval<value_type_r>())) value_type;

	typedef typename make_pod_array<value_type, dimensions>::type pod_type;

};

template<typename T, size_t ...N>
struct rank<nTuple<T, N...>>
{
	static constexpr size_t value =
			nTuple_traits<nTuple<T, N...>>::dimensions::size();
};

template<typename TInts, TInts ...N>
nTuple<TInts, sizeof...(N)> seq2ntuple(integer_sequence<TInts, N...>)
{
	return std::move(nTuple<TInts, sizeof...(N)>(
	{ N... }));
}

template<typename T, size_t M, size_t ...N>
std::ostream &operator<<(std::ostream & os, nTuple<T, M, N...> const & v)
{
	os << std::endl << "{";
	for (int i = 0; i < M - 1; ++i)
	{
		os << v[i] << " , ";
	}

	os << v[M - 1] << "}";

	return os;
}

typedef nTuple<Real, 3> Vec3;

typedef nTuple<Real, 3> IVec3;

typedef nTuple<Integral, 3> RVec3;

typedef nTuple<Complex, 3> CVec3;

template<typename T, size_t N> using Vector=nTuple<T,N>;

template<typename T, size_t M, size_t N> using Matrix=nTuple<T,M,N >;

template<typename T, size_t ... N> using Tensor=nTuple<T,N...>;

template<typename T, size_t N, size_t ... M>
void swap(nTuple<T, N, M...> & l, nTuple<T, N, M...> & r)
{
	_seq_for<N>::eval(_impl::_swap(), (l), (r));
}

template<typename T, size_t N, size_t ... M>
void swap(nTuple<T, N, M...> & l,
		typename nTuple_traits<nTuple<T, N, M...>>::pod_type & r)
{
	_seq_for<N>::eval(_impl::_swap(), (l), (r));
}

template<typename TR, typename T, size_t ... N>
void assign(nTuple<T, N...> & l, TR const& r)
{
	_seq_for<N...>::eval(_impl::_assign(), l, r);
}

template<typename TR, typename T, size_t ... N>
auto inner_product(nTuple<T, N...> const & l, TR const& r)
DECL_RET_TYPE(( _seq_reduce<N... >::eval ( _impl::plus(),l*r) ))

template<typename T> inline auto determinant(
		Matrix<T, 3, 3> const & m)
				DECL_RET_TYPE(( m[0][0] * m[1][1] * m[2][2] - m[0][2] * m[1][1] * m[2][0] + m[0][1] //
								* m[1][2] * m[2][0] - m[0][1] * m[1][0] * m[2][2] + m[1][0] * m[2][1]//
								* m[0][2] - m[1][2] * m[2][1] * m[0][0]//
						)

				)

template<typename T> inline auto determinant(
		Matrix<T, 4, 4> const & m)
		DECL_RET_TYPE((//
				m[0][3] * m[1][2] * m[2][1] * m[3][0] - m[0][2] * m[1][3] * m[2][1] * m[3][0]//
				- m[0][3] * m[1][1] * m[2][2] * m[3][0] + m[0][1] * m[1][3]//
				* m[2][2] * m[3][0] + m[0][2] * m[1][1] * m[2][3] * m[3][0] - m[0][1]//
				* m[1][2] * m[2][3] * m[3][0] - m[0][3] * m[1][2] * m[2][0] * m[3][1]//
				+ m[0][2] * m[1][3] * m[2][0] * m[3][1] + m[0][3] * m[1][0] * m[2][2]//
				* m[3][1] - m[0][0] * m[1][3] * m[2][2] * m[3][1] - m[0][2] * m[1][0]//
				* m[2][3] * m[3][1] + m[0][0] * m[1][2] * m[2][3] * m[3][1] + m[0][3]//
				* m[1][1] * m[2][0] * m[3][2] - m[0][1] * m[1][3] * m[2][0] * m[3][2]//
				- m[0][3] * m[1][0] * m[2][1] * m[3][2] + m[0][0] * m[1][3] * m[2][1]//
				* m[3][2] + m[0][1] * m[1][0] * m[2][3] * m[3][2] - m[0][0] * m[1][1]//
				* m[2][3] * m[3][2] - m[0][2] * m[1][1] * m[2][0] * m[3][3] + m[0][1]//
				* m[1][2] * m[2][0] * m[3][3] + m[0][2] * m[1][0] * m[2][1] * m[3][3]//
				- m[0][0] * m[1][2] * m[2][1] * m[3][3] - m[0][1] * m[1][0] * m[2][2]//
				* m[3][3] + m[0][0] * m[1][1] * m[2][2] * m[3][3]//
		))

template<typename T, size_t ...N> auto abs(nTuple<T, N...> const & m)
DECL_RET_TYPE((std::sqrt(std::abs(dot(m, m)))))

template<size_t N, typename T> inline auto NProduct(nTuple<T, N> const & v)
->decltype(v[0]*v[1])
{
	decltype(v[0]*v[1]) res;
	res = 1;
	for (size_t i = 0; i < N; ++i)
	{
		res *= v[i];
	}
	return res;

}
template<size_t N, typename T> inline auto NSum(nTuple<T, N> const & v)
->decltype(v[0]+v[1])
{
	decltype(v[0]+v[1]) res;
	res = 0;
	for (size_t i = 0; i < N; ++i)
	{
		res += v[i];
	}
	return res;
}

template<typename T1, size_t ... N, typename TR>
inline auto dot(nTuple<T1, N...> const &l, TR const &r)
DECL_RET_TYPE((inner_product(l,r)))

template<typename T1, size_t ... N1, typename T2, size_t ... N2> inline auto cross(
		nTuple<T1, N1...> const & l, nTuple<T2, N2...> const & r)
		->nTuple<decltype(get_value(l,0)*get_value(r,0)),3>
{
	nTuple<decltype(get_value(l,0)*get_value(r,0)), 3> res =
	{ l[1] * r[2] - l[2] * r[1], l[2] * get_value(r, 0)
			- get_value(l, 0) * r[2], get_value(l, 0) * r[1]
			- l[1] * get_value(r, 0) };
	return std::move(res);
}

#define _SP_DEFINE_nTuple_EXPR_BINARY_RIGHT_OPERATOR(_OP_, _NAME_)                                                  \
	template<typename T1,size_t ...N1,typename  T2> \
	nTuple<Expression<_impl::_NAME_,nTuple<T1,N1...>,T2>> \
	operator _OP_(nTuple<T1,N1...> const & l,T2 const &r)  \
	{return (nTuple<Expression<_impl::_NAME_,nTuple<T1,N1...>,T2>>(l,r)) ;}                 \


#define _SP_DEFINE_nTuple_EXPR_BINARY_OPERATOR(_OP_,_NAME_)                                                  \
	template<typename T1,size_t ...N1,typename  T2> \
	nTuple<Expression<_impl::_NAME_,nTuple<T1,N1...>,T2>> \
	operator _OP_(nTuple<T1, N1...> const & l,T2 const &r)  \
	{return (nTuple<Expression<_impl::_NAME_,nTuple<T1,N1...>,T2>>(l,r));}                    \
	\
	template< typename T1,typename T2 ,size_t ...N2> \
	nTuple<Expression< _impl::_NAME_,T1,nTuple< T2,N2...>>> \
	operator _OP_(T1 const & l, nTuple< T2,N2...>const &r)                    \
	{return (nTuple<Expression< _impl::_NAME_,T1,nTuple< T2,N2...>>>(l,r))  ;}                \
	\
	template< typename T1,size_t ... N1,typename T2 ,size_t ...N2>  \
	nTuple<Expression< _impl::_NAME_,nTuple< T1,N1...>,nTuple< T2,N2...>>>\
	operator _OP_(nTuple< T1,N1...> const & l,nTuple< T2,N2...>  const &r)                    \
	{return (nTuple<Expression< _impl::_NAME_,nTuple< T1,N1...>,nTuple< T2,N2...>>>(l,r));}                    \


#define _SP_DEFINE_nTuple_EXPR_UNARY_OPERATOR(_OP_,_NAME_)                           \
		template<typename T,size_t ...N> \
		nTuple<Expression<_impl::_NAME_,nTuple<T,N...> >> \
		operator _OP_(nTuple<T,N...> const &l)  \
		{return (nTuple<Expression<_impl::_NAME_,nTuple<T,N...> >>(l)) ;}    \

#define _SP_DEFINE_nTuple_EXPR_BINARY_FUNCTION(_NAME_)                                                  \
			template<typename T1,size_t ...N1,typename  T2> \
			nTuple<Expression<_impl::_##_NAME_,nTuple<T1,N1...>,T2>> \
			_NAME_(nTuple<T1,N1...> const & l,T2 const &r)  \
			{return (nTuple<Expression<_impl::_##_NAME_,nTuple<T1,N1...>,T2>>(l,r));}       \
			\
			template< typename T1,typename T2,size_t ...N2> \
			nTuple<Expression< _impl::_##_NAME_,T1,nTuple< T2,N2...>>>\
			_NAME_(T1 const & l, nTuple< T2,N2...>const &r)                    \
			{return (nTuple<Expression< _impl::_##_NAME_,T1,nTuple< T2,N2...>>>(l,r)) ;}       \
			\
			template< typename T1,size_t ... N1,typename T2,size_t  ...N2> \
			nTuple<Expression< _impl::_##_NAME_,nTuple< T1,N1...>,nTuple< T2,N2...>>>\
			_NAME_(nTuple< T1,N1...> const & l,nTuple< T2,N2...>  const &r)                    \
			{return (nTuple<Expression< _impl::_##_NAME_,nTuple< T1,N1...>,nTuple< T2,N2...>>>(l,r))  ;}   \


#define _SP_DEFINE_nTuple_EXPR_UNARY_FUNCTION( _NAME_)                           \
		template<typename T,size_t ...N> \
		nTuple<Expression<_impl::_##_NAME_,nTuple<T,N...>>> \
		_NAME_(nTuple<T,N ...> const &r)  \
		{return (nTuple<Expression<_impl::_##_NAME_,nTuple<T,N...>>>(r));}     \

DEFINE_EXPRESSOPM_TEMPLATE_BASIC_ALGEBRA2(nTuple)
}
//namespace simpla

#endif  // INCLUDEnTuple_H_
