/*
 * test_field.cpp
 *
 *  Created on: 2012-1-13
 *      Author: salmon
 */

#include <gtest/gtest.h>
#include <random>

#include "utilities/log.h"

#include "fetl/fetl.h"

#include "physics/constants.h"

#include "mesh/uniform_rect.h"

using namespace simpla;

DEFINE_FIELDS(UniformRectMesh)

template<typename TF>
class TestFETLBasicArithmetic: public testing::Test
{
protected:
	virtual void SetUp()
	{
		Log::Verbose(10);

		mesh.dt_ = 1.0;
		mesh.xmin_[0] = 0;
		mesh.xmin_[1] = 0;
		mesh.xmin_[2] = 0;
		mesh.xmax_[0] = 1.0;
		mesh.xmax_[1] = 1.0;
		mesh.xmax_[2] = 1.0;
		mesh.dims_[0] = 20;
		mesh.dims_[1] = 30;
		mesh.dims_[2] = 40;
		mesh.gw_[0] = 2;
		mesh.gw_[1] = 2;
		mesh.gw_[2] = 2;

		mesh.Update();

	}
public:
	typedef TF FieldType;
	typedef Form<0, Real> RScalarField;

	Mesh mesh;

};

typedef testing::Types<RZeroForm
		, ROneForm, RTwoForm, RThreeForm, CZeroForm,
		COneForm, CTwoForm, CThreeForm, VecZeroForm, VecOneForm, VecTwoForm
//		,VecThreeForm
//		,CVecZeroForm, CVecOneForm, CVecTwoForm,CVecThreeForm
> AllFieldTypes;

//, VecThreeForm

// test arithmetic.h
TYPED_TEST_CASE(TestFETLBasicArithmetic, AllFieldTypes);

TYPED_TEST(TestFETLBasicArithmetic,create_write_read){
{

	Log::Verbose(10);

	Mesh const & mesh = TestFixture::mesh;

	typename TestFixture::FieldType f( mesh );

	typename TestFixture::FieldType::value_type a; a= 1.0;

	for (size_t s = 0, e=f.size(); s < e; ++s)
	{
		f[s] = a*static_cast<double>(s);
	}

	for (size_t s = 0, e=f.size(); s < e; ++s)
	{
		typename TestFixture::FieldType::value_type res;
		res=a*static_cast<Real>(s);
		ASSERT_EQ(res,f[s])<<"idx=" << s;
	}

}
}

TYPED_TEST(TestFETLBasicArithmetic,assign){
{

	typename TestFixture::FieldType f1(TestFixture::mesh),f2(TestFixture::mesh);

	typedef typename TestFixture::FieldType::value_type value_type;

	value_type a; a = 3.0;

	std::fill(f1.begin(),f1.end(), a);

	for (auto p : f1)
	{
		ASSERT_EQ(a,p)<<"idx="<< p;
	}

	size_t s0=0;

	TestFixture::mesh.ForEach(TestFixture::FieldType::IForm,
			[&](typename TestFixture::FieldType::index_type const & s)
			{
				s0=s;
				f2[s]=a*static_cast<Real>(s);
			}
	);

	f1 += f2;

	TestFixture::mesh.ForEach(TestFixture::FieldType::IForm,

			[&a,&f1](typename TestFixture::FieldType::index_type const & s)
			{
				typename TestFixture::FieldType::value_type res;
				res=a+a*static_cast<Real>(s);
				ASSERT_EQ( res,f1[s])<<"idx="<< s;
			}

	);

	f1*=2.0;

	TestFixture::mesh.ForEach(TestFixture::FieldType::IForm,

			[&a,&f1](typename TestFixture::FieldType::index_type const & s)
			{
				typename TestFixture::FieldType::value_type res;
				res=(a+a*static_cast<Real>(s))*2.0;
				ASSERT_EQ( res,f1[s])<<"idx="<< s;
			}

	);
}
}
TYPED_TEST(TestFETLBasicArithmetic, constant_real){
{

	typename TestFixture::FieldType f1( TestFixture::mesh),f2(TestFixture::mesh),f3(TestFixture::mesh);

	Real a,b,c;
	a=1.0,b=-2.0,c=3.0;

	typedef typename TestFixture::FieldType::value_type value_type;

	value_type va,vb;

	va=2.0;vb=3.0;

	std::fill(f1.begin(),f1.end(), va);
	std::fill(f2.begin(),f2.end(), vb);

	f3 = -f1 *2.0 + f2 * c- f1/b
	;

	TestFixture::mesh.ForEach(TestFixture::FieldType::IForm,

			[&](typename TestFixture::FieldType::index_type const & s)
			{
				value_type res;
				res= - f1[s]*2.0 + f2[s] *c -f1[s]/b
				;
				ASSERT_EQ( res, f3[s]) << s;
			}
	);
}
}

TYPED_TEST(TestFETLBasicArithmetic, scalar_field){
{
	//FIXME  should test with non-uniform field

	typename TestFixture::FieldType f1( TestFixture::mesh),f2( TestFixture::mesh),
	f3( TestFixture::mesh),f4( TestFixture::mesh);

	typename TestFixture::RScalarField a( TestFixture::mesh);
	typename TestFixture::RScalarField b( TestFixture::mesh);
	typename TestFixture::RScalarField c( TestFixture::mesh);

	Real ra=1.0,rb=2.0,rc=3.0;
	typename TestFixture::FieldType::value_type va,vb,vc;

	va=ra;
	vb=rb;
	vc=rc;

	std::fill(a.begin(),a.end(), ra);
	std::fill(b.begin(),b.end(), rb);
	std::fill(c.begin(),c.end(), rc);

	size_t count=0;

	std::mt19937 gen;
	std::uniform_real_distribution<Real> uniform_dist(0, 1.0);

	TestFixture::mesh.ForEach(TestFixture::FieldType::IForm,
			[&](typename Mesh::index_type const & s)
			{
				f1[s]=va*uniform_dist(gen);
				f2[s]=vb*uniform_dist(gen);
				f3[s]=vc*uniform_dist(gen);
			}
	);

	f4= -(f1^a)-f2/b +f3*c;
//	Plus( Minus(Negate(Wedge(f1,a)),Divides(f2,b)),Multiplies(f3,c) )
	;
	/**           (+)
	 *           /   \
	 *         (-)    (*)
	 *        /   \    | \
	 *      (^)    (/) f1 c
	 *     /  \   /  \
	 *-f1      a f2   b
	 *
	 * */
	count =0;

	TestFixture::mesh.ForEach(TestFixture::FieldType::IForm,
			[&](typename Mesh::index_type const & s)
			{
				typename TestFixture::FieldType::value_type res;
				res=
				-f1[s]*ra
				-f2[s]/rb
				+f3[s]*rc
				;

				if(res!=f4[s])
				{
					++count;
				}
			}
	);
	EXPECT_EQ(0,count)<< "number of error points =" << count;

}
}
////// test vector_calculus.h
template<typename T>
class TestFETLVecAlgegbra: public testing::Test
{
protected:
	virtual void SetUp()
	{
		mesh.dt_ = 1.0;
		mesh.xmin_[0] = 0;
		mesh.xmin_[1] = 0;
		mesh.xmin_[2] = 0;
		mesh.xmax_[0] = 1.0;
		mesh.xmax_[1] = 1.0;
		mesh.xmax_[2] = 1.0;
		mesh.dims_[0] = 1;
		mesh.dims_[1] = 10;
		mesh.dims_[2] = 10;
		mesh.gw_[0] = 2;
		mesh.gw_[1] = 2;
		mesh.gw_[2] = 2;

		mesh.Update();
	}
public:
	Mesh mesh;
	typedef T value_type;
	typedef nTuple<3, value_type> Vec3;
	typedef Form<0, T> ScalarField;
	typedef Form<0, nTuple<3, T> > VectorField;
};

typedef testing::Types<double, Complex
		, nTuple<3, Real>
> VecFieldTypes;

TYPED_TEST_CASE(TestFETLVecAlgegbra, VecFieldTypes);

TYPED_TEST(TestFETLVecAlgegbra,vec_0_form){
{
	const Mesh& mesh = TestFixture::mesh;

	typename TestFixture::Vec3 vc1 =
	{	1.0, 2.0, 3.0};

	typename TestFixture::Vec3 vc2 =
	{	-1.0, 4.0, 2.0};

	typename TestFixture::Vec3 res_vec;

	res_vec = Cross(vc2,vc1);

	typename TestFixture::value_type res_scalar;

	res_scalar = Dot(vc1, vc2);

	typename TestFixture::ScalarField res_scalar_field(mesh);

	typename TestFixture::VectorField va(mesh), vb(mesh), res_vector_field(
			mesh);

	std::fill(va.begin(), va.end(), vc2);

	res_scalar_field = Dot(vc1, va);

	res_vector_field = Cross( va,vc1);

	res_scalar_field.ForEach (

			[&](typename TestFixture::ScalarField::value_type const & v)
			{
				ASSERT_EQ(res_scalar, v);
			}

	);

	res_vector_field.ForEach (

			[&](typename TestFixture::VectorField::value_type const & v)
			{
				ASSERT_EQ(res_vec , v);
			}

	);

}
}

template<typename TP>
class TestFETLDiffCalcuate: public testing::Test
{

protected:
	virtual void SetUp()
	{
		mesh.dt_ = 1.0;
		mesh.xmin_[0] = 0;
		mesh.xmin_[1] = 0;
		mesh.xmin_[2] = 0;
		mesh.xmax_[0] = 1.0;
		mesh.xmax_[1] = 1.0;
		mesh.xmax_[2] = 1.0;
		mesh.dims_[0] = 1;
		mesh.dims_[1] = 10;
		mesh.dims_[2] = 10;
		mesh.gw_[0] = 2;
		mesh.gw_[1] = 2;
		mesh.gw_[2] = 2;

		mesh.Update();

	}
public:

	Mesh mesh;

	typedef TP value_type;
	typedef Form<0, value_type> TZeroForm;
	typedef Form<1, value_type> TOneForm;
	typedef Form<2, value_type> TTwoForm;

	double RelativeError(double a, double b)
	{
		return (2.0 * fabs((a - b) / (a + b)));
	}

	void SetValue(double *v)
	{
		*v = 1.0;
	}

	void SetValue(Complex *v)
	{
		*v = Complex(1.0, 2.0);
	}

	template<int N, typename TV>
	void SetValue(nTuple<N, TV> *v)
	{
		for (size_t i = 0; i < N; ++i)
		{
			SetValue(&((*v)[i]));
		}
	}
};

typedef testing::Types<double
//		, Complex, nTuple<3, double>,
//		nTuple<3, nTuple<3, double>>
> PrimitiveTypes;

TYPED_TEST_CASE(TestFETLDiffCalcuate, PrimitiveTypes);

TYPED_TEST(TestFETLDiffCalcuate, curl_grad_eq_0){
{
	Mesh const & mesh = TestFixture::mesh;

	typename TestFixture::value_type v;

	TestFixture::SetValue(&v);

	typename TestFixture::TZeroForm sf(mesh);
	typename TestFixture::TTwoForm vf2(mesh);

	std::mt19937 gen;
	std::uniform_real_distribution<Real> uniform_dist(0, 1.0);

	std::fill(sf.begin(),sf.end(), v*0.0);
	std::fill(vf2.begin(),vf2.end(), v*0.0);

	mesh.ForEach(0,
			[&](typename Mesh::index_type const & s)
			{
				sf[s]=v*uniform_dist(gen);
			}
	);

//	mesh.UpdateCyCleBoundary(sf);

	vf2 = Curl(Grad(sf));

	size_t count=0;
	mesh.ForEach(2,
			[&](typename Mesh::index_type const & s)
			{
				count+=( abs(vf2[s])>1.0e-10)?1:0;
				ASSERT_NEAR(0.0, abs(vf2[s]),1.0e-10) << "idx=" << s;
			}
	);

	ASSERT_EQ(0,count)<< "number of non-zero points =" << count;
}
}

TYPED_TEST(TestFETLDiffCalcuate, div_curl_eq_0){
{

	Mesh const & mesh = TestFixture::mesh;

	typename TestFixture::TZeroForm sf(mesh);
	typename TestFixture::TOneForm vf1(mesh);
	typename TestFixture::TTwoForm vf2(mesh);

	typename TestFixture::value_type v;

	TestFixture::SetValue(&v);

	std::mt19937 gen;
	std::uniform_real_distribution<Real> uniform_dist(0, 1.0);

	for(auto p:vf2)
	{
		p=v*uniform_dist(gen);
	}

	vf1 = Curl(vf2);
	sf = Diverge( Curl(vf2));

	size_t count=0;

	mesh.ForEach(0,
			[&sf,&count](typename Mesh::index_type const &s)
			{
				count+=( abs(sf[s])>1.0e-15)?1:0;
				ASSERT_NEAR(0.0, abs(sf[s]),1.0e-14) << "idx=" << s;
			}
	);
	ASSERT_DOUBLE_EQ(0,count)<< "number of non-zero points =" << count;
}
}
