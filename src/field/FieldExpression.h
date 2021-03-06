/**
 * @file field_expression.h
 *
 *  Created on: 2015-1-30
 *      Author: salmon
 */

#ifndef COREFieldField_EXPRESSION_H_
#define COREFieldField_EXPRESSION_H_

#include <stddef.h>
#include <cstdbool>
#include <type_traits>

#include "../gtl/ExpressionTemplate.h"
#include "../gtl/type_traits.h"

namespace simpla
{
/** @addtogroup field
 *  @{
 */
template<typename ...> struct Field;

/// @name  Field Expression
/// @{

template<typename ...> class Expression;

template<typename ...> class BooleanExpression;
namespace traits
{

template<typename> struct field_value_type;
template<typename TOP, typename ...T>
struct field_value_type<Field<simpla::BooleanExpression<TOP, T...> > > { typedef bool type; };
}//namespace  traits

template<typename ...T>
struct Field<Expression<T...> > : public Expression<T...>
{
    typedef Field<Expression<T...> > this_type;
    using Expression<T...>::args;
    using Expression<T...>::m_op_;
    using Expression<T...>::Expression;
};

template<typename ...T>
struct Field<BooleanExpression<T...> > : Expression<T...>
{
    using Expression<T...>::args;
    using Expression<T...>::m_op_;
    using Expression<T...>::Expression;
};


template<typename ...> struct AssignmentExpression;


template<typename TOP, typename TL, typename TR>
struct Field<AssignmentExpression<TOP, TL, TR> > : public AssignmentExpression<TOP, TL, TR>
{
    typedef AssignmentExpression<TOP, TL, TR> expression_type;

    typedef traits::value_type_t<TL> value_type;

    typedef Field<AssignmentExpression<TOP, TL, TR>> this_type;

    using AssignmentExpression<TOP, TL, TR>::AssignmentExpression;

    bool is_excuted_ = false;

    void excute()
    {
        if (!is_excuted_)
        {
//            expression_type::lhs = expression_type::rhs;
//            is_excuted_ = true;
        }

    }

    void do_not_excute()
    {
        is_excuted_ = true;
    }

    ~Field()
    {
        excute();
    }
};

DEFINE_EXPRESSOPM_TEMPLATE_BASIC_ALGEBRA(Field)

#define SP_DEF_BINOPField_NTUPLE(_OP_, _NAME_)                                                 \
template<typename ...T1, typename T2, size_t ... N>                                            \
Field<Expression<_impl::_NAME_, Field<T1...>, nTuple<T2, N...> > > operator _OP_(              \
        Field<T1...> const & l, nTuple<T2, N...> const &r)                                    \
{return (Field<Expression<_impl::_NAME_, Field<T1...>, nTuple<T2, N...> > >(l, r));}         \
template<typename T1, size_t ... N, typename ...T2>                                            \
Field<Expression<_impl::_NAME_, nTuple<T1, N...>, Field<T2...> > > operator _OP_(              \
        nTuple<T1, N...> const & l, Field< T2...>const &r)                                    \
{    return (Field<Expression< _impl::_NAME_,T1,Field< T2...>>>(l,r));}                       \


SP_DEF_BINOPField_NTUPLE(+, plus)

SP_DEF_BINOPField_NTUPLE(-, minus)

SP_DEF_BINOPField_NTUPLE(*, multiplies)

SP_DEF_BINOPField_NTUPLE(/, divides)

SP_DEF_BINOPField_NTUPLE(%, modulus)

SP_DEF_BINOPField_NTUPLE(^, bitwise_xor)

SP_DEF_BINOPField_NTUPLE(&, bitwise_and)

SP_DEF_BINOPField_NTUPLE(|, bitwise_or)

#undef SP_DEF_BINOPField_NTUPLE

/** @} */
}  // namespace simpla

#endif /* COREFieldField_EXPRESSION_H_ */
