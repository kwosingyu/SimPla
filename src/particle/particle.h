/*
 * particle.h
 *
 *  Created on: 2012-11-1
 *      Author: salmon
 */

#ifndef PARTICLE_H_
#define PARTICLE_H_

#include <cstddef>
#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "../fetl/fetl.h"
# include "../fetl/field_rw_cache.h"

#include "../utilities/log.h"
#include "../utilities/lua_state.h"
#include "../utilities/memory_pool.h"
#include "../utilities/singleton_holder.h"
#include "../utilities/type_utilites.h"
#include "../io/data_stream.h"
#include "load_particle.h"
#include "save_particle.h"

#ifndef NO_STD_CXX
//need  libstdc++

#include <ext/mt_allocator.h>
template<typename T> using FixedSmallSizeAlloc=__gnu_cxx::__mt_alloc<T>;
#endif

namespace simpla
{
template<typename, typename > struct Field;

template<typename, int> struct Geometry;

template<typename TM> class ParticleBase;

//*******************************************************************************************************
template<class Engine>
class Particle: public Engine, public ParticleBase<typename Engine::mesh_type>
{

public:

	enum
	{
		IForm = 3
	};

	typedef Engine engine_type;

	typedef ParticleBase<typename Engine::mesh_type> base_type;

	typedef Particle<engine_type> this_type;

	typedef typename engine_type::mesh_type mesh_type;

	typedef typename engine_type::Point_s particle_type;

	typedef particle_type value_type;

	DEFINE_FIELDS(mesh_type)

	//container

	typedef std::list<value_type, FixedSmallSizeAlloc<value_type> > cell_type;

	typedef typename cell_type::allocator_type allocator_type;

	typedef std::vector<cell_type> container_type;

public:
	mesh_type const &mesh;

private:

	cell_type pool_;

	container_type data_;

	std::vector<container_type> mt_data_; // for sort

	std::string name_;

public:

	template<typename ...Args> Particle(mesh_type const & pmesh);

	virtual ~Particle();

	virtual std::string GetTypeAsString() const
	{
		return engine_type::GetTypeAsString();
	}

	size_t size() const
	{
		size_t res = 0;

		for (auto const & v : data_)
		{
			res += v.size();
		}
		return res;
	}

	/**
	 *  Dump particles to a continue memory block
	 *  !!!this is a heavy operation!!!
	 *
	 * @return <datapoint , number of particles>
	 */
	std::pair<std::shared_ptr<value_type>, size_t> GetDataSet() const;

	allocator_type GetAllocator()
	{
		return pool_.get_allocator();
	}

	cell_type & operator[](size_t s)
	{
		return data_.at(s);
	}
	cell_type const & operator[](size_t s) const
	{
		return data_.at(s);
	}

	//***************************************************************************************************

	void Update() override;

	void DumpData(std::string const &path) const override;

	void Deserialize(LuaObject const &cfg) override;

	std::ostream & Serialize(std::ostream & os) const override;

	//***************************************************************************************************

	template<typename ...Args> inline void Insert(size_t s, Args const & ...args)
	{
		data_[s].emplace_back(engine_type::Trans(std::forward<Args const &>(args)...));
	}

	template<typename TFun, typename ... Args>
	void Function(TFun &fun, Args const& ... args) const;

	template<typename TFun, typename ... Args>
	void Function(TFun &fun, Args const& ... args);

	template<typename ... Args>
	void NextTimeStep(Real dt, Args const& ... args)
	{
		_NextTimeStep(dt, std::forward<Args const &>(args)...);
	}

	template<typename TJ, typename ... Args>
	void Collect(TJ * J, Args const & ... args) const
	{
		_Collect(J, std::forward<Args const &>(args)...);
	}

	void Sort() override
	{
		_Sort();
		base_type::Sort();
	}

	template<typename ... Args>
	void Collide(Args const& ... args)
	{
		Collide(std::forward<Args const &>(args)...);
	}
	template<typename ... Args>
	void Boundary(int flag, typename mesh_type::tag_type in, typename mesh_type::tag_type out, Real dt,
			Args const& ... args)
	{
		_Boundary(flag, in, out, dt, std::forward<Args const &>(args)...);
	}

private:
	template<typename ... Args> void _NextTimeStep(Real dt, Args const& ... args);

	template<typename TJ, typename ... Args> void _Collect(TJ * J, Args const & ... args) const;

	void _Sort();

	template<typename ... Args>
	void _Boundary(int flag, typename mesh_type::tag_type in, typename mesh_type::tag_type out, Real dt,
			Args const& ... args);

	template<typename ... Args> void _Collide(Args const& ... args);

public:

	//***************************************************************************************************
	// Interface :
	//       inhert virtual function
	//***************************************************************************************************
	void NextTimeStep(double dt, Form<1> const &E, Form<2> const &B) override
	{
		_NextTimeStep(dt, E, B);
	}
	void NextTimeStep(double dt, VectorForm<0> const &E, VectorForm<0> const &B) override
	{
		_NextTimeStep(dt, E, B);
	}

	void Boundary(int flag, typename mesh_type::tag_type in, typename mesh_type::tag_type out, double dt,
			Form<1> const &E, Form<2> const &B) override
	{
		_Boundary(flag, in, out, dt, E, B);
	}
	void Boundary(int flag, typename mesh_type::tag_type in, typename mesh_type::tag_type out, double dt,
			VectorForm<0> const &E, VectorForm<0> const &B) override
	{
		_Boundary(flag, in, out, dt, E, B);
	}

	void Collect(Form<0> * n, Form<1> const &E, Form<2> const &B) const override
	{
		_Collect(n, E, B);
	}
	void Collect(Form<1> * J, Form<1> const &E, Form<2> const &B) const override
	{
		_Collect(J, E, B);
	}
	void Collect(Form<2> * J, Form<1> const &E, Form<2> const &B) const override
	{
		_Collect(J, E, B);
	}
	void Collect(VectorForm<0> * J, Form<1> const &E, Form<2> const &B) const override
	{
		_Collect(J, E, B);
	}
	void Collect(VectorForm<1> * P, Form<1> const &E, Form<2> const &B) const override
	{
		_Collect(P, E, B);
	}
	void Collect(VectorForm<2> * P, Form<1> const &E, Form<2> const &B) const override
	{
		_Collect(P, E, B);
	}

	void Collect(TensorForm<0> * P, Form<1> const &E, Form<2> const &B) const override
	{
		_Collect(P, E, B);
	}

	void Collide(Real dt, ParticleBase<mesh_type> *p) override
	{
		_Collide(dt, p);
	}

	/**
	 *  resort particles in cell 's', and move out boundary particles to 'dest' container
	 * @param
	 */
	void Resort(index_type s, container_type * dest = nullptr);

};

template<class Engine>
template<typename ...Args> Particle<Engine>::Particle(mesh_type const & pmesh) :
		engine_type(pmesh), mesh(pmesh)
{
}

template<class Engine>
Particle<Engine>::~Particle()
{
}

template<class Engine>
void Particle<Engine>::Deserialize(LuaObject const &cfg)
{
	Update();

	LoadParticle(cfg, this);

	base_type::Deserialize(cfg);

}
template<class Engine>
std::pair<std::shared_ptr<typename Engine::Point_s>, size_t> Particle<Engine>::GetDataSet() const
{
	size_t num = size();

	std::shared_ptr<value_type> res = (MEMPOOL.allocate_shared_ptr<value_type>(num));

	value_type * it = res.get();

	for (auto const & l : data_)
	{
		it = std::copy(l.begin(), l.end(), it);
	}

	return std::make_pair(res, num);

}

template<class Engine>
std::ostream & Particle<Engine>::Serialize(std::ostream & os) const
{
	os << "{ ";

	engine_type::Serialize(os);

	os << ",\n"

	<< "\tData = " << Data(*this, engine_type::name_)

	<< "} ";

	return os;
}

template<typename TM>
std::ostream & operator<<(std::ostream & os, Particle<TM> const &self)
{
	return self.Serialize(os);
}

template<class Engine>
void Particle<Engine>::Update()
{
	if (data_.size() < mesh.GetNumOfElements(IForm))
		data_.resize(mesh.GetNumOfElements(IForm), cell_type(GetAllocator()));

	const unsigned int num_threads = std::thread::hardware_concurrency();

	mt_data_.resize(num_threads);

	for (auto & d : mt_data_)
	{
		d.resize(mesh.GetNumOfElements(IForm), cell_type(GetAllocator()));
	}
}

template<class Engine>
void Particle<Engine>::DumpData(std::string const &path) const
{
	LOGGER << Data(*this, base_type::GetName());
}

template<class Engine>
void Particle<Engine>::Resort(index_type src, container_type *other)
{
	if (other == nullptr)
		other = &(this->data_);
	auto & cell = this->data_[src];
	auto pt = cell.begin();
	while (pt != cell.end())
	{
		auto p = pt;
		++pt;

		auto dest = this->mesh.SearchCell(src, &(p->x[0]));

		if (dest != src)
		{
			(*other)[dest].splice((*other)[dest].begin(), cell, p);
		}

	}
}

template<class Engine>
void Particle<Engine>::_Sort()
{

	Update();

	if (base_type::IsSorted())
		return;

//LOGGER << flush << indent << "Sort Particle [" << this->GetName() << ":" << this->GetTypeAsString() << "]";

	const unsigned int num_threads = std::thread::hardware_concurrency();

	try
	{

		std::vector<std::thread> threads;

		for (unsigned int thread_id = 0; thread_id < num_threads; ++thread_id)
		{
			threads.emplace_back(std::thread(

			[this](unsigned int t_num,unsigned int t_id )
			{
				this->mesh._Traversal(t_num, t_id, this->IForm,
						[this,t_id](index_type const &src)
						{	Resort(src,&(this->mt_data_[t_id]));});
			}

			, num_threads, thread_id)

			);
		}

		for (auto & t : threads)
		{
			t.join();
		}

	} catch (std::exception const & e)
	{
		ERROR << e.what();

	}

	try
	{
		std::vector<std::thread> threads2;

		for (int thread_id = 0; thread_id < num_threads; ++thread_id)
		{
			threads2.emplace_back(std::thread(

			[this]( int t_num, int t_id)
			{
				this->mesh._Traversal(t_num, t_id, this->IForm,

						[&](index_type const &s)
						{
							for (int i = 0; i < t_num; ++i)
							{
								this->data_[s].splice(this->data_[s].begin(),this->mt_data_[i][s] );
							}
						}
				);

			}

			, num_threads, thread_id));
		}

		for (auto & t : threads2)
		{
			t.join();
		}
	} catch (std::exception const & e)
	{
		ERROR << e.what();

	}
}

template<class Engine>
template<typename ...Args>
void Particle<Engine>::_NextTimeStep(Real dt, Args const& ... args)
{
	if (data_.empty())
	{
		WARNING << "Particle [" << engine_type::name_ << "] is not initialized!";
		return;
	}

	Sort();

	base_type::NextTimeStep(dt, std::forward<Args const&>(args) ...);

	const unsigned int num_threads = std::thread::hardware_concurrency();

	std::vector<std::thread> threads;

	for (unsigned int thread_id = 0; thread_id < num_threads; ++thread_id)
	{
		threads.emplace_back(std::thread(

		[this,num_threads, thread_id,dt]( Cache<const Args> ... args_c2)
		{
			this->mesh._Traversal(num_threads, thread_id, this->IForm,

					[&](index_type const &s)
					{

						for (auto & p : this->data_[s])
						{
							RefreshCache(s,args_c2...);

							engine_type::NextTimeStep(&p, dt, *args_c2...);
						}
					}
			);

		},

		Cache<const Args >(args , engine_type::GetAffectedRegion())...));
	}

	for (auto & t : threads)
	{
		t.join();
	}

	Sort();

}

template<class Engine>
template<typename TJ, typename ...Args>
void Particle<Engine>::_Collect(TJ * J, Args const & ... args) const
{
	if (data_.empty())
	{
		WARNING << "Particle [" << engine_type::name_ << "] is not initialized!";
		return;
	}

	if (!base_type::IsSorted())
		ERROR << "Particles are not sorted!";

	const unsigned int num_threads = std::thread::hardware_concurrency();

	std::vector<std::thread> threads;

	for (unsigned int thread_id = 0; thread_id < num_threads; ++thread_id)
	{

		/***
		 *  @NOTICE std::thread  accept parameter by VALUE!!!
		 *
		 *     NOT by REFERENCE!!!!
		 *
		 *
		 *
		 */

		threads.emplace_back(

		std::thread(

		[this,num_threads, thread_id]( Cache<TJ*> J_c2, Cache<const Args> ... args_c2)
		{

			this->mesh._Traversal(num_threads, thread_id, this->IForm,

					[&](index_type const &s)
					{
						RefreshCache(s,J_c2,args_c2...);

						for (auto const& p : this->data_[s])
						{
							engine_type::Collect(p, &(*J_c2) , *args_c2...);
						}

						FlushCache(J_c2,args_c2...);

					}
			);
		}

		, Cache<TJ*>(J, engine_type::GetAffectedRegion())

		, Cache<const Args >(args ,engine_type::GetAffectedRegion())...)

		);
	}

	for (auto & t : threads)
	{
		t.join();
	}

}

template<class Engine>
template<typename TFun, typename ...Args>
void Particle<Engine>::Function(TFun &fun, Args const& ... args)
{
	if (data_.empty())
	{
		WARNING << "Particle [" << engine_type::name_ << "] is not initialized!";
		return;
	}

	if (!base_type::IsSorted())
		ERROR << "Particles are not sorted!";

	const unsigned int num_threads = std::thread::hardware_concurrency();

	std::vector<std::thread> threads;

	for (unsigned int thread_id = 0; thread_id < num_threads; ++thread_id)
	{
		threads.emplace_back(

		std::thread(

		[this,num_threads, thread_id]( Cache<const Args>&& ... args_c2)
		{

			this->mesh._Traversal(num_threads, thread_id, this->IForm,

					[&](index_type const &s)
					{

						RefreshCache(s, args_c2...);

						for (auto const& p : this->data_[s])
						{
							fun(p, *args_c2...);
						}

					}
			);
		}

		, Cache<const Args >(args ,engine_type::GetAffectedRegion())...)

		);
	}

	for (auto & t : threads)
	{
		t.join();
	}

}

template<class Engine>
template<typename ... Args>
void Particle<Engine>::_Boundary(int flag, typename mesh_type::tag_type in, typename mesh_type::tag_type out, Real dt,
		Args const &... args)
{
	auto selector = mesh.tags().template BoundarySelector<VERTEX>(in, out);

// @NOTE: difficult to parallism

	auto fun = [&](index_type src)
	{
		if(!selector(src)) return;

		auto & cell = this->data_[src];

		auto pt = cell.begin();

		while (pt != cell.end())
		{
			auto p = pt;
			++pt;

			index_type dest=src;
			if (flag == base_type::REFELECT)
			{
				coordinates_type x;

				nTuple<3,Real> v;

				Engine::InvertTrans(*p,&x,&v,std::forward<Args const &>(args)...);

				dest=this->mesh.Refelect(src,dt,&x,&v);

				Engine::Trans(x,v,&(*p),std::forward<Args const &>(args)...);
			}

			if (dest != src)
			{
				data_[dest].splice(data_[dest].begin(), cell, p);
			}
			else
			{
				cell.erase(p);
			}

		}
	};

// @NOTE: is a simple implement, need  parallism

	mesh.SerialTraversal(VERTEX, fun);

}
template<class Engine> template<typename ... Args>
void Particle<Engine>::_Collide(Args const &... args)
{
	UNIMPLEMENT;
}

//*******************************************************************************************************
template<typename TM>
struct PICEngineBase
{

protected:
	Real m_, q_;
	std::string name_;
public:
	typedef TM mesh_type;

public:

	mesh_type const &mesh;

	PICEngineBase(mesh_type const &pmesh) :
			mesh(pmesh), m_(1.0), q_(1.0), name_("unnamed")
	{

	}
	virtual ~PICEngineBase()
	{
	}

	virtual std::string GetTypeAsString() const
	{
		return "unknown";
	}

	virtual size_t GetAffectedRegion() const
	{
		return 2;
	}

	inline Real GetMass() const
	{
		return m_;
	}

	inline Real GetCharge() const
	{
		return q_;
	}

	inline void SetMass(Real m)
	{
		m_ = m;
	}

	inline void SetCharge(Real q)
	{
		q_ = q;
	}

	virtual void Update()
	{
	}

	virtual void Deserialize(LuaObject const &vm)
	{
		m_ = vm["Mass"].as<Real>();
		q_ = vm["Charge"].as<Real>();
	}

	virtual std::ostream & Serialize(std::ostream & os) const
	{
		os

		<< "Name = " << name_ << " ,"

		<< "Mass = " << m_ << " , "

		<< "Charge = " << q_;

		return os;
	}

};

//*******************************************************************************************************

template<typename TParticleEngine>
std::shared_ptr<ParticleBase<typename TParticleEngine::mesh_type> > CreateParticle(
		typename TParticleEngine::mesh_type const & mesh)
{

	typedef Particle<TParticleEngine> particle_type;
	typedef typename TParticleEngine::mesh_type mesh_type;

	return std::dynamic_pointer_cast<ParticleBase<mesh_type> >(
			std::shared_ptr<ParticleBase<mesh_type> >(new particle_type(mesh)));
}

//*******************************************************************************************************

template<typename TM>
class ParticleBase
{

public:
	typedef TM mesh_type;

	DEFINE_FIELDS(mesh_type)

	enum
	{
		REFELECT, ABSORB
	};

	ParticleBase() :
			isSorted_(false), clock_(0)
	{
	}
	virtual ~ParticleBase()
	{
	}

	virtual void Update()
	{
	}

	virtual void DumpData(std::string const &path = "") const
	{

	}

	virtual void Deserialize(LuaObject const &cfg)
	{
	}

	virtual std::string GetTypeAsString() const
	{
		return "unknown";
	}

	virtual std::ostream & Serialize(std::ostream & os) const
	{
		return os;
	}

	bool IsSorted() const
	{
		return isSorted_;
	}

	Real GetClock() const
	{
		return clock_;
	}

	void SetClock(Real clock)
	{
		clock_ = clock;
	}

//interface
	virtual void NextTimeStep(double dt, Form<1> const &E, Form<2> const &B)
	{
		isSorted_ = false;
		clock_ += dt;
	}
	virtual void NextTimeStep(double dt, VectorForm<0> const &E, VectorForm<0> const &B)
	{
		isSorted_ = false;
		clock_ += dt;
	}

	virtual void Boundary(int flag, typename mesh_type::tag_type in, typename mesh_type::tag_type out, double dt,
			Form<1> const &E, Form<2> const &B)
	{
		UNIMPLEMENT;
	}
	virtual void Boundary(int flag, typename mesh_type::tag_type in, typename mesh_type::tag_type out, double dt,
			VectorForm<0> const &E, VectorForm<0> const &B)
	{
		UNIMPLEMENT;
	}
	virtual void Collide(Real dt, ParticleBase *)
	{
		UNIMPLEMENT;
	}

	virtual void Collect(Form<0> * n, Form<1> const &E, Form<2> const &B) const
	{
		UNIMPLEMENT;
	}
	virtual void Collect(Form<1> * J, Form<1> const &E, Form<2> const &B) const
	{
		UNIMPLEMENT;
	}
	virtual void Collect(Form<2> * J, Form<1> const &E, Form<2> const &B) const
	{
		UNIMPLEMENT;
	}
	virtual void Collect(VectorForm<0> * J, Form<1> const &E, Form<2> const &B) const
	{
		UNIMPLEMENT;
	}
	virtual void Collect(VectorForm<1> * P, Form<1> const &E, Form<2> const &B) const
	{
		UNIMPLEMENT;
	}
	virtual void Collect(VectorForm<2> * P, Form<1> const &E, Form<2> const &B) const
	{
		UNIMPLEMENT;
	}
	virtual void Collect(TensorForm<0> * P, Form<1> const &E, Form<2> const &B) const
	{
		UNIMPLEMENT;
	}
	virtual void Sort()
	{
		isSorted_ = true;
	}

	void SetName(std::string const & name)
	{
		name_(name);
	}
	std::string const &GetName() const
	{
		return name_;
	}

private:
	bool isSorted_;
	Real clock_;
	std::string name_;

};

//*******************************************************************************************************
template<typename TM>
class ParticleCollection: public std::map<std::string, std::shared_ptr<ParticleBase<TM> > >
{
public:
	typedef TM mesh_type;

	typedef ParticleBase<mesh_type> particle_type;

	typedef std::map<std::string, std::shared_ptr<particle_type> > base_type;

	typedef std::function<std::shared_ptr<particle_type>(mesh_type const &)> create_fun;

	typedef ParticleCollection<mesh_type> this_type;

private:
	std::map<std::string, create_fun> factory_;
public:

	mesh_type const & mesh;

	template<typename U>
	friend std::ostream & operator<<(std::ostream & os, ParticleCollection<U> const &self);

	ParticleCollection(mesh_type const & pmesh) :
			mesh(pmesh)
	{
	}
	~ParticleCollection()
	{
	}

	void RegisterFactory(std::string const &engine_name, create_fun const &fun)
	{
		factory_.emplace(engine_name, fun);
	}
	template<typename TEngine>
	void RegisterFactory(std::string engine_name = "")
	{
		if (engine_name == "")
			engine_name = TEngine::TypeName();

		RegisterFactory(engine_name, create_fun(&CreateParticle<TEngine>));
	}

	void Deserialize(LuaObject const &cfg);

	std::ostream & Serialize(std::ostream & os) const;

	template<typename PT>
	inline void Serialize(PT &vm) const
	{
		WARNING << "UNIMPLEMENT!!";
	}

	void Sort();

	void DumpData(std::string const & path = "") const;

	template<typename ... Args> void NextTimeStep(Args const & ... args);

	template<typename TJ, typename ... Args> void Collect(TJ *J, Args const & ... args) const;
};

template<typename TM>
void ParticleCollection<TM>::Deserialize(LuaObject const &cfg)
{
	if (cfg.empty())
		return;

	Logger logger(LOG_LOG);

	logger << "Load Particles " << endl << flush << indent;

	for (auto const &p : cfg)
	{
		std::string key;

		if (!p.first.is_number())
		{
			key = p.first.template as<std::string>();
		}
		else
		{
			p.second.GetValue("Name", &key);
		}

		std::string engine = p.second.at("Engine").template as<std::string>();

		auto it = factory_.find(engine);

		if (it != factory_.end())
		{
			auto t = it->second(mesh);

			t->Deserialize(p.second);

			this->emplace(key, t);
		}
		else
		{
			WARNING << "I do not know how to create \"" << key << "\" particle! [engine=" << engine << "]";

			return;
		}

	}

	logger << DONE;

}
template<typename TM>

std::ostream & ParticleCollection<TM>::Serialize(std::ostream & os) const
{

	os << "Particles={ \n";

	ContainerOutPut3(os, this->begin(), this->end(),

	[](std::ostream & oos, decltype(this->begin()) const &it)->std::ostream &
	{
		oos<<"\t";

		it->second->Serialize(oos);

		oos<<"\n";

		return oos;
	});

	os << "} \n";

	return os;
}

template<typename TM>
template<typename ... Args>
void ParticleCollection<TM>::NextTimeStep(Args const & ... args)
{
	for (auto & p : *this)
	{
		LOG_CMD2(("Move Particle [" + p.first + ":" + p.second->GetTypeAsString() + "]"),
				(p.second->NextTimeStep(args...)));
	}
}

template<typename TM>
template<typename TJ, typename ... Args>
void ParticleCollection<TM>::Collect(TJ *J, Args const & ... args) const
{
	for (auto & p : *this)
	{
		LOG_CMD2(("Collect particle [" + p.first + ":" + p.second->GetTypeAsString()

		+ "] to Form<" + ToString(TJ::IForm) + ","

		+ (is_ntuple<typename TJ::value_type>::value ? "Vector" : "Scalar") + ">!]"

		), (p.second->Collect(J, args...)));
	}
}
template<typename TM>
void ParticleCollection<TM>::Sort()
{
	for (auto & p : *this)
	{
		p.second->Sort();
	}
}

template<typename TM>
void ParticleCollection<TM>::DumpData(std::string const & path) const
{
	for (auto const &p : *this)
	{
		p.second->DumpData(path);
	}
}
template<typename TM>
std::ostream & operator<<(std::ostream & os, ParticleCollection<TM> const &self)
{
	return self.Serialize(os);
}

//******************************************************************************************************
template<typename TX, typename TV, typename FE, typename FB> inline
void BorisMethod(Real dt, Real cmr, FE const & fE, FB const &fB, TX *x, TV *v)
{
// @ref  Birdsall(1991)   p.62
// Bories Method

	(*x) += (*v) * 0.5 * dt;

	auto B = real(fB((*x)));
	auto E = real(fE((*x)));

	Vec3 v_;

	auto t = B * (cmr * dt * 0.5);

	(*v) += E * (cmr * dt * 0.5);

	v_ = (*v) + Cross((*v), t);

	(*v) += Cross(v_, t) * (2.0 / (Dot(t, t) + 1.0));

	(*v) += E * (cmr * dt * 0.5);

	(*x) += (*v) * 0.5 * dt;
}
}
// namespace simpla

#endif /* PARTICLE_H_ */
