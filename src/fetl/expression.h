/*
 * expression.h
 *
 *  Created on: 2013-7-20
 *      Author: salmon
 */

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <type_traits>
#include <utility>
#include <complex>
#include <cmath>
/** !file
 *     Define type-independent expression template
 *     arithmetic operator + - / *
 *	   TODO: relation operator
 *
 *
 * */

namespace simpla
{

struct NullType;

struct EmptyType
{
};

struct Zero
{
	Zero()
	{
	}
	template<typename TL> Zero(TL const &)
	{
	}
	template<typename TL, typename TR> Zero(TL const &, TR const &)
	{
	}
};
struct One
{
	One()
	{
	}
	template<typename TL> One(TL const &)
	{
	}

	template<typename TL, typename TR> One(TL const &, TR const &)
	{
	}

};

struct Infinity
{
	Infinity()
	{

	}
	template<typename TL> Infinity(TL const &)
	{
	}
	template<typename TL, typename TR> Infinity(TL const &, TR const &)
	{
	}

};

struct Undefine
{
	Undefine()
	{

	}
	template<typename TL> Undefine(TL const &)
	{
	}
	template<typename TL, typename TR> Undefine(TL const &, TR const &)
	{
	}

};

template<int N, typename TExpr> struct nTuple;

template<typename TG, typename TExpr> struct Field;

template<typename > struct PlaceHolder;

template<typename TOP, typename TL, typename TR> struct BiOp;

template<typename TOP, typename TL> struct UniOp;

template<typename T>
struct remove_const_reference
{
	typedef typename std::remove_const<typename std::remove_reference<T>::type>::type type;
};

// check is_Field
template<typename > struct is_Field
{
	static const bool value = false;
};
template<typename T> struct is_Field<T&>
{
	static const bool value = is_Field<T>::value;
};

template<typename T> struct is_Field<const T>
{
	static const bool value = is_Field<T>::value;
};

template<typename TG, typename TE> struct is_Field<Field<TG, TE> >
{
	static const bool value = true;
};

template<typename TOP, typename TL, typename TR> struct is_Field<
		BiOp<TOP, TL, TR> >
{
	static const bool value = is_Field<TL>::value || is_Field<TR>::value;
};

template<typename TOP, typename TExpr> struct is_Field<UniOp<TOP, TExpr> >
{
	static const bool value = is_Field<TExpr>::value;
};

// check is_nTuple
template<typename > struct is_nTuple
{
	static const bool value = false;
};

template<typename T> struct is_nTuple<T&>
{
	static const bool value = is_nTuple<T>::value;
};

template<typename T> struct is_nTuple<const T>
{
	static const bool value = is_nTuple<T>::value;
};

template<int N, typename T> struct is_nTuple<nTuple<N, T> >
{
	static const bool value = true;
};

template<typename TOP, typename TL, typename TR> struct is_nTuple<
		BiOp<TOP, TL, TR> >
{
	static const bool value = (is_nTuple<TL>::value || is_nTuple<TR>::value)
			&& !(is_Field<TL>::value || is_Field<TR>::value);
};

template<typename TOP, typename TExpr> struct is_nTuple<UniOp<TOP, TExpr> >
{
	static const bool value = is_nTuple<TExpr>::value;
};

template<typename > struct is_complex
{
	static const bool value = false;
};

template<typename T> struct is_complex<std::complex<T> >
{
	static const bool value = true;
};

template<typename TOP, typename TL, typename TR> struct is_complex<
		BiOp<TOP, TL, TR> >
{
	static const bool value = is_complex<TL>::value || is_complex<TR>::value;
};

template<typename TOP, typename TExpr> struct is_complex<UniOp<TOP, TExpr> >
{
	static const bool value = is_complex<TExpr>::value;
};

template<typename > struct has_PlaceHolder
{
	static const bool value = false;
};

template<typename TE> struct has_PlaceHolder<PlaceHolder<TE> >
{
	static const bool value = true;
};

template<typename TOP, typename TL, typename TR> struct has_PlaceHolder<
		BiOp<TOP, TL, TR> >
{
	static const bool value = has_PlaceHolder<TL>::value
			|| has_PlaceHolder<TR>::value;
};

template<typename TOP, typename TExpr> struct has_PlaceHolder<UniOp<TOP, TExpr> >
{
	static const bool value = has_PlaceHolder<TExpr>::value;
};

template<typename TE> struct is_indexable
{
	typedef typename remove_const_reference<TE>::type T;
	static const bool value = is_nTuple<T>::value || is_Field<T>::value
			|| std::is_pointer<T>::value;
};

template<typename TL>
struct is_arithmetic_scalar
{
	static const bool value = (std::is_arithmetic<TL>::value
			|| is_complex<TL>::value);
};

template<typename TL>
struct is_primitive
{
	static const bool value = (is_arithmetic_scalar<TL>::value
			|| is_nTuple<TL>::value || has_PlaceHolder<TL>::value);
};

template<typename TL>
struct ReferenceTraits
{
	typedef typename std::conditional<
			std::is_copy_constructible<TL>::value
					&& !(std::is_trivial<TL>::value && is_nTuple<TL>::value),
			TL, TL const &>::type type;

};
template<typename TL>
struct ConstReferenceTraits
{
	typedef typename std::add_const<typename ReferenceTraits<TL>::type>::type type;

};
template<typename I> inline
double index(double v, I)
{
	return (v);
}
template<typename I> inline std::complex<double> index(std::complex<double> v,
		I)
{
	return (v);
}
template<typename T, typename I> inline
auto index(T const & v, I const & s)->decltype(v[s])
{
	return (v[s]);
}

#define DECL_RET_TYPE(_EXPR_) ->decltype((_EXPR_)){return (_EXPR_);}
#define CONDITION_DECL_RET_TYPE(_COND_,_EXPR_)	->typename std::enable_if<_COND_,decltype((_EXPR_))>::type {return (_EXPR_);}

template<typename TL> inline auto //
operator -(TL const &lhs) DECL_RET_TYPE((-1.0*lhs))

template<typename TE> inline TE const &
operator +(TE const &e, Zero const &)
{
	return (e);
}

template<typename TE> inline TE const &
operator +(Zero const &, TE const &e)
{
	return (e);
}

template<typename TE> inline TE const &
operator -(TE const &e, Zero const &)
{
	return (e);
}

template<typename TE> inline auto operator -(Zero const &, TE const &e)
DECL_RET_TYPE((-e))

Zero operator +(Zero const &, Zero const &e)
{
	return (Zero());
}

template<typename TE> inline TE const &operator *(TE const &e, One const &)
{
	return (e);
}

template<typename TE> inline TE const & operator *(One const &, TE const &e)
{
	return (e);
}

template<typename TE> inline Zero operator *(TE const &, Zero const &)
{
	return (Zero());
}

template<typename TE> inline Zero operator *(Zero const &, TE const &)
{
	return (Zero());
}

template<typename TE> inline Infinity operator /(TE const &e, Zero const &)
{
	return (Infinity());
}

template<typename TE> inline Zero operator /(Zero const &, TE const &e)
{
	return (Zero());
}

template<typename TE> inline Zero operator /(TE const &, Infinity const &)
{
	return (Zero());
}

template<typename TE> inline Infinity operator /(Infinity const &, TE const &e)
{
	return (Infinity());
}

template<typename TOP, typename TL>
struct UniOp
{
public:

	typename ReferenceTraits<TL>::type expr;

	typedef UniOp<TOP, TL> ThisType;

	typedef decltype(TOP::eval(expr ,0 )) ValueType;

	UniOp(TL const & l) :
			expr(l)
	{
	}

	UniOp(ThisType const &) =default;

	inline ValueType
	operator[](size_t const & s)const
	{
		return ( TOP::eval(expr,s ));
	}

};

#define UNI_FUN_OP(_OP_NAME_,_FUN_)                                                    \
struct Op##_OP_NAME_                                                                 \
{template<typename TL> inline static auto                               \
eval(TL const & l, size_t s) ->decltype((std::sin(index(l, s))))     \
{	return (_FUN_(index(l, s)));} };                                                     \
template<typename TL> inline UniOp<Op##_OP_NAME_, TL> _OP_NAME_(TL const &lhs)         \
{	return (UniOp<Op##_OP_NAME_, TL>(lhs));}                                           \
inline double _OP_NAME_(double lhs){return (_FUN_(lhs)); }                         \

UNI_FUN_OP(abs, std::abs)
UNI_FUN_OP(sin, std::sin)
UNI_FUN_OP(cos, std::cos)
UNI_FUN_OP(tan, std::tan)
UNI_FUN_OP(log, std::log)
UNI_FUN_OP(log10, std::log10)
UNI_FUN_OP(exp, std::exp)

#undef UNI_FUN_OP

inline double abs(std::complex<double> lhs)
{
	return (std::abs(lhs));
}

template<typename TOP, typename TL, typename TR>
struct BiOp
{
public:

	typename ReferenceTraits<TL>::type l_;
	typename ReferenceTraits<TR>::type r_;

	typedef BiOp<TOP, TL, TR> ThisType;

	typedef decltype(TOP::eval(l_,r_,0)) ValueType;
	BiOp(TL const & l, TR const &r) :
			l_(l), r_(r)
	{
	}

	BiOp(ThisType const &) =default;

	inline ValueType operator[](size_t const & s)const
	{
		return ( TOP::eval(l_,r_,s));
	}

};
struct OpMultiplies
{
	template<typename TL, typename TR>
	static inline auto eval(TL const & l, TR const &r, size_t s)
	DECL_RET_TYPE((index(l,s)* index(r,s)))
};

struct OpDivides
{
	template<typename TL, typename TR>
	static inline auto eval(TL const & l, TR const &r, size_t s)
	DECL_RET_TYPE((index(l,s)/ index(r,s)))
};

struct OpPlus
{
	template<typename TL, typename TR>
	static inline auto eval(TL const & l, TR const &r, size_t s)
	DECL_RET_TYPE((index(l,s)+ index(r,s)))
};

struct OpMinus
{
	template<typename TL, typename TR>
	static inline auto eval(TL const & l, TR const &r, size_t s)
	DECL_RET_TYPE((index(l,s)- index(r,s)))
};

template<typename TL, typename TR> inline BiOp<OpPlus, TL, TR> operator +(
		TL const &lhs, TR const & rhs)
{
	return (BiOp<OpPlus, TL, TR>(lhs, rhs));
}
template<typename TL, typename TR> inline BiOp<OpMinus, TL, TR> operator -(
		TL const &lhs, TR const & rhs)
{
	return (BiOp<OpMinus, TL, TR>(lhs, rhs));
}
template<typename TL, typename TR> inline BiOp<OpMultiplies, TL, TR> operator *(
		TL const &lhs, TR const & rhs)
{
	return (BiOp<OpMultiplies, TL, TR>(lhs, rhs));
}
template<typename TL, typename TR> inline BiOp<OpDivides, TL, TR> operator /(
		TL const &lhs, TR const & rhs)
{
	return (BiOp<OpDivides, TL, TR>(lhs, rhs));
}

#define BI_FUN_OP(_OP_NAME_,_FUN_)                                               \
struct Op##_OP_NAME_                                                            \
{template<typename TL, typename TR> inline static auto eval(TL const &l,        \
		TR const & r,size_t s) DECL_RET_TYPE((_FUN_(index(l,s),index(r,s))))};                              \
template<typename TL, typename TR> inline BiOp<Op##_OP_NAME_, TL, TR>            \
_OP_NAME_(TL const &lhs, TR const & rhs)                                         \
{                                                                                \
	return (BiOp<Op##_OP_NAME_, TL, TR>(lhs, rhs));                              \
}                                                                                \

BI_FUN_OP(pow, std::pow)
BI_FUN_OP(modf, std::modf)
BI_FUN_OP(Dot, Dot)
BI_FUN_OP(Cross, Cross)
#undef BI_FUN_OP

}
// namespace simpla

#endif /* EXPRESSION_H_ */
