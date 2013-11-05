/*
 *  _fetl_impl::vector_calculus.h
 *
 *  Created on: 2012-3-1
 *      Author: salmon
 */

#ifndef VECTOR_CALCULUS_H_
#define VECTOR_CALCULUS_H_

#include <fetl/ntuple.h>
#include <fetl/primitives.h>
#include <type_traits>

namespace simpla
{

template<typename TM, int IL, int TOP, typename TL>
struct Field<Geometry<TM, IL>, UniOp<TOP, TL> >
{

	TM const & mesh;

	typename ConstReferenceTraits<TL>::type l_;

	Field(TL const & l) :
			mesh(l.mesh), l_(l)
	{
	}

	typedef decltype(
			_OpEval(Int2Type<TOP>(),
					std::declval<typename std::remove_reference<TL>::type const&>()
					,std::declval<typename TM::index_type>())

	) value_type;

	inline value_type operator[](typename TM::index_type s) const
	{
		return (_OpEval(Int2Type<TOP>(), l_, s));
	}
};

template<typename TM, int IFORM, int TOP, typename TL, typename TR>
struct Field<Geometry<TM, IFORM>, BiOp<TOP, TL, TR> >
{
	TM const & mesh;
	typename ConstReferenceTraits<TL>::type l_;
	typename ConstReferenceTraits<TR>::type r_;
	typedef Field<Geometry<TM, IFORM>, BiOp<TOP, TL, TR> > this_type;

	Field(TL const & l, TR const & r) :
			mesh(get_mesh(l, r)), l_(l), r_(r)
	{
	}

	typedef decltype(
			_OpEval(Int2Type<TOP>(),
					std::declval<typename std::remove_reference<TL>::type const&>(),
					std::declval<typename std::remove_reference<TR>::type const&>(),
					std::declval<typename TM::index_type>()
			)

	) value_type;

	inline value_type operator[](typename TM::index_type s) const
	{
		return (_OpEval(Int2Type<TOP>(), l_, r_, s));
	}
private:

	template<int IL, typename VL, typename VR> static inline TM const & get_mesh(
			Field<Geometry<TM, IL>, VL> const & l, VR const & r)
	{
		return (l.mesh);
	}
	template<typename VL, int IR, typename VR> static inline TM const & get_mesh(
			VL const & l, Field<Geometry<TM, IR>, VR> const & r)
	{
		return (r.mesh);
	}

	template<int IL, typename VL, int IR, typename VR> static inline TM const & get_mesh(
			Field<Geometry<TM, IL>, VL> const & l,
			Field<Geometry<TM, IR>, VR> const & r)
	{
		return (l.mesh);
	}

}
;

template<typename TG, typename TR> inline auto Grad(
		Field<Geometry<TG, 0>, TR> const & f)
				DECL_RET_TYPE(
						(Field<Geometry<TG, 1>, UniOp<GRAD,Field<Geometry<TG, 0>, TR> > >(f)))

template<typename TG, typename TR> inline auto Diverge(
		Field<Geometry<TG, 1>, TR> const & f)
		DECL_RET_TYPE((Field<Geometry<TG, 0>,
						UniOp<DIVERGE, Field<Geometry<TG, 1>, TR> > >( f)))

template<typename TG, typename TR> inline auto Curl(
		Field<Geometry<TG, 1>, TR> const & f)
		DECL_RET_TYPE( (Field<Geometry<TG, 2>,
						UniOp<CURL, Field<Geometry<TG, 1>, TR> > >(f)))
template<typename TG, typename TR> inline auto Curl(
		Field<Geometry<TG, 2>, TR> const & f)
		DECL_RET_TYPE( (Field<Geometry<TG, 1>,
						UniOp<CURL, Field<Geometry<TG, 2>, TR> > >(f)))

template<typename TG, int IL, typename TL> inline  //
auto operator*(Field<Geometry<TG, IL>, TL> const & f)
DECL_RET_TYPE(
		(typename std::conditional<(IL > 0 && IL <= TG::NUM_OF_DIMS),
				Field<Geometry<TG, TG::NUM_OF_DIMS - IL>,
				UniOp<HODGESTAR
				,Field<Geometry<TG, IL>, TL> > >, Zero>::type(f)))

template<typename TG, int IL, typename TL> inline  //
auto d(Field<Geometry<TG, IL>, TL> const & f)
DECL_RET_TYPE(
		(typename std::conditional<(IL > 0 && IL+1 <= TG::NUM_OF_DIMS),
				Field<Geometry<TG, IL+1>,
				UniOp<EXTRIORDERIVATIVE,Field<Geometry<TG, IL>, TL> > >
				, Zero>::type(f)) )

template<typename TG, int IL, int IR, typename TL, typename TR> inline auto //
operator^(Field<Geometry<TG, IL>, TL> const & lhs,
		Field<Geometry<TG, IR>, TR> const & rhs)
		DECL_RET_TYPE(
				(typename std::conditional<(IL + IR >=0 &&
								IL+IR <= TG::NUM_OF_DIMS ),
						Field<Geometry<TG,IL+IR> ,
						BiOp<WEDGE,Field<Geometry<TG, IL>, TL> ,
						Field<Geometry<TG, IR>, TR> > >
						,Zero>::type
						(lhs, rhs)))

template<typename TG, int IL, typename TL> inline  //
auto operator-(Field<Geometry<TG, IL>, TL> const & f)
DECL_RET_TYPE(
		( Field<Geometry<TG, IL>,
				UniOp<NEGATE,Field<Geometry<TG, IL>, TL> > > (f)))

template<typename TGeo, typename TL, typename TR> inline auto //
operator+(Field<TGeo, TL> const & lhs, Field<TGeo, TR> const & rhs)
DECL_RET_TYPE(
		( Field<TGeo ,
				BiOp<PLUS,Field<TGeo, TL> , Field<TGeo, TR> > > (lhs, rhs)))

template<typename TGeo, typename TL, typename TR> inline auto //
operator-(Field<TGeo, TL> const & lhs, Field<TGeo, TR> const & rhs)
DECL_RET_TYPE(
		( Field<TGeo , BiOp<MINUS,
				Field<TGeo, TL> ,
				Field<TGeo, TR> > > (lhs, rhs)))

template<typename TG, int IL, typename TL, typename TR> inline auto //
operator*(Field<Geometry<TG, IL>, TL> const & lhs,
		Field<Geometry<TG, 0>, TR> const & rhs)
		DECL_RET_TYPE(
				(Field<Geometry<TG,IL >,
						BiOp<MULTIPLIES,Field<Geometry<TG, IL>, TL>,
						Field<Geometry<TG,0>, TR> > > (lhs, rhs))
		)

template<typename TG, int IR, typename TL, typename TR> inline auto //
operator*(Field<Geometry<TG, 0>, TL> const & lhs,
		Field<Geometry<TG, IR>, TR> const & rhs)
		DECL_RET_TYPE(
				(Field<Geometry<TG,IR >,
						BiOp<MULTIPLIES,Field<Geometry<TG, 0>, TL>,
						Field<Geometry<TG,IR>, TR> > > (lhs, rhs))
		)

template<typename TG, typename TL, typename TR> inline auto //
operator*(Field<Geometry<TG, 0>, TL> const & lhs,
		Field<Geometry<TG, 0>, TR> const & rhs)
		DECL_RET_TYPE(
				(Field<Geometry<TG,0 >,
						BiOp<MULTIPLIES,Field<Geometry<TG, 0>, TL>,
						Field<Geometry<TG,0>, TR> > > (lhs, rhs))
		)

template<typename TG, typename TL, typename TR> inline auto //
operator*(Field<TG, TL> const & lhs, TR const & rhs)
DECL_RET_TYPE(
		(Field<TG,BiOp<MULTIPLIES,Field<TG,TL>,TR > > (lhs, rhs)))

template<typename TG, typename TL, int NR, typename TR> inline auto //
operator*(Field<TG, TL> const & lhs,
		nTuple<NR, TR> const & rhs)
				DECL_RET_TYPE(
						(Field<TG,BiOp<MULTIPLIES,Field<TG,TL>,nTuple<NR,TR> > > (lhs, rhs)))

template<typename TG, typename TL, typename TR> inline auto //
operator*(TL const & lhs, Field<TG, TR> const & rhs)
DECL_RET_TYPE((Field<TG,BiOp<MULTIPLIES,TL,Field<TG,TR> > > (lhs, rhs)))

template<int NL, typename TG, typename TL, typename TR> inline auto //
operator*(nTuple<NL, TL> const & lhs,
		Field<TG, TR> const & rhs)
				DECL_RET_TYPE((Field<TG,BiOp<MULTIPLIES,nTuple<NL,TL>,Field<TG,TR> > > (lhs, rhs)))

template<typename TG, int IL, typename TL, typename TR> inline auto //
operator/(Field<Geometry<TG, IL>, TL> const & lhs,
		Field<Geometry<TG, 0>, TR> const & rhs)
		DECL_RET_TYPE(
				(Field<Geometry<TG,IL >,
						BiOp<DIVIDES,Field<Geometry<TG, IL>, TL>,
						Field<Geometry<TG, 0>, TR> > > (lhs, rhs))
		)
template<typename TG, typename TL, typename TR> inline auto //
operator/(Field<Geometry<TG, 0>, TL> const & lhs,
		Field<Geometry<TG, 0>, TR> const & rhs)
		DECL_RET_TYPE(
				(Field<Geometry<TG,0 >,
						BiOp<DIVIDES,
						Field<Geometry<TG, 0>, TL>,
						Field<Geometry<TG, 0>, TR> > > (lhs, rhs))
		)
template<typename TG, int IL, typename TL, typename TR> inline auto //
operator/(Field<Geometry<TG, IL>, TL> const & lhs, TR rhs)
DECL_RET_TYPE(
		(Field<Geometry<TG,IL >,
				BiOp<DIVIDES,Field<Geometry<TG, IL>, TL>, TR > > (lhs, rhs))
)

template<typename TGeo, typename TL, typename TR> inline auto //
operator==(Field<TGeo, TL> const & lhs, Field<TGeo, TR> const & rhs)
DECL_RET_TYPE((lhs-rhs))

template<typename TG, typename TL, typename TR> inline auto //
Cross(Field<Geometry<TG, 0>, TL> const & lhs,
		Field<Geometry<TG, 0>, TR> const & rhs)
		DECL_RET_TYPE(
				(Field<Geometry<TG,0> ,
						BiOp<CROSS,Field<Geometry<TG, 0>, TL> ,
						Field<Geometry<TG, 0>, TR> > >(lhs, rhs)))

template<typename TG, typename TL, typename TR> inline auto //
Cross(Field<Geometry<TG, 0>, TL> const & lhs, nTuple<3, TR> const & rhs)
DECL_RET_TYPE(
		(Field<Geometry<TG,0> ,
				BiOp<CROSS,Field<Geometry<TG, 0>, TL> ,
				nTuple<3,TR> > >(lhs, rhs)))

template<typename TG, typename TL, typename TR> inline auto //
Cross(nTuple<3, TL> const & lhs, Field<Geometry<TG, 0>, TR> const & rhs)
DECL_RET_TYPE(
		(Field<Geometry<TG,0> ,
				BiOp<CROSS,nTuple<3,TL> ,
				Field<Geometry<TG, 0>, TR> > >(lhs, rhs)))

template<typename TG, typename TL, typename TR> inline auto //
Dot(Field<Geometry<TG, 0>, TL> const & lhs,
		Field<Geometry<TG, 0>, TR> const & rhs)
		DECL_RET_TYPE(
				(Field<Geometry<TG,0> ,
						BiOp<DOT,Field<Geometry<TG, 0>, TL> ,
						Field<Geometry<TG, 0>, TR> > >(lhs, rhs)))

template<typename TG, typename TL, typename TR> inline auto //
Dot(nTuple<3, TL> const & lhs, Field<Geometry<TG, 0>, TR> const & rhs)
DECL_RET_TYPE(
		(Field<Geometry<TG,0> ,
				BiOp<DOT,nTuple<3,TL> ,
				Field<Geometry<TG, 0>, TR> > >(lhs, rhs)))

template<typename TG, typename TL, typename TR> inline auto //
Dot(Field<Geometry<TG, 0>, TL> const & lhs, nTuple<3, TR> const & rhs)
DECL_RET_TYPE(
		(Field<Geometry<TG,0> ,
				BiOp<DOT,Field<Geometry<TG, 0>, TL> ,
				nTuple<3, TR> > >(lhs, rhs)))

}
// namespace simpla
#endif /* VECTOR_CALCULUS_H_ */
