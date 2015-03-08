/*
 * demo_pic.h
 *
 *  Created on: 2014年11月21日
 *      Author: salmon
 */

#ifndef EXAMPLE_USE_CASE_PIC_DEMO_PIC_H_
#define EXAMPLE_USE_CASE_PIC_DEMO_PIC_H_

#include "../../../core/utilities/utilities.h"
#include "../../../core/particle/particle_engine.h"
using namespace simpla;

namespace simpla
{

struct PICDemo
{
	typedef PICDemo this_type;
	typedef Vec3 coordinates_type;
	typedef Vec3 vector_type;
	typedef Real scalar_type;

//	SP_DEFINE_POINT_STRUCT(Point_s,
//			coordinates_type ,x,
//			Vec3, v,
//			Real, f )
//	SP_DEFINE_POINT_STRUCT(Point_s,
//			double[3] ,x
//	)
//
	struct Point_s
	{
		nTuple<double, 3> x;
		static DataType datatype()
		{
			auto d_type = DataType::create_opaque_type<Point_s>("Point_s");
			d_type.push_back(make_datatype<nTuple<double, 3>>(), "x",
					offsetof(Point_s, x));
			;
			return std::move(d_type);
		}
	};

	SP_DEFINE_PROPERTIES(
			Real, mass,
			Real, charge,
			Real, temperature
	)

private:
	Real m_cmr_, m_q_kT_;
public:

	PICDemo() :
			m_mass(1.0), m_charge(1.0), m_temperature(1.0)
	{
		update();
	}

	void update()
	{
		DEFINE_PHYSICAL_CONST
		m_cmr_ = m_charge / m_mass;
		m_q_kT_ = m_charge / (m_temperature * boltzmann_constant);
	}

	~PICDemo()
	{
	}

	static std::string get_type_as_string()
	{
		return "PICDemo";
	}

	template<typename TE, typename TB>
	void next_timestep(Point_s const* p0, Real dt, TE const &fE,
			TB const & fB) const
	{
//		p1->x += p0->v * dt * 0.5;
//
//		auto B = fB(p0->x);
//		auto E = fE(p0->x);
//
//		Vec3 v_;
//
//		auto t = B * (cmr_ * dt * 0.5);
//
//		p1->v += E * (cmr_ * dt * 0.5);
//
//		v_ = p0->v + cross(p1->v, t);
//
//		v_ = cross(v_, t) / (dot(t, t) + 1.0);
//
//		p1->v += v_;
//		auto a = (-dot(E, p1->v) * q_kT_ * dt);
//		p1->w = (-a + (1 + 0.5 * a) * p1->w) / (1 - 0.5 * a);
//
//		p1->v += v_;
//		p1->v += E * (cmr_ * dt * 0.5);
//
//		p1->x += p1->v * dt * 0.5;

	}

	static inline Point_s push_forward(coordinates_type const & x,
			Vec3 const &v, Real f = 1.0)
	{
		return std::move(Point_s(
		{ x }));
	}

	static inline auto pull_back(Point_s const & p)
	DECL_RET_TYPE((std::make_tuple(p.x )))
};

}  // namespace simpla

#endif /* EXAMPLE_USE_CASE_PIC_DEMO_PIC_H_ */
