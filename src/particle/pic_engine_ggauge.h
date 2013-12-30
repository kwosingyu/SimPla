/*
 * pic_engine_ggauge.h
 *
 *  Created on: 2013年10月23日
 *      Author: salmon
 */

#ifndef PIC_ENGINE_GGAUGE_H_
#define PIC_ENGINE_GGAUGE_H_

#include "../fetl/fetl.h"
#include "../fetl/ntuple_ops.h"
#include "../physics/constants.h"
namespace simpla
{

template<typename > class PICEngineBase;

template<typename TM, int NMATE = 8>
struct PICEngineGGauge: public PICEngineBase<TM>
{

	Real cmr_, q_, T_, vT_;

	Real cosdq[NMATE], sindq[NMATE];
public:
	typedef PICEngineBase<TM> base_type;
	typedef PICEngineGGauge<TM, NMATE> this_type;
	typedef TM mesh_type;

	DEFINE_FIELDS (mesh_type)

	struct Point_s
	{
		coordinates_type x;
		Vec3 v;
		Real f;
		nTuple<NMATE, scalar_type> w;

		static std::string DataTypeDesc()
		{
			std::ostringstream os;
			os

			<< "H5T_COMPOUND {          "

			<< "   H5T_ARRAY { [3] H5T_NATIVE_DOUBLE}    \"x\" : " << (offsetof(Point_s, x)) << ";"

			<< "   H5T_ARRAY { [3] H5T_NATIVE_DOUBLE}    \"v\" :  " << (offsetof(Point_s, v)) << ";"

			<< "   H5T_NATIVE_DOUBLE    \"f\" : " << (offsetof(Point_s, f)) << ";"

			<< "   H5T_ARRAY { [" << NMATE << "] H5T_NATIVE_DOUBLE}    \"w\" :  " << (offsetof(Point_s, w)) << ";"

			<< "}";

			return os.str();
		}
	};

	PICEngineGGauge(mesh_type const &pmesh)
			: base_type(pmesh), cmr_(1.0), q_(1.0), T_(1.0), vT_(1.0)
	{

	}
	virtual ~PICEngineGGauge()
	{
	}

	size_t GetAffectedRegion() const override
	{
		return 4;
	}
	static inline std::string TypeName()
	{
		return "GGauge" + ToString(NMATE);
	}
	std::string GetTypeAsString() const override
	{
		return this_type::TypeName();
	}

	void Deserialize(LuaObject const &obj) override
	{
		base_type::Deserialize(obj);

		obj["T"].as<Real>(&T_);

		Update();
	}
	void Update() override
	{
		cmr_ = base_type::q_ / base_type::m_;
		q_ = base_type::q_;

		vT_ = std::sqrt(2.0 * T_ / base_type::m_);

		constexpr Real theta = TWOPI / static_cast<Real>(NMATE);

		for (int i = 0; i < NMATE; ++i)
		{
			sindq[i] = std::sin(theta * i);
			cosdq[i] = std::cos(theta * i);
		}

	}
	std::ostream & Serialize(std::ostream & os) const override
	{

		os << "Engine = 'GGague" << NMATE << "' " << " , ";

		base_type::Serialize(os);

		return os;
	}

	static Point_s DefaultValue()
	{
		Point_s p;
		p.f = 1.0;
		return std::move(p);
	}

	template<typename TB, typename TE, typename ... Others>
	inline void NextTimeStep(Point_s * p, Real dt, TB const & B, TE const &E, Others const &...others) const
	{
		RVec3 B0 = real(B.mean(p->x));
		Real BB = InnerProduct(B0, B0);

		Real Bs = std::sqrt(BB);
		Vec3 v0, v1, r0, r1;
		Vec3 Vc;
		Vc = (InnerProduct(p->v, B0) * B0) / BB;
		v1 = Cross(p->v, B0 / Bs);
		v0 = -Cross(v1, B0 / Bs);
		r0 = -Cross(v0, B0) / (cmr_ * BB);
		r1 = -Cross(v1, B0) / (cmr_ * BB);

		for (int ms = 0; ms < NMATE; ++ms)
		{
			Vec3 v, r;
			v = Vc + v0 * cosdq[ms] + v1 * sindq[ms];
			r = (p->x + r0 * cosdq[ms] + r1 * sindq[ms]);
			p->w[ms] += 0.5 * InnerProduct(E(r), v) * dt;
		}

		Vec3 t, V_;

		t = B0 * cmr_ * dt * 0.5;

		V_ = p->v + Cross(p->v, t);

		p->v += Cross(V_, t) / (InnerProduct(t, t) + 1.0) * 2.0;

		Vc = (InnerProduct(p->v, B0) * B0) / BB;

		p->x += Vc * dt * 0.5;

		v1 = Cross(p->v, B0 / Bs);
		v0 = -Cross(v1, B0 / Bs);
		r0 = -Cross(v0, B0) / (cmr_ * BB);
		r1 = -Cross(v1, B0) / (cmr_ * BB);

		for (int ms = 0; ms < NMATE; ++ms)
		{
			Vec3 v, r;
			v = Vc + v0 * cosdq[ms] + v1 * sindq[ms];
			r = (p->x + r0 * cosdq[ms] + r1 * sindq[ms]);
			p->w[ms] += 0.5 * InnerProduct(E(r), v) * q_ / T_ * dt;

		}
		p->x += Vc * dt * 0.5;
	}

	template<typename TV, typename TB, typename ... Others>
	inline typename std::enable_if<!is_ntuple<TV>::value, void>::type Collect(Point_s const &p,
	        Field<Geometry<mesh_type, 0>, TV>* n, TB const & B, Others const &... others) const
	{
		RVec3 B0 = real(B.mean(p.x));
		Real BB = InnerProduct(B0, B0);

		Real Bs = sqrt(BB);
		Vec3 v0, v1, r0, r1;
		Vec3 Vc;

		Vc = (InnerProduct(p.v, B0) * B0) / BB;

		v1 = Cross(p.v, B0 / Bs);
		v0 = -Cross(v1, B0 / Bs);
		r0 = -Cross(v0, B0) / (cmr_ * BB);
		r1 = -Cross(v1, B0) / (cmr_ * BB);

		for (int ms = 0; ms < NMATE; ++ms)
		{
			Vec3 v, r;
			r = (p.x + r0 * cosdq[ms] + r1 * sindq[ms]);

			n->Collect(p.w[ms], r);
		}

	}

	template<int IFORM, typename TV, typename TB, typename ...Others>
	inline void Collect(Point_s const &p, Field<Geometry<mesh_type, IFORM>, TV>* J, TB const & B,
	        Others const &... others) const
	{
		RVec3 B0 = real(B.mean(p.x));
		Real BB = InnerProduct(B0, B0);

		Real Bs = sqrt(BB);
		Vec3 v0, v1, r0, r1;
		Vec3 Vc;

		Vc = (InnerProduct(p.v, B0) * B0) / BB;

		v1 = Cross(p.v, B0 / Bs);
		v0 = -Cross(v1, B0 / Bs);
		r0 = -Cross(v0, B0) / (cmr_ * BB);
		r1 = -Cross(v1, B0) / (cmr_ * BB);
		for (int ms = 0; ms < NMATE; ++ms)
		{
			Vec3 v, r;
			v = Vc + v0 * cosdq[ms] + v1 * sindq[ms];
			r = (p.x + r0 * cosdq[ms] + r1 * sindq[ms]);

			J->Collect(v * p.w[ms] * p.f, r);
		}
	}

	template<typename TX, typename TV, typename TFun>
	inline Point_s Trans(TX const & x, TV const &v, TFun const & n, ...) const
	{
		Point_s p;
		p.x = x;
		p.v = v;
		p.f = n(x);

		return std::move(p);
	}

	template<typename TX, typename TV, typename ... Others>
	inline void Trans(TX const & x, TV const &v, Point_s * p, Others const &...) const
	{
		p->x = x;
		p->v = v;
	}

	template<typename TX, typename TV, typename ... Others>
	inline void InvertTrans(Point_s const &p, TX * x, TV *v, Others const &...) const
	{
		*x = p.x;
		*v = p.v;
	}

}
;

template<typename TM> std::ostream&
operator<<(std::ostream& os, typename PICEngineGGauge<TM>::Point_s const & p)
{
	os << "{ x= {" << p.x << "} , v={" << p.v << "}, f=" << p.f << " , w=" << p.w << " }";

	return os;
}

} // namespace simpla

#endif /* PIC_ENGINE_GGAUGE_H_ */