/**
 * @file   cylindrical.h
 *
 *  created on: 2014-3-13
 *      Author: salmon
 */

#ifndef CORE_MESH_STRUCTURED_CORE_MESH_STRUCTURED_COORDINATES_CYLINDRICAL_H_
#define CORE_MESH_STRUCTURED_COORDINATES_CYLINDRICAL_H_

#include <iostream>
#include <utility>
#include <cmath>
#include "../../utilities/utilities.h"
#include "../../physics/constants.h"
#include "../../physics/physical_constants.h"
namespace simpla
{

/**
 *  @ingroup geometry
 *
 *  \brief  cylindrical coordinates \f$ \left(R Z \phi \right) \f$
 */
template<typename TTopology, size_t IPhiAxis = 2>
class CylindricalCoordinate: public TTopology, public enable_create_from_this<
										CylindricalCoordinate<TTopology,
												IPhiAxis>>
{
private:
	bool is_valid_ = false;
public:
	typedef TTopology topology_type;

	static constexpr size_t PhiAxis = (IPhiAxis) % 3;
	static constexpr size_t RAxis = (PhiAxis + 1) % 3;
	static constexpr size_t ZAxis = (PhiAxis + 2) % 3;

	typedef CylindricalCoordinate<topology_type, PhiAxis> this_type;

	static constexpr size_t ndims = topology_type::ndims;

	typedef Real scalar_type;

	typedef typename topology_type::coordinate_type coordinate_type;
	typedef typename topology_type::id_type id_type;
	typedef typename topology_type::index_type index_type;
	typedef typename topology_type::iterator iterator;

	typedef nTuple<Real, ndims> vector_type;
	typedef nTuple<Real, ndims> covector_type;

	CylindricalCoordinate(this_type const & rhs) = delete;

	CylindricalCoordinate() :
			topology_type()
	{
		xmin_ = coordinate_type( { 1, 0, 0 });

		xmax_ = coordinate_type( { 2, 1, TWOPI });

		inv_length_ = coordinate_type( { 1.0, 1.0, 1.0 / TWOPI });

		length_ = coordinate_type( { 2, 1, TWOPI });

		shift_ = coordinate_type( { 0, 0, 0 });
	}
	template<typename ... Args>
	CylindricalCoordinate(Args && ... args) :
			topology_type(std::forward<Args>(args)...)
	{
		load(std::forward<Args>(args)...);
	}

	~CylindricalCoordinate()
	{
	}
	static std::string get_type_as_string_static()
	{
		return "Cylindrical" + value_to_string(PhiAxis);
	}

	std::string get_type_as_string() const
	{
		return get_type_as_string_static();
	}

	//***************************************************************************************************
	// Geometric properties
	// Metric
	//***************************************************************************************************

	Real dt_ = 0.0;
	Real time0_ = 0.0;
	Real CFL_ = 0.5;
	// Time
	void next_timestep()
	{
		topology_type::next_timestep();
	}

	void time(Real p_time)
	{
		time0_ = p_time;
	}
	Real time() const
	{
		return static_cast<double>(topology_type::get_clock()) * dt_ + time0_;
	}

	void dt(Real p_dt)
	{
		dt_ = p_dt;
	}

	Real dt() const
	{
		return dt_;
	}

	coordinate_type xmin_ /* = { 1, 0, 0 }*/;

	coordinate_type xmax_ /*= { 2, 1, TWOPI }*/;

	coordinate_type inv_length_/* = { 1.0, 1.0, 1.0 / TWOPI }*/;

	coordinate_type length_/* = { 2, 1, TWOPI }*/;

	coordinate_type shift_/* = { 0, 0, 0 }*/;

	template<typename TDict>
	bool load(TDict const & dict)
	{

		if (topology_type::load(dict) && dict["Min"] && dict["Max"])
		{

			LOGGER << "Load Cylindrical Geometry ";

			extents(

			dict["Min"].template as<nTuple<Real, ndims>>(),

			dict["Max"].template as<nTuple<Real, ndims>>());

			CFL_ = dict["CFL"].template as<Real>(0.5);

			dt_ = dict["dt"].template as<Real>(1.0);

			return true;

		}

		WARNING << "Configure Error: no Min or Max ";

		return false;

	}

	template<typename OS>
	OS & print(OS &os) const
	{
		topology_type::print(os);

		os

		<< " Min = " << xmin_ << " ," << std::endl

		<< " Max  = " << xmax_ << "," << std::endl

		<< " dt  = " << dt_ << "," << std::endl;

		return os;
	}

	bool is_valid() const
	{
		return is_valid_ && topology_type::is_valid();
	}
	bool update();

	void updatedt(Real dx2 = 0.0)
	{
		DEFINE_PHYSICAL_CONST

		auto dx_ = dx();

		Real R0 = (xmin_[RAxis] + xmax_[RAxis]) * 0.5;

		dx2 += dx_[RAxis] * dx_[RAxis] + dx_[ZAxis] * dx_[ZAxis]
				+ R0 * R0 * dx_[PhiAxis] * dx_[PhiAxis];

		Real safe_dt = CFL_ * std::sqrt(dx2) / speed_of_light;

		if (dt_ > safe_dt)
		{
			dt_ = safe_dt;
		}
	}
	void updatedt(nTuple<Real, ndims> const & kimg)
	{
		updatedt(0.0);
	}
	void updatedt(nTuple<Complex, ndims> const & kimg)
	{
		Real dx2 = 0.0;

		Real R0 = (xmin_[RAxis] + xmax_[RAxis]) * 0.5;

		if (std::imag(kimg[RAxis]) > EPSILON)
		{
			dx2 += TWOPI * TWOPI
					/ (std::imag(kimg[RAxis]) * std::imag(kimg[RAxis]));
		}
		if (std::imag(kimg[ZAxis]) > EPSILON)
		{
			dx2 += TWOPI * TWOPI
					/ (std::imag(kimg[ZAxis]) * std::imag(kimg[ZAxis]));
		}
		if (std::imag(kimg[PhiAxis]) > EPSILON)
		{
			dx2 += R0 * R0
					/ (std::imag(kimg[PhiAxis]) * std::imag(kimg[PhiAxis]));
		}

		updatedt(dx2);

	}

	void extents(coordinate_type const & pmin, coordinate_type const & pmax)
	{

		xmin_ = pmin;
		xmax_ = pmax;
	}
	inline std::pair<coordinate_type, coordinate_type> extents() const
	{
		return std::move(std::make_pair(xmin_, xmax_));
	}

	inline coordinate_type dx(index_type s = 0UL) const
	{

		coordinate_type res;

		auto d = topology_type::dx();

		for (int i = 0; i < ndims; ++i)
		{
			res[i] = length_[i] * d[i];
		}

		return std::move(res);
	}
	//! @name Normalize coordiantes to  [0,1 )
	//!@{

	template<typename ... Args>
	inline coordinate_type coordinates(Args && ... args) const
	{
		return CoordinateFromTopology(
				topology_type::coordinates(std::forward<Args >(args)...));
	}

	coordinate_type CoordinateFromTopology(coordinate_type const &x) const
	{

		return coordinate_type( {

		x[0] * length_[0] + shift_[0],

		x[1] * length_[1] + shift_[1],

		x[2] * length_[2] + shift_[2]

		});

	}
	coordinate_type CoordinateToTopology(coordinate_type const &x) const
	{
		return coordinate_type( {

		(x[0] - shift_[0]) * inv_length_[0],

		(x[1] - shift_[1]) * inv_length_[1],

		(x[2] - shift_[2]) * inv_length_[2]

		});

	}
	template<typename ... Args>
	inline coordinate_type coordinates_local_to_global(Args && ... args) const
	{
		return CoordinateFromTopology(
				topology_type::coordinates_local_to_global(
						std::forward<Args >(args)...));
	}

	std::tuple<index_type, coordinate_type> coordinates_global_to_local(
			coordinate_type x,
			typename topology_type::index_type shift = 0UL) const
	{
		return std::move(
				topology_type::coordinates_global_to_local(
						std::move(CoordinateToTopology(x)), shift));
	}
	std::tuple<index_type, coordinate_type> coordinates_global_to_local_NGP(
			coordinate_type x, index_type shift = 0UL) const
	{
		return std::move(
				topology_type::coordinates_global_to_local_NGP(
						std::move(CoordinateToTopology(x)), shift));
	}
	//!@}
	//! @name Coordiantes convert Cylindrical <-> Cartesian
	//! \f$\left(r,z,\phi\right)\Longleftrightarrow\left(x,y,z\right)\f$
	//! @{

	static constexpr coordinate_type MapToCartesian(coordinate_type const &y)
	{
		coordinate_type x;

		/**
		 *  @note
		 * coordinates transforam
		 *
		 *  \f{eqnarray*}{
		 *		x & = & r\cos\phi\\
		 *		y & = & r\sin\phi\\
		 *		z & = & Z
		 *  \f}
		 *
		 */

		return std::move(coordinate_type( {

		y[RAxis] * std::cos(y[PhiAxis]),

		y[RAxis] * std::sin(y[PhiAxis]),

		y[ZAxis] }));
	}

	static inline coordinate_type MapFromCartesian(coordinate_type const &x)
	{
		coordinate_type y;
		/**
		 *  @note
		 *  coordinates transforam
		 *  \f{eqnarray*}{
		 *		r&=&\sqrt{x^{2}+y^{2}}\\
		 *		Z&=&z\\
		 *		\phi&=&\arg\left(y,x\right)
		 *  \f}
		 *
		 */
		y[ZAxis] = x[CARTESIAN_YAXIS];
		y[RAxis] = std::sqrt(
				x[CARTESIAN_XAXIS] * x[CARTESIAN_XAXIS]
						+ x[CARTESIAN_ZAXIS] * x[CARTESIAN_ZAXIS]);
		y[PhiAxis] = std::atan2(x[CARTESIAN_ZAXIS], x[CARTESIAN_XAXIS]);

		return std::move(y);
	}

	template<typename TV>
	std::tuple<coordinate_type, TV> push_forward(
			std::tuple<coordinate_type, TV> const & Z) const
	{
		return std::move(
				std::make_tuple(MapFromCartesian(std::get<0>(Z)),
						std::get<1>(Z)));
	}

	template<typename TV>
	std::tuple<coordinate_type, TV> pull_back(
			std::tuple<coordinate_type, TV> const & R) const
	{
		return std::move(
				std::make_tuple(MapToCartesian(std::get<0>(R)), std::get<1>(R)));
	}

	/**
	 *
	 *   transform vector from Cartesian to Cylindrical
	 *
	 *
	 * \verbatim
	 *
	 *     theta   y   r
	 *          \  |  /
	 *           \ | /
	 *            \|/------x
	 *          y  /
	 *          | /
	 *          |/)theta
	 *          0------x
	 *
	 * \endverbatim
	 *
	 * @param Z  \f$ \left(x,y,z\right),u=u_{x}\partial_{x}+u_{y}\partial_{y}+u_{z}\partial_{z} \f$
	 * @param CartesianZAxis
	 * @return  \f$ v=v_{r}\partial_{r}+v_{Z}\partial_{Z}+v_{\theta}/r\partial_{\theta} \f$
	 *
	 */
	template<typename TV>
	std::tuple<coordinate_type, nTuple<TV, ndims> > push_forward(
			std::tuple<coordinate_type, nTuple<TV, ndims> > const & Z) const
	{
		coordinate_type r = MapFromCartesian(std::get<0>(Z));

		auto const & v = std::get<1>(Z);

		nTuple<TV, ndims> u;

		Real c = std::cos(r[PhiAxis]), s = std::sin(r[PhiAxis]);

		u[ZAxis] = v[CARTESIAN_YAXIS];

		u[RAxis] = v[CARTESIAN_XAXIS] * c + v[CARTESIAN_ZAXIS] * s;

		u[PhiAxis] = (-v[CARTESIAN_XAXIS] * s + v[CARTESIAN_ZAXIS] * c)
				/ r[RAxis];

		return std::move(std::make_tuple(r, u));
	}
	/**
	 *
	 *   pull_back vector from Cylindrical  to Cartesian
	 *
	 *
	 * @param R  \f$ v=v_{r}\partial_{r}+v_{Z}\partial_{Z}+v_{\theta}/r\partial_{\theta} \f$
	 * @param CartesianZAxis
	 * @return  \f$ \left(x,y,z\right),u=u_{x}\partial_{x}+u_{y}\partial_{y}+u_{z}\partial_{z} \f$
	 *
	 */
	template<typename TV>
	std::tuple<coordinate_type, nTuple<TV, ndims> > pull_back(
			std::tuple<coordinate_type, nTuple<TV, ndims> > const & R) const
	{
		auto const & r = std::get<0>(R);
		auto const & u = std::get<1>(R);

		Real c = std::cos(r[PhiAxis]), s = std::sin(r[PhiAxis]);

		nTuple<TV, ndims> v;

		v[CARTESIAN_XAXIS] = u[RAxis] * c - u[PhiAxis] * r[RAxis] * s;
		v[CARTESIAN_ZAXIS] = u[RAxis] * s + u[PhiAxis] * r[RAxis] * c;
		v[CARTESIAN_YAXIS] = u[ZAxis];

		return std::move(std::make_tuple(MapToCartesian(r), v));
	}
//! @}

//	auto select(size_t iform, coordinate_type const & xmin,
//			coordinate_type const & xmax) const
//					DECL_RET_TYPE((this->topology_type::select(this->topology_type::select(iform), this->CoordinateToTopology(xmin),this->CoordinateToTopology(xmax))))
//
//	template<typename ...Args>
//	auto select(size_t iform,
//			Args && ...args) const
//					DECL_RET_TYPE((this->topology_type::select(iform,std::forward<Args >(args)...)))
//
//	template<typename TV>
//	TV const& Normal(index_type s, TV const & v) const
//	{
//		return v;
//	}
//
//	template<typename TV>
//	TV const& Normal(index_type s, nTuple<TV, 3> const & v) const
//	{
//		return v[topology_type::component_number(s)];
//	}
//
//	template<typename TV>
//	TV sample(std::integral_constant<size_t, VERTEX>, index_type s,
//			TV const &v) const
//	{
//		return v;
//	}
//
//	template<typename TV>
//	TV sample(std::integral_constant<size_t, VOLUME>, index_type s,
//			TV const &v) const
//	{
//		return v;
//	}
//
//	template<typename TV>
//	TV sample(std::integral_constant<size_t, EDGE>, index_type s,
//			nTuple<TV, 3> const &v) const
//	{
//		return v[topology_type::component_number(s)];
//	}
//
//	template<typename TV>
//	TV sample(std::integral_constant<size_t, FACE>, index_type s,
//			nTuple<TV, 3> const &v) const
//	{
//		return v[topology_type::component_number(s)];
//	}
//
//	template<size_t IFORM, typename TV>
//	TV sample(std::integral_constant<size_t, IFORM>, index_type s,
//			TV const & v) const
//	{
//		return v;
//	}
//
//	template<size_t IFORM, typename TV>
//	typename std::enable_if<(IFORM == EDGE || IFORM == FACE), TV>::type sample(
//			std::integral_constant<size_t, IFORM>, index_type s,
//			nTuple<TV, ndims> const & v) const
//	{
//		return Normal(s, v);
//	}

	//! @name Metric/coordinates  depend transform
	//! @{

	Real volume_[8] = { 1, // 000
			1, //001
			1, //010
			1, //011
			1, //100
			1, //101
			1, //110
			1 //111
			};
	Real inv_volume_[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };

	/**
	 *\verbatim
	 *                ^y
	 *               /
	 *        z     /
	 *        ^    /
	 *        |  110-------------111
	 *        |  /|              /|
	 *        | / |             / |
	 *        |/  |            /  |
	 *       100--|----------101  |
	 *        | m |           |   |
	 *        |  010----------|--011
	 *        |  /            |  /
	 *        | /             | /
	 *        |/              |/
	 *       000-------------001---> x
	 *
	 *\endverbatim
	 */

	constexpr Real HodgeStarVolumeScale(index_type s) const
	{
		return 1.0;
	}

	constexpr Real cell_volume(index_type s) const
	{
		return volume_[1] * volume_[2] * volume_[4] * coordinates(s)[RAxis];
	}

	constexpr scalar_type volume(index_type s) const
	{
		size_t n = topology_type::ele_suffix(s);
		return volume_[n]
				* (((n & (1UL << (ndims - PhiAxis - 1))) > 0) ?
						coordinates(s)[RAxis] : 1.0);
	}

	constexpr scalar_type inv_volume(index_type s) const
	{
		size_t n = topology_type::ele_suffix(s);
		return inv_volume_[n]
				/ (((n & (1UL << (ndims - PhiAxis - 1))) > 0) ?
						coordinates(s)[RAxis] : 1.0);
	}

	constexpr scalar_type dual_volume(index_type s) const
	{
		size_t n = topology_type::ele_suffix(topology_type::dual(s));
		return volume_[n]
				* (((n & (1UL << (ndims - PhiAxis - 1))) > 0) ?
						coordinates(s)[RAxis] : 1.0);
	}
	constexpr scalar_type inv_dual_volume(index_type s) const
	{
		size_t n = topology_type::ele_suffix(topology_type::dual(s));
		return inv_volume_[n]
				/ (((n & (1UL << (ndims - PhiAxis - 1))) > 0) ?
						coordinates(s)[RAxis] : 1.0);
	}
	//! @}
}
;

template<typename TTopology, size_t IPhiAxis>
bool CylindricalCoordinate<TTopology, IPhiAxis>::update()
{

	if (!topology_type::update())
		return false;

	DEFINE_PHYSICAL_CONST

	auto dims = topology_type::dimensions();

	if (xmin_[RAxis] < EPSILON || dims[RAxis] <= 1)
	{

		RUNTIME_ERROR(

				std::string(
						" illegal configure: Cylindrical R_min=0 or dims[R]<=1!!")

				+ " coordinates = ("

				+ " R=[ " + value_to_string(xmin_[RAxis]) + " , "
						+ value_to_string(xmax_[RAxis]) + "]"

						+ ", Z=[ " + value_to_string(xmin_[ZAxis]) + " , "
						+ value_to_string(xmax_[ZAxis]) + "]"

						+ ", Phi=[ " + value_to_string(xmin_[PhiAxis]) + " , "
						+ value_to_string(xmax_[PhiAxis]) + "] )"

						+ ", dimension = ("

						+ " R = " + value_to_string(dims[RAxis])

						+ ", Z = " + value_to_string(dims[RAxis])

						+ ", Phi =" + value_to_string(dims[PhiAxis]) + ")"

						);
	}

	for (int i = 0; i < ndims; ++i)
	{

		shift_[i] = xmin_[i];

		if ((xmax_[i] - xmin_[i]) < EPSILON || dims[i] <= 1)
		{

			xmax_[i] = xmin_[i];

			inv_length_[i] = 0.0;

			length_[i] = 0.0;

			volume_[1UL << (ndims - i - 1)] = 1.0;

			inv_volume_[1UL << (ndims - i - 1)] = 1.0;
		}
		else
		{

			length_[i] = (xmax_[i] - xmin_[i]);

			if (i == PhiAxis && length_[i] > TWOPI)
			{
				xmax_[i] = xmin_[i] + TWOPI;

				length_[i] = TWOPI;
			}

			inv_length_[i] = 1.0 / length_[i];

			volume_[1UL << (ndims - i - 1)] = length_[i];

			inv_volume_[1UL << (ndims - i - 1)] = inv_length_[i];

		}
	}

	/**
	 *\verbatim
	 *                ^y
	 *               /
	 *        z     /
	 *        ^    /
	 *        |  110-------------111
	 *        |  /|              /|
	 *        | / |             / |
	 *        |/  |            /  |
	 *       100--|----------101  |
	 *        | m |           |   |
	 *        |  010----------|--011
	 *        |  /            |  /
	 *        | /             | /
	 *        |/              |/
	 *       000-------------001---> x
	 *
	 *\endverbatim
	 */

	volume_[0] = 1;
	//		volume_[1] /* 001 */= dx_[0];
	//		volume_[2] /* 010 */= dx_[1];
	//		volume_[4] /* 100 */= dx_[2];

	volume_[3] /* 011 */= volume_[1] * volume_[2];
	volume_[5] /* 101 */= volume_[4] * volume_[1];
	volume_[6] /* 110 */= volume_[2] * volume_[4];

	volume_[7] /* 111 */= volume_[1] * volume_[2] * volume_[4];

	inv_volume_[0] = 1;
	//		inv_volume_[1] /* 001 */= inv_dx_[0];
	//		inv_volume_[2] /* 010 */= inv_dx_[1];
	//		inv_volume_[4] /* 100 */= inv_dx_[2];

	inv_volume_[3] /* 011 */= inv_volume_[1] * inv_volume_[2];
	inv_volume_[5] /* 101 */= inv_volume_[4] * inv_volume_[1];
	inv_volume_[6] /* 110 */= inv_volume_[2] * inv_volume_[4];

	inv_volume_[7] /* 111 */= inv_volume_[1] * inv_volume_[2] * inv_volume_[4];

	updatedt();

	return true;

}
template<typename TTopology, size_t IPhiAxis>
constexpr size_t CylindricalCoordinate<TTopology, IPhiAxis>::PhiAxis;

template<typename TTopology, size_t IPhiAxis>
constexpr size_t CylindricalCoordinate<TTopology, IPhiAxis>::RAxis;

template<typename TTopology, size_t IPhiAxis>
constexpr size_t CylindricalCoordinate<TTopology, IPhiAxis>::ZAxis;
}
// namespace simpla

#endif /* CORE_MESH_STRUCTURED_COORDINATES_CYLINDRICAL_H_ */
